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

void input_handle_key_event(SDL_Keycode keycode, bool press);
void input_handle_mouse_motion(SDL_MouseMotionEvent event);
void input_handle_mouse_click(bool button_down);

InputState input_read_state();