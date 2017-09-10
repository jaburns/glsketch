#pragma once

#include "linmath.h"

typedef struct Transform
{
    vec3 position;
    quat rotation;
    vec3 scale;
}
Transform;