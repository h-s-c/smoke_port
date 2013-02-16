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


/*******************************************************************************
* STRUCT: Property
*
* DESCRIPTION:
* Defines a structure used to get and set properties for a system and object.
*******************************************************************************/

namespace Properties
{
    namespace Values
    {
        #define VALUE1( v1 )                         ((v1) << 0)
        #define VALUE2( v1, v2 )                     (VALUE1(v1) | ((v2) << 8))
        #define VALUE3( v1, v2, v3 )                 (VALUE2(v1, v2) | ((v3) << 16))
        #define VALUE4( v1, v2, v3, v4 )             (VALUE3(v1, v2, v3) | ((v4) << 24))

        #define VALUE1x2( v1 )                       VALUE2( v1, v1 )
        #define VALUE1x3( v1 )                       VALUE3( v1, v1, v1 )
        #define VALUE1x4( v1 )                       VALUE4( v1, v1, v1, v1 )

        static const u32 Count                      = 4;
        static const u32 Mask                       = 0x000000FF;

        static const u32 None                       = 0x00000000;
        static const u32 Boolean                    = 0x00000001;
        static const u32 Enum                       = 0x00000002;
        static const u32 Int32                      = 0x00000003;
        static const u32 Float32                    = 0x00000004;
        static const u32 String                     = 0x00000005;
        static const u32 Path                       = 0x00000006;
        static const u32 Angle                      = 0x00000007;

        static const u32 Vector3                    = 0x00101010;
        static const u32 Vector4                    = 0x11111111;

        static const u32 Quaternion                 = 0x12121212;

        static const u32 Color3                     = 0x00131313;
        static const u32 Color4                     = 0x14141414;
    }

    namespace Flags
    {
        static const u32 Valid                      = 0x00000001;
        static const u32 InitOnly                   = 0x00000002;
        static const u32 Multiple                   = 0x00000004; // this is write only by default
        static const u32 WriteOnly                  = 0x00000008;
    }

    /// <summary>
    ///   Class for providing a method to transfer paramters between a system and the framework.
    /// </summary>
    class Property
    {
    public:

        /// <summary>
        ///   Empty constructor.
        /// </summary>
        Property( void ) {}

        /// <summary>
        ///   Constructor for creating a well formed property.  Useful for creating an array of
        ///    statically defined properties.
        /// </summary>
        /// <param name="pszPropertyName">The name of this property.</param>
        /// <param name="PropertyType">The type of property from Proerties::Values.</param>
        /// <param name="PropertyFlags">Flags for qualifying the use of the property.</param>
        //Property( pcstr pszPropertyName, u32 PropertyType, u32 PropertyFlags, ... );

