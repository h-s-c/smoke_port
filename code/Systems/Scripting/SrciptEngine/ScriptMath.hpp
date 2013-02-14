// Copyright © 2008 Intel Corporation
// All Rights Reserved
// 
// The sample source code contained or described herein and all documents
// related to the sample source code ("Material") are owned by Intel Corporation
// or its suppliers or licensors. Title to the Material remains with Intel Corporation
// or its suppliers and licensors. The Material may contain trade secrets and
// proprietary and confidential information of Intel Corporation and its suppliers
// and licensors, and is protected by worldwide copyright and trade secret laws and
// treaty provisions. The sample source code is provided AS IS, with no warranties
// of any kind, express or implied. Except as expressly permitted by the Software
// license, neither Intel Corporation nor its suppliers assumes any responsibility
// or liability for any errors or inaccuracies that may appear herein.

#pragma once


namespace ScriptMath
{
    public ref class Vector3
    {
    public:

        Vector3( void )
        {
        }

        Vector3( float x, float y, float z )
        {
            Set( x, y, z );
        }

        void Set( float x, float y, float z )
        {
            m_x = x; m_y = y; m_z = z;
        }

        property float X
        {
            float get()
            {
                return m_x;
            }
            void set( float x )
            {
                m_x = x;
            }
        }

        property float Y
        {
            float get()
            {
                return m_y;
            }
            void set( float y )
            {
                m_y = y;
            }
        }

        property float Z
        {
            float get()
            {
                return m_z;
            }
            void set( float z )
            {
                m_z = z;
            }
        }


    protected:

        float       m_x;
        float       m_y;
        float       m_z;
    };


    public ref class Vector4 : public Vector3
    {
    public:

        Vector4( void )
        {
        }

        Vector4( float x, float y, float z, float w )
        {
            Set( x, y, z, w );
        }

        void Set( float x, float y, float z, float w )
        {
            m_x = x; m_y = y; m_z = z; m_w = w;
        }

        property float W
        {
            float get()
            {
                return m_w;
            }
            void set( float w )
            {
                m_w = w;
            }
        }


    protected:

        float       m_w;
    };
}
