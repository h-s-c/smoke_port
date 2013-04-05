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
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/ProceduralTrees/System.hpp"

ManagerInterfaces   g_Managers;

void 
InitProceduralTreesSystem( ManagerInterfaces* pManagers)
{
    g_Managers = *pManagers;
}

ISystem* 
CreateProceduralTreesSystem()
{
    return new TreeSystem();
}

void 
DestroyProceduralTreesSystem( ISystem* pSystem)
{
    delete reinterpret_cast<TreeSystem*>( pSystem );
}

extern "C" 
{
    DLLEXPORT struct SystemFuncs SystemProceduralTrees = {
        &InitProceduralTreesSystem,
        &CreateProceduralTreesSystem,
        &DestroyProceduralTreesSystem
    };
}
