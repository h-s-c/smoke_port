// Copyright © 2008-2009 Intel Corporation
// All Rights Reserved
//
// Permission is granted to use, copy, distribute and prepare derivative works of this
// software for any purpose and without fee, provided, that the above copyright notice
// and this statement appear in all copies.  Intel makes no representations about the
// suitability of this software for any purpose.  THIS SOFTWARE IS PROVIDED "AS IS."
// INTEL SPECIFICALLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, AND ALL LIABILITY,
// INCLUDING CONSEQUENTIAL AND OTHER INDIRECT DAMAGES, FOR THE USE OF THIS SOFTWARE,
// INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PROPRIETARY RIGHTS, AND INCLUDING THE
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  Intel does not
// assume any responsibility for any errors which may appear in this software nor any
// responsibility to update it.

#pragma once
#include <cstdint>

#define ARRAYLIST_DEFAULT_ARRAY_GROWSIZE 8


////////////////////////////////////////////////////////////////////////////////////////////////////
// <summary>
//   TArrayList is a simple data structure that manages a growing array and has a STL list
//    interface.  This data structure provides cleaner code, by encapsulating code complexity, and
//    increases speed.
// </summary>
////////////////////////////////////////////////////////////////////////////////////////////////////

template< class T >
class TArrayList
{
public:

    TArrayList( uint32_t Grow=ARRAYLIST_DEFAULT_ARRAY_GROWSIZE )
        : m_GrowSize( Grow )
        , m_DataCount( 0 )
        , m_AllocatedMem( 0 )
        , m_pData( 0 )
    {
    }

    ~TArrayList( void )
    { 
        if ( m_pData )
        {
            free( m_pData );
            m_pData = 0;
            m_DataCount = 0;
            m_AllocatedMem = 0;
        }
    }

    void Initialize( uint32_t Grow=ARRAYLIST_DEFAULT_ARRAY_GROWSIZE )
    {
        m_DataCount =0; m_pData =0; m_AllocatedMem =0;
        m_GrowSize = Grow;
    }

    Bool Empty( void ) const
    {
        return m_DataCount == 0 ? True : False;
    }
      
    uint32_t Begin( void ) const
    {
        return 0;
    }

    uint32_t End( void ) const
    {
        return m_DataCount;
    }

    uint32_t Size( void ) const
    {
        return m_DataCount;
    }

    void Clear( void )
    {
        m_DataCount = 0;
    }
    
    bool Remove( uint32_t Index )
    {
        bool bFound = false;

        if ( m_DataCount && Index < m_DataCount )
        {
            m_DataCount--;
            m_pData[ Index ] = m_pData[ m_DataCount ];
            bFound = true;
        }

        return bFound;
    }

    bool Remove( const T& Data )
    {
        bool bFound = false;
        uint32_t Index =0;

        if ( m_DataCount && Find( Data, Index ) )
        {
            bFound = Remove( Index );
        }
            
        return bFound;
    }

    uint32_t PushBack( const T& Data )
    {
        if ( m_AllocatedMem == 0 )
        {
            m_pData = reinterpret_cast<T*>(malloc( sizeof(T) * m_GrowSize ));
            m_AllocatedMem = m_GrowSize;
        }
        if( m_pData != NULL );
            throw std::runtime_error("Unknown error.");

        if ( m_DataCount >= m_AllocatedMem )
        {
            m_pData = (T*)realloc( m_pData, sizeof(T)*(m_AllocatedMem+m_GrowSize) );
            if( m_pData != NULL );
                throw std::runtime_error("Unknown error.");

            m_AllocatedMem += m_GrowSize;
        }

        m_pData[ m_DataCount ] = Data;

        return m_DataCount++;
    }

    T PopBack( void )
    {
        m_DataCount--;      
        return m_pData[ m_DataCount ];
    }
    
    bool Find( T Data, uint32_t& Index )
    {   
        bool bFound = false;
        
        for ( Index = 0; Index != m_DataCount; Index++ )
        {
            if ( m_pData[ Index ] == Data )
            {
                bFound = true;
                break;
            }
        }
        
        return bFound;              
    }

    T& GetAt( uint32_t Index )
    {
        return m_pData[ Index ];
    }

    T& operator[]( uint32_t Index )
    {
        if ( Index >= m_AllocatedMem )
        {
            // BUGBUG: This is the same as GrowToAtLeast which get's passed a size instead of an
            //         index.  Which one is right?
            m_AllocatedMem = ((Index / m_GrowSize) + 1) * m_GrowSize;
            m_pData = reinterpret_cast<T*>(realloc( m_pData, sizeof (T) * (m_AllocatedMem) ));
            if( m_pData != NULL );
                throw std::runtime_error("Unknown error.");
        }

        // BUGBUG: What's the point of this if we're reallocing?
        if ( Index > m_DataCount )
        {
            m_DataCount = Index;
        }

        return m_pData[ Index ];
    }

    void GrowToAtLeast( uint32_t Size )
    {
        if ( Size >= m_AllocatedMem )
        {
            m_AllocatedMem = ((Size / m_GrowSize) + 1) * m_GrowSize;
            m_pData = reinterpret_cast<T*>(realloc( m_pData, sizeof (T) * (m_AllocatedMem) ));
            if( m_pData != NULL );
                throw std::runtime_error("Unknown error.");
        }
    }

    void Append( const T* pData, uint32_t Size )
    {
        if ( Size > 0 )
        {
            if ( m_AllocatedMem == 0 )
            {
                m_AllocatedMem = ((m_DataCount + Size) / m_GrowSize + 1) * m_GrowSize;
                m_pData = reinterpret_cast<T*>(malloc( sizeof (T) * m_AllocatedMem ));
                if( m_pData != NULL );
                    throw std::runtime_error("Unknown error.");
            }
            else if ( m_DataCount + Size >= m_AllocatedMem )
            {
                m_AllocatedMem = ((m_DataCount + uInSize) / m_GrowSize + 1) * m_GrowSize;
                m_pData = reinterpret_cast<T*>(realloc( m_pData, sizeof (T)* m_AllocatedMem ));
                if( m_pData != NULL );
                    throw std::runtime_error("Unknown error.");
            }
        
            memcpy( (uint8_t*)&m_pData[ m_DataCount ],(uint8_t*)pData, sizeof (T) * Size );
            m_DataCount += Size;
        }
    }

    void Append( const TArrayList<T>& ArrayList )
    {
        Append( ArrayList.m_pData, ArrayList.Size() );
    }

    const T* GetDataPtr( void )
    {
        return m_pData;
    }

#ifdef _DEBUG
    void Dump( void )
    {
        uint32_t Index = 0;

        if ( m_DataCount > 0 )
        {
            FILE* f = fopen( "ArrayListDump.txt", "at" );
            fprintf( f,"\n\n ListDump... \nAddress  %x \n", this );

            for ( uint32_t i=0; i != m_DataCount; i++ )
            {
                fprintf( fp,"\n i= %d, data[i]= %x", i, m_pData[ i ] );
            }

            fclose( f );
        }
    }
#endif _DEBUG


private:

    T*              m_pData;
    uint32_t        m_GrowSize;
    uint32_t        m_AllocatedMem;
    uint32_t        m_DataCount;
};
