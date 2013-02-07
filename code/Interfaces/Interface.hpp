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
}
using namespace Interface;
