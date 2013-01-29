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
