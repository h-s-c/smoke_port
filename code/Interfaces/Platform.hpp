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
        
    };

    virtual IWindow& WindowSystem( void ) = 0;

};
