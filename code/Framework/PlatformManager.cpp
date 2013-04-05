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

#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Framework/PlatformManager.hpp"


std::once_flag                   
PlatformManager::only_one;
std::shared_ptr<PlatformManager> 
PlatformManager::instance_ = nullptr;

size_t
PlatformManager::Window::GetWindowHandle( void )
{
    std::lock_guard<std::mutex> lock(this->windowMutex);
    return this->window;
}

void 
PlatformManager::Window::SetWindowHandle(size_t window)
{
    std::lock_guard<std::mutex> lock(this->windowMutex);
    this->window = window;
}

uint32_t
PlatformManager::Window::GetWindowHeight( void )
{
    std::lock_guard<std::mutex> lock(this->windowMutex);
    return this->height;
}

void 
PlatformManager::Window::SetWindowHeight(uint32_t height)
{
    std::lock_guard<std::mutex> lock(this->windowMutex);
    this->height = height;
}

uint32_t
PlatformManager::Window::GetWindowWidth( void )
{
    std::lock_guard<std::mutex> lock(this->windowMutex);
    return this->width;
}

void 
PlatformManager::Window::SetWindowWidth(uint32_t width)
{
    std::lock_guard<std::mutex> lock(this->windowMutex);
    this->width = width;
}
