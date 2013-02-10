#pragma once

// Base
#include "Base/Platform.hpp"
#include "Base/Math.hpp"

struct AABB
{
    float           xMin;
    float           yMin;
    float           zMin;
    float           xMax;
    float           yMax;
    float           zMax;
    Math::Vector3   max;
    Math::Vector3   min;
    
};
