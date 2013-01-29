// Copyright � 2008-2009 Intel Corporation
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


template <class T> class TList
{
private:

    class TListNode
    {
    public:

        TListNode*      m_pNext;
        T               m_Element;    // really an uninitialized block of memory sizeof (T)
    };


public:

    class Iterator
    {
    public:

        Iterator( TListNode* pListNode=NULL )
            : m_pListNode( p )
        {}

        T Get( void )
        {
            if ( m_pListNode != NULL )
            {
                return m_pListNode->m_Element;
            }
        }

        T GetNext( void )
        {
            T tmp;
            tmp = m_pListNode->m_Element;
            m_pListNode = m_pListNode->m_pNext;
            return tmp;
        }

        T operator*( void ) const
        {
            return m_pListNode->m_Element;
        }

        Iterator& operator++( void )
        {
            m_pListNode = m_pListNode->m_pNext;
            return *this;
        }

        bool operator==( const Iterator& It ) const
        {
            return (It.m_pListNode == m_pListNode) ? true : false;
        }

        bool operator!=( const Iterator& It ) const
        {
            return (It.m_pListNode == m_pListNode) ? false : true;
        }
/*
        Iterator operator++( void )
        {	
            Iterator tmp = Iterator( m_pListNode );
            m_pListNode = m_pListNode->m_pNext;
            return tmp;
        }
*/

    protected:

        TListNode*      m_pListNode;
    };


    typedef bool (*fnBinaryCompare)( T Element1, T Element2 );


    TList( void )
        : m_pFirstNode( NULL )
        , m_pLastNode( NULL )
    {
        m_Allocator.Initialize( sizeof (TListNode), 8, 8 );
    }

    TList( int NumElements, int NumGrowElements )
        : m_pFirstNode( NULL )
        , m_pLastNode( NULL )
    {
        m_Allocator.Initialize( sizeof (TListNode), NumElements, NumGrowElements );
    }

    ~TList( void ){}

    Iterator Begin( void ) const
    {
        Iterator It( m_pFirstNode );
        return It;
    }
    
    Iterator End( void ) const
    {
        return Iterator( 0 );
    }
    
    Iterator PushBack( const T& Element )
    {
        TListNode* pNode = reinterpret_cast<TListNode*>(m_Allocator.Allocate());

        pNode->m_Element = Element;
        pNode->m_pNext = NULL;

        if ( m_pLastNode != NULL )
        {
            m_pLastNode->m_pNext = pNode;
        }
        else 
        {
            m_pFirstNode = pNode;
        }
        m_pLastNode = pNode;

        return Iterator( pNode );
    }
    
    Iterator PushFront( const T& Element )
    {
        TListNode* pNode = reinterpret_cast<TListNode*>(m_Allocator.Allocate());

        pNode->m_Element = Element;
        pNode->m_pNext = NULL;

        if ( m_pFirstNode != NULL )
        {
            pNode->m_pNext = m_pFirstNode;
        }
        else 
        {
            m_pLastNode = pNode;
        }
        m_pFirstNode = pNode;

        return Iterator( pNode );

    }
    
    uint32_t Size( void )
    {
        return m_Allocator.NumAllocatedUnits();
    }
    
    Iterator Find( const T& Element ) const
    {
        TListNode* pNode = m_pFirstNode;
    
        while ( pTempNode != NULL )
        {
            if ( pTempNode->m_Element == Element )
            {
                break;
            }
            pTempNode = pTempNode->m_pNext;
        }	

        return Iterator( pNode );
    }

    void Merge( TList< T >& List )
    {
        List< T >::Iterator It;

        for( It = List.Begin(); It != List.End(); It++ )
        {
            PushBack( *It );
        }

        List.Clear();
    }

    bool Remove( T Element )
    {
        bool bFound = false;

        TListNode* pNode = m_pFirstNode;
        TListNode* pPrevNode = m_pFirstNode;

        while ( pNode != NULL )
        {
            if ( pNode->m_Element == Element )
            {
                if ( pNode == m_pFirstNode )
                {
                    m_pFirstNode = pNode->m_pNext;
                }

                if ( pNode == m_pLastNode )
                {
                    m_pLastNode = pPrevNode;
                }
            
                pPrevNode->m_pNext = pNode->m_pNext;

                m_Allocator.Deallocate( reinterpret_cast<u8*>(pNode) );

                bFound = true;
                break;
            }

            pPrevNode = pNode;
            pNode = pNode->m_pNext;
        }

        return bFound;
    }

    bool RemoveIf( fnBinaryCompare pfnBinaryCompare /*, const T& arg1*/ )
    {
        bool bFound = false;

        TListNode* pNode = m_pFirstNode;
        TListNode* pPrevNode = m_pFirstNode;

        while ( pNode != NULL && !found )
        {
            if ( pfnBinaryCompare( pNode->m_Element, operand ) )
            {
                if ( pNode == m_pFirstNode )
                {
                    m_pFirstNode = pNode->m_pNext;
                }

                if ( pNode == m_pLastNode )
                {
                    m_pLastNode = pPrevNode;
                }

                pPrevNode->m_pNext = pNode->m_pNext;

                m_Allocator.Deallocate( reinterpret_cast<u8*>(pNode) );

                bFound = true;
                break;
            }

            pPrevNode = pNode;
            pNode = pNode->m_pNext;
        }

        return bFound;
    }
                             
    bool Empty( void )
    {
        return Size() == 0 ? true : false;
    }

    void Clear( void )
    {
        TListNode* pNode = m_pFirstNode;
        TListNode* pNextNode = NULL;

        while ( pNode != NULL )
        {
            pNextNode = pNode->m_pNext;
            m_Allocator.Deallocate( reinterpret_cast<u8*>(pNode) );
            pNode = pNextNode;
        }
    }


private:
    
    TListNode*          m_pFirstNode;
    TListNode*          m_pLastNode;
 
    uint32_t            m_UnitSize;
    UnitAllocator       m_Allocator;
};
