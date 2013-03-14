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

namespace Errors
{
    #define MakeError( System, Error )                  ((System << 16) | (Error & 0xFFFF))

    namespace System
    {
        static const uint32_t None                           = 0x0000;
        static const uint32_t Memory                         = 0x0001;
        static const uint32_t File                           = 0x0002;
    }

    static const uint32_t Undefined                          = MakeError( System::None, 0xffff );
    static const uint32_t Success                            = MakeError( System::None, 0x0000 );
    static const uint32_t Failure                            = MakeError( System::None, 0x0001 );

    static const uint32_t NotImplemented                     = MakeError( System::None, 0x0010 );

    namespace Memory
    {
        static const uint32_t OutOfMemory                    = MakeError( System::Memory, 0x0001 );
        static const uint32_t InvalidAddress                 = MakeError( System::Memory, 0x0002 );
    }

    namespace File
    {
        static const uint32_t ErrorLoading                   = MakeError( System::File, 0x0001 );
        static const uint32_t InvalidFormat                  = MakeError( System::File, 0x0002 );
        static const uint32_t NotFound                       = MakeError( System::File, 0x0003 );
    }
}
