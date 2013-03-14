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

////////////////////////////////////////////////////////////////////////
///   An interface for accessing platform specific functionality for 
///   things like the OS and processor.
////////////////////////////////////////////////////////////////////////

class IPlatform
{
public:
    class IWindow
    {
    public:
        // Get window handle.
        virtual size_t GetWindowHandle(void) = 0;
        // Set window handle.
        virtual void  SetWindowHandle(size_t window) = 0;
        // Get window height.
        virtual uint32_t GetWindowHeight(void) = 0;
        // Set window height.
        virtual void SetWindowHeight(uint32_t height) = 0;
        // Get window width.
        virtual uint32_t GetWindowWidth(void) = 0;
        // Set window width.
        virtual void SetWindowWidth(uint32_t width) = 0;
        
    };

    virtual IWindow& Window( void ) = 0;

};
