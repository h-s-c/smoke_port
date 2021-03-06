/* The MIT License (MIT)
 * Copyright (c) 2013 Kevin Schmidt
 *  
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and 
 * associated documentation files (the "Software"), to deal in the Software without restriction, 
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, 
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
 * furnished to do so, subject to the following conditions:
 *  
 * The above copyright notice and this permission notice shall be included in all copies or 
 * substantial portions of the Software.
 *  
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT 
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/
 
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/InputOIS/System.hpp"
#include "Systems/InputOIS/Scene.hpp"
#include "Systems/InputOIS/Object.hpp"


pcstr InputSystem::sm_kapszPropertyNames[] =
{
    "MoveForward",
    "MoveBack",
    "MoveLeft",
    "MoveRight",
    "MoveUp",
    "MoveDown",
    "ResetView",
    "Pause",
    "Quit",
    "PolygonMode",
    "UseFireHose",
    "ShowNormals",
    "ShowTangents",
    "ShowBoundingBox",
    "ShowCaptions",
    "SetThreadCountTo1",
    "SetThreadCountTo2",
    "SetThreadCountTo4",
    "SetThreadCountToMax",
    "ToggleOverlay1",
    "ToggleOverlay2",
    "ToggleOverlay3",
    "ToggleOverlay4",
    "ToggleOverlay5"
};

const Properties::Property InputSystem::sm_kaDefaultProperties[] =
{
    Properties::Property( sm_kapszPropertyNames[ HotKey_MoveForward ],
    VALUE2( Properties::Values::String, Properties::Values::Int32 ),
    Properties::Flags::Valid,
    "Key", "Ctrl", NULL, NULL,
    "", 0),
    Properties::Property( sm_kapszPropertyNames[ HotKey_MoveBack ],
    VALUE2( Properties::Values::String, Properties::Values::Int32 ),
    Properties::Flags::Valid,
    "Key", "Ctrl", NULL, NULL,
    "", 0),
    Properties::Property( sm_kapszPropertyNames[ HotKey_MoveLeft ],
    VALUE2( Properties::Values::String, Properties::Values::Int32 ),
    Properties::Flags::Valid,
    "Key", "Ctrl", NULL, NULL,
    "", 0),
    Properties::Property( sm_kapszPropertyNames[ HotKey_MoveRight ],
    VALUE2( Properties::Values::String, Properties::Values::Int32 ),
    Properties::Flags::Valid,
    "Key", "Ctrl", NULL, NULL,
    "", 0),
    Properties::Property( sm_kapszPropertyNames[ HotKey_MoveUp ],
    VALUE2( Properties::Values::String, Properties::Values::Int32 ),
    Properties::Flags::Valid,
    "Key", "Ctrl", NULL, NULL,
    "", 0),
    Properties::Property( sm_kapszPropertyNames[ HotKey_MoveDown ],
    VALUE2( Properties::Values::String, Properties::Values::Int32 ),
    Properties::Flags::Valid,
    "Key", "Ctrl", NULL, NULL,
    "", 0),
    Properties::Property( sm_kapszPropertyNames[ HotKey_ResetView ],
    VALUE2( Properties::Values::String, Properties::Values::Int32 ),
    Properties::Flags::Valid,
    "Key", "Ctrl", NULL, NULL,
    "", 0),
    Properties::Property( sm_kapszPropertyNames[ HotKey_Pause ],
    VALUE2( Properties::Values::String, Properties::Values::Int32 ),
    Properties::Flags::Valid,
    "Key", "Ctrl", NULL, NULL,
    "", 0),
    Properties::Property( sm_kapszPropertyNames[ HotKey_Quit ],
    VALUE2( Properties::Values::String, Properties::Values::Int32 ),
    Properties::Flags::Valid,
    "Key", "Ctrl", NULL, NULL,
    "", 0),
    Properties::Property( sm_kapszPropertyNames[ HotKey_PolygonMode ],
    VALUE2( Properties::Values::String, Properties::Values::Int32 ),
    Properties::Flags::Valid,
    "Key", "Ctrl", NULL, NULL,
    "", 0),
    Properties::Property( sm_kapszPropertyNames[ HotKey_UseFireHose ],
    VALUE2( Properties::Values::String, Properties::Values::Int32 ),
    Properties::Flags::Valid,
    "Key", "Ctrl", NULL, NULL,
    "", 0),
    Properties::Property( sm_kapszPropertyNames[ HotKey_ShowNormals ],
    VALUE2( Properties::Values::String, Properties::Values::Int32 ),
    Properties::Flags::Valid,
    "Key", "Ctrl", NULL, NULL,
    "", 0),
    Properties::Property( sm_kapszPropertyNames[ HotKey_ShowTangents ],
    VALUE2( Properties::Values::String, Properties::Values::Int32 ),
    Properties::Flags::Valid,
    "Key", "Ctrl", NULL, NULL,
    "", 0),
    Properties::Property( sm_kapszPropertyNames[ HotKey_ShowBoundingBox ],
    VALUE2( Properties::Values::String, Properties::Values::Int32 ),
    Properties::Flags::Valid,
    "Key", "Ctrl", NULL, NULL,
    "", 0),
    Properties::Property( sm_kapszPropertyNames[ HotKey_ShowCaptions ],
    VALUE2( Properties::Values::String, Properties::Values::Int32 ),
    Properties::Flags::Valid,
    "Key", "Ctrl", NULL, NULL,
    "", 0),
    Properties::Property( sm_kapszPropertyNames[ HotKey_SetThreadCountTo1 ],
    VALUE2( Properties::Values::String, Properties::Values::Int32 ),
    Properties::Flags::Valid,
    "Key", "Ctrl", NULL, NULL,
    "", 0),
    Properties::Property( sm_kapszPropertyNames[ HotKey_SetThreadCountTo2 ],
    VALUE2( Properties::Values::String, Properties::Values::Int32 ),
    Properties::Flags::Valid,
    "Key", "Ctrl", NULL, NULL,
    "", 0),
    Properties::Property( sm_kapszPropertyNames[ HotKey_SetThreadCountTo4 ],
    VALUE2( Properties::Values::String, Properties::Values::Int32 ),
    Properties::Flags::Valid,
    "Key", "Ctrl", NULL, NULL,
    "", 0),
    Properties::Property( sm_kapszPropertyNames[ HotKey_SetThreadCountToMax ],
    VALUE2( Properties::Values::String, Properties::Values::Int32 ),
    Properties::Flags::Valid,
    "Key", "Ctrl", NULL, NULL,
    "", 0),
    Properties::Property( sm_kapszPropertyNames[ HotKey_ToggleOverlay1 ],
    VALUE2( Properties::Values::String, Properties::Values::Int32 ),
    Properties::Flags::Valid,
    "Key", "Ctrl", NULL, NULL,
    "", 0),
    Properties::Property( sm_kapszPropertyNames[ HotKey_ToggleOverlay2 ],
    VALUE2( Properties::Values::String, Properties::Values::Int32 ),
    Properties::Flags::Valid,
    "Key", "Ctrl", NULL, NULL,
    "", 0),
    Properties::Property( sm_kapszPropertyNames[ HotKey_ToggleOverlay3 ],
    VALUE2( Properties::Values::String, Properties::Values::Int32 ),
    Properties::Flags::Valid,
    "Key", "Ctrl", NULL, NULL,
    "", 0),
    Properties::Property( sm_kapszPropertyNames[ HotKey_ToggleOverlay4 ],
    VALUE2( Properties::Values::String, Properties::Values::Int32 ),
    Properties::Flags::Valid,
    "Key", "Ctrl", NULL, NULL,
    "", 0),
    Properties::Property( sm_kapszPropertyNames[ HotKey_ToggleOverlay5 ],
    VALUE2( Properties::Values::String, Properties::Values::Int32 ),
    Properties::Flags::Valid,
    "Key", "Ctrl", NULL, NULL,
    "", 0)
};


InputSystem::InputSystem(
    void
    )
    : ISystem()
{
}


InputSystem::~InputSystem(
    void
    )
{
}


pcstr
InputSystem::GetName(
    void
    )
{
    return System::Names::Input;
}


System::Type
InputSystem::GetSystemType(
    void
    )
{
    return System::Types::Input;
}


Error
InputSystem::Initialize(
    Properties::Array Properties
    )
{
    ASSERT( !m_bInitialized );

    m_bInitialized = True;

    SetProperties( Properties );

    return Errors::Success;
}


void
InputSystem::GetProperties(
    Properties::Array& Properties
    )
{
    //
    // Get the index of our first item.
    //
    i32 iProperty = static_cast<i32>(Properties.size());
    iProperty = 0;

    //
    // Add all the properties.
    //
    Properties.reserve( Properties.size() + HotKey_Count );

    for ( i32 i=0; i < HotKey_Count; i++ )
    {
        Properties.push_back( sm_kaDefaultProperties[ i ] );
    }
}


void
InputSystem::SetProperties(
    Properties::Array Properties
    )
{
    ASSERT( m_bInitialized );
    //
    // Read in the properties.
    //
    for ( Properties::Iterator it=Properties.begin(); it != Properties.end(); it++ )
    {
        if ( it->GetFlags() & Properties::Flags::Valid )
        {
            // property name
            pcstr sName = it->GetName();

            // HOT! key (may be a combo)
            HotKey* hotKey = new HotKey;
            hotKey->psKey = it->GetString( 0 );
            hotKey->bCtrl = it->GetBool( 1 );

            // add to HOT! keys map
            m_aHotKeys[sName] = hotKey;

            //
            // Set this property to invalid since it's already been read.
            //
            it->ClearFlag( Properties::Flags::Valid );
        }
    }
}

ISystemScene*
InputSystem::CreateScene(
    void
    )
{
    return new InputScene( this );
}


Error
InputSystem::DestroyScene(
    ISystemScene* pSystemScene
    )
{
    ASSERT( pSystemScene != NULL );

    InputScene* pScene = reinterpret_cast<InputScene*>(pSystemScene);
    SAFE_DELETE( pScene );

    return Errors::Success;
}

