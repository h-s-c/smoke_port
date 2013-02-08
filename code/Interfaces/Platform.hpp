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
        virtual void* GetWindowHandle(void) = 0;
        // Set window handle.
        virtual void  SetWindowHandle(void* window) = 0;
        // Get window height.
        virtual uint32_t GetWindowHeight(void) = 0;
        // Set window height.
        virtual void SetWindowHeight(uint32_t height) = 0;
        // Get window width.
        virtual uint32_t GetWindowWidth(void) = 0;
        // Set window width.
        virtual void SetWindowWidth(uint32_t width) = 0;
        
    };

    virtual IWindow& WindowSystem( void ) = 0;

};
