#pragma once

#include "gfx.h"
#include "linmath.h"
#include <stdbool.h>

typedef struct InputState
{
    vec3 look_dir;
    vec3 movement;
    bool clicking;
}
InputState;

void input_bind_handlers(SDL_Window* window);
InputState input_read_state();