        /// <summary>
        ///   Constructor for creating a well formed property.  Useful for creating an array of
        ///    statically defined properties.  Gives the extra added bonus of naming the values
        ///    within an xDF instead of just Value1, Value2, etc.
        /// </summary>
        /// <param name="pszPropertyName">The name of this property.</param>
        /// <param name="PropertyType">The type of property from Proerties::Values.</param>
        /// <param name="PropertyFlags">Flags for qualifying the use of the property.</param>
        /// <param name="pszValue1Name">Name of value 1.</param>
        /// <param name="pszValue2Name">Name of value 2.</param>
        /// <param name="pszValue3Name">Name of value 3.</param>
        /// <param name="pszValue4Name">Name of value 4.</param>
        Property(
            pcstr pszName,
            u32 Type,
            u32 Flags,
            pcstr pszValue1Name,
            pcstr pszValue2Name,
            pcstr pszValue3Name,
            pcstr pszValue4Name,
            ...
            )
            : m_pszName( pszName )
            , m_Type( Type )
            , m_Flags( Flags )
            , m_apszEnumOptions( NULL )
        {
            //
            // Set to default if nothing is passed in.
            //
            pcstr sm_kpszValue1Name = "Value1";
            pcstr sm_kpszValue2Name = "Value2";
            pcstr sm_kpszValue3Name = "Value3";
            pcstr sm_kpszValue4Name = "Value4";
            m_apszValueNames[ 0 ] = (pszValue1Name != NULL) ? pszValue1Name : sm_kpszValue1Name;
            m_apszValueNames[ 1 ] = (pszValue2Name != NULL) ? pszValue2Name : sm_kpszValue2Name;
            m_apszValueNames[ 2 ] = (pszValue3Name != NULL) ? pszValue3Name : sm_kpszValue3Name;
            m_apszValueNames[ 3 ] = (pszValue4Name != NULL) ? pszValue4Name : sm_kpszValue4Name;
    /*
            //
            // Do some extra processing for the known types.
            //
            if ( Type == Values::Vector3 )
            {
                if ( pszValue1Name == NULL && pszValue2Name == NULL && pszValue3Name == NULL )
                {
                    m_apszValueNames[ 0 ] = "X";
                    m_apszValueNames[ 1 ] = "Y";
                    m_apszValueNames[ 2 ] = "Z";
                }
            }
            else if ( Type == Values::Vector4 || Type == Values::Quaternion )
            {
                if ( pszValue1Name == NULL && pszValue2Name == NULL &&
                     pszValue3Name == NULL && pszValue4Name == NULL )
                {
                    m_apszValueNames[ 0 ] = "X";
                    m_apszValueNames[ 1 ] = "Y";
                    m_apszValueNames[ 2 ] = "Z";
                    m_apszValueNames[ 3 ] = "W";
                }
            }
            else if ( Type == Values::Color3 )
            {
                if ( pszValue1Name == NULL && pszValue2Name == NULL && pszValue3Name == NULL )
                {
                    m_apszValueNames[ 0 ] = "B";
                    m_apszValueNames[ 1 ] = "G";
                    m_apszValueNames[ 2 ] = "R";
                }
            }
            else if ( Type == Values::Color4 )
            {
                if ( pszValue1Name == NULL && pszValue2Name == NULL &&
                     pszValue3Name == NULL && pszValue4Name == NULL )
                {
                    m_apszValueNames[ 0 ] = "B";
                    m_apszValueNames[ 1 ] = "G";
                    m_apszValueNames[ 2 ] = "R";
                    m_apszValueNames[ 3 ] = "A";
                }
            }
    */
            //
            // Read in the ... passed in values.
            //
            va_list pArg;
            va_start( pArg, pszValue4Name );

            for ( u32 i=0; i < Values::Count; i++ )
            {
                switch ( Type & Values::Mask )
                {
                case Values::None:
                    i = Values::Count;
                    break;

                case Values::Boolean:
                    m_aValues[ i ].Boolean = va_arg( pArg, u32 );
                    break;

                case Values::Enum:
                case Values::Int32:
                    m_aValues[ i ].Int32 = va_arg( pArg, i32 );
                    break;
                /*When a function with a variable-length argument list is called, 
                 * the variable arguments are passed using C's old ``default argument promotions.'' 
                 * These say that types char and short int are automatically promoted to int, 
                 * and type float is automatically promoted to double. 
                 * Therefore, varargs functions will never receive arguments of type char, short int, or float. 
                 * Furthermore, it's an error to ``pass'' the type names char, short int, or float as the 
                 * second argument to the va_arg() macro. Finally, for vaguely related reasons, 
                 * the last fixed argument (the one whose name is passed as the second argument to the va_start() macro) 
                 * should not be of type char, short int, or float, either.*/
                 
#ifdef COMPILER_MSVC
#pragma warning( disable : 4244 )
#endif
                case Values::Float32:
                case Values::Angle:
                    m_aValues[ i ].Float32 = va_arg( pArg, double );
                    break;

                case Values::Vector3 & Values::Mask:
                case Values::Color3 & Values::Mask:
                {
                    ASSERTMSG( i == 0, "Vector3 or Color3 can be the only value on a property." );
                    m_aValues[ 0 ].Float32 = va_arg( pArg, double );
                    m_aValues[ 1 ].Float32 = va_arg( pArg, double );
                    m_aValues[ 2 ].Float32 = va_arg( pArg, double );
                    i = Values::Count;
                    break;
                }

                case Values::Vector4 & Values::Mask:
                case Values::Quaternion & Values::Mask:
                case Values::Color4 & Values::Mask:
                {
                    ASSERTMSG( i == 0, "Vector4, Quaternion, or Color4 can be the only value on a property." );
                    m_aValues[ 0 ].Float32 = va_arg( pArg, double );
                    m_aValues[ 1 ].Float32 = va_arg( pArg, double );
                    m_aValues[ 2 ].Float32 = va_arg( pArg, double );
                    m_aValues[ 3 ].Float32 = va_arg( pArg, double );
                    i = Values::Count;
                    break;
                }
#ifdef COMPILER_MSVC
#pragma warning( default : 4244 )
#endif
                case Values::String:
                case Values::Path:
                    m_asValues[ i ] = va_arg( pArg, pcstr );
                    break;

                default:
                    ASSERTMSG( False, "Unhandled property type." );
                    break;
                }

                Type >>= 8;
            }

            va_end( pArg );
        }

        /// <summary>
        ///   Returns the property name.
        /// </summary>
        /// <returns>The property's name.</returns>
        pcstr GetName( void ) const
        {
            return m_pszName;
        }

        u32 GetValueType( i32 Index ) const
        {
            ASSERT( Index >= 0 && Index < Values::Count );
            return (m_Type >> (Index * 8)) & Values::Mask;
        }

        u32 GetFlags( void ) const
        {
            return m_Flags;
        }

        void SetFlag( u32 Flag )
        {
            m_Flags |= Flag;
        }

        void ClearFlag( u32 Flag )
        {
            m_Flags &= ~Flag;
        }

        Bool GetBool( i32 Index ) const
        {
            ASSERT( Index >= 0 && Index < Values::Count );
            return m_aValues[ Index ].Boolean;
        }

        i32 GetInt32( i32 Index ) const
        {
            ASSERT( Index >= 0 && Index < Values::Count );
            return m_aValues[ Index ].Int32;
        }

        f32 GetFloat32( i32 Index ) const
        {
            ASSERT( Index >= 0 && Index < Values::Count );
            return m_aValues[ Index ].Float32;
        }

