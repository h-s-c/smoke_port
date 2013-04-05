// public domain

#pragma once
#include <functional>

extern "C"
{
    struct ManagerInterfaces
    {
        IEnvironment*       pEnvironment;
        IService*           pService;
        ITaskManager*       pTask;
        IPlatform*          pPlatform;
    };

    struct SystemFuncs 
    {
        std::function<void STDCALL (ManagerInterfaces* pManagers)> InitSystem;
        std::function<ISystem* STDCALL ()> CreateSystem;
        std::function<void STDCALL (ISystem* pSystem)> DestroySystem;
    };
}
