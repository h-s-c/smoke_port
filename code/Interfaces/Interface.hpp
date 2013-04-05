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
#include "Base/Compat.hpp"
#include "Base/Errors.hpp"
#include "Base/Intrinsics.hpp"
#include "Base/Math.hpp"
#include "Base/Platform.hpp"

#include <algorithm>
#include <cstring>
#include <list>
#include <mutex>
#include <stdarg.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <stddef.h> 
#include <atomic>


namespace Interface
{
    #include "Interfaces/SystemTypes.hpp"
    #include "Interfaces/Property.hpp"
    #include "Interfaces/ChangeControl.hpp"
    #include "Interfaces/TaskManager.hpp"
    #include "Interfaces/Environment.hpp"
    #include "Interfaces/Service.hpp"
    #include "Interfaces/Platform.hpp"
    #include "Interfaces/System.hpp"
    #include "Interfaces/Generic.hpp"
    #include "Interfaces/Geometry.hpp"
    #include "Interfaces/Graphics.hpp"
    #include "Interfaces/GUI.hpp"
    #include "Interfaces/Contact.hpp"
    #include "Interfaces/Behavior.hpp"
    #include "Interfaces/Area.hpp"
    #include "Interfaces/Move.hpp"
    #include "Interfaces/Target.hpp"
    #include "Interfaces/Input.hpp"
    #include "Interfaces/Tree.hpp"
}
using namespace Interface;