        const std::string GetString( i32 Index ) const
        {
            ASSERT( Index >= 0 && Index < Values::Count );
            return m_asValues[ Index ];
        }

        pcstr GetStringPtr( i32 Index ) const
        {
            ASSERT( Index >= 0 && Index < Values::Count );
            return m_asValues[ Index ].c_str();
        }

        const Base::Vector3 GetVector3( void ) const
        {
            return Base::Vector3( m_aValues[ 0 ].Float32,
                                  m_aValues[ 1 ].Float32,
                                  m_aValues[ 2 ].Float32);
        }

        const Base::Vector4 GetVector4( void ) const
        {
            Base::Vector4 v = { m_aValues[ 0 ].Float32, m_aValues[ 1 ].Float32,
                                m_aValues[ 2 ].Float32, m_aValues[ 3 ].Float32 };
            return v;
        }

        const Base::Quaternion GetQuaternion( void ) const
        {
            Base::Quaternion q ={ m_aValues[ 0 ].Float32, m_aValues[ 1 ].Float32,
                                  m_aValues[ 2 ].Float32, m_aValues[ 3 ].Float32 };
            return q;
        }

        const Base::Color3 GetColor3( void ) const
        {
            Base::Color3 c = { m_aValues[ 0 ].Float32, m_aValues[ 1 ].Float32,
                               m_aValues[ 2 ].Float32 };
            return c;
        }

        const Base::Color4 GetColor4( void ) const
        {
            Base::Color4 c = { m_aValues[ 0 ].Float32, m_aValues[ 1 ].Float32,
                               m_aValues[ 2 ].Float32, m_aValues[ 3 ].Float32 };
            return c;
        }

        void SetValue( i32 Index, const Bool& Value )
        {
            ASSERT( Index >= 0 && Index < Values::Count );
            m_aValues[ Index ].Boolean = Value;
        }

        void SetValue( i32 Index, const i32& Value )
        {
            ASSERT( Index >= 0 && Index < Values::Count );
            m_aValues[ Index ].Int32 = Value;
        }

        void SetValue( i32 Index, const f32& Value )
        {
            ASSERT( Index >= 0 && Index < Values::Count );
            m_aValues[ Index ].Float32 = Value;
        }

        void SetValue( i32 Index, const std::string& Value )
        {
            ASSERT( Index >= 0 && Index < Values::Count );
            m_asValues[ Index ] = Value;
        }

        void SetValue( const Base::Vector3& Value )
        {
            m_aValues[ 0 ].Float32 = Value.x;
            m_aValues[ 1 ].Float32 = Value.y;
            m_aValues[ 2 ].Float32 = Value.z;
        }

        void SetValue( const Base::Vector4& Value )
        {
            m_aValues[ 0 ].Float32 = Value.x;
            m_aValues[ 1 ].Float32 = Value.y;
            m_aValues[ 2 ].Float32 = Value.z;
            m_aValues[ 3 ].Float32 = Value.w;
        }

        void SetValue( const Base::Quaternion& Value )
        {
            m_aValues[ 0 ].Float32 = Value.x;
            m_aValues[ 1 ].Float32 = Value.y;
            m_aValues[ 2 ].Float32 = Value.z;
            m_aValues[ 3 ].Float32 = Value.w;
        }

        void SetValue( const Base::Color3& Value )
        {
            m_aValues[ 0 ].Float32 = Value.b;
            m_aValues[ 1 ].Float32 = Value.g;
            m_aValues[ 2 ].Float32 = Value.r;
        }

        void SetValue( const Base::Color4& Value )
        {
            m_aValues[ 0 ].Float32 = Value.b;
            m_aValues[ 1 ].Float32 = Value.g;
            m_aValues[ 2 ].Float32 = Value.r;
            m_aValues[ 3 ].Float32 = Value.a;
        }

        pcstr GetEnumOption( i32 Index ) const
        {
            return m_apszEnumOptions[ Index ];
        }

        void SetEnumOptions( const pcstr* apszEnumOptions )
        {
            m_apszEnumOptions = apszEnumOptions;
        }

        pcstr GetValueName( i32 Index ) const
        {
            ASSERT( Index >= 0 && Index < Values::Count );
            return m_apszValueNames[ Index ];
        }


    protected:

        static pcstr            sm_kpszValue1Name;
        static pcstr            sm_kpszValue2Name;
        static pcstr            sm_kpszValue3Name;
        static pcstr            sm_kpszValue4Name;

        pcstr                   m_pszName;

        u32                     m_Type;
        u32                     m_Flags;

        union Value
        {
            u32                 Boolean;
            i32                 Int32;
            f32                 Float32;
        };
        Value                   m_aValues[ Values::Count ];
        std::string             m_asValues[ Values::Count ];

        const pcstr*            m_apszEnumOptions;

        pcstr                   m_apszValueNames[ Values::Count ];
    };

    typedef std::vector<Properties::Property>       Array;
    typedef Array::iterator                         Iterator;
    typedef Array::const_iterator                   ConstIterator;
}
