// ======================================================================== //
// Copyright 2009-2012 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#include "Base/Platform.hpp"
#include "Base/Library.hpp"

#include <iostream>

////////////////////////////////////////////////////////////////////////////////
/// Windows Platform
////////////////////////////////////////////////////////////////////////////////

#if defined(PLATFORM_OS_WINDOWS) && !defined(USE_DLFCN_WIN32)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace Base
{
    /* opens a shared library */
    void* OpenLibrary( const std::string& libName, const std::string& libPath)
    {
        std::string fullName = std::string(libPath + "/" + libName + ".dll");
        HMODULE handle = LoadLibraryA(fullName.c_str());
        if (handle) 
        {
            return void*(handle);
        }
        else
        {
            //std::cerr << dlerror() << std::endl;
            return nullptr;
        }
    }

    /* returns address of a symbol from the library */
    void* GetSymbol(void* lib, const std::string& symName) 
    {
        void* sym = GetProcAddress(HMODULE(lib),symName.c_str());
        if (sym) 
        {
            return sym;
        }
        else
        {
            //std::cerr << dlerror() << std::endl;
            return nullptr;
        }
    }

    /* closes the shared library */
    void CloseLibrary(void* lib) 
    {
        FreeLibrary(HMODULE(lib));
    }
}
#endif

////////////////////////////////////////////////////////////////////////////////
/// Unix Platform
////////////////////////////////////////////////////////////////////////////////

#if defined(PLATFORM_OS_UNIX) || defined(USE_DLFCN_WIN32)

#include <dlfcn.h>

namespace Base
{
    /* opens a shared library */
    void* OpenLibrary( const std::string& libName, const std::string& libPath)
    {
        #if defined(PLATFORM_OS_LINUX) || defined(PLATFORM_OS_FREEBSD) 
        std::string fullName = std::string(libPath + "/" + libName + ".so");     
        #elif defined(PLATFORM_OS_MACOSX)
        std::string fullName = std::string(libPath + "/" + libName + ".dylib");
        #elif defined(PLATFORM_OS_WINDOWS)
        std::string fullName = std::string(libPath + "/" + libName + ".dll");
        #endif
        void* handle = dlopen(fullName.c_str(),RTLD_NOW);
        if (handle) 
        {
            return handle;
        }
        else
        {
            std::cerr << dlerror() << std::endl;
            return nullptr;
        }
    }

    /* returns address of a symbol from the library */
    void* GetSymbol(void* lib, const std::string& symName) 
    {
        void* sym = dlsym(lib,symName.c_str());
        if (sym) 
        {
            return sym;
        }
        else
        {
            std::cerr << dlerror() << std::endl;
            return nullptr;
        }
    }

    /* closes the shared library */
    void CloseLibrary(void* lib) 
    {
        dlclose(lib);
    }
}
#endif
