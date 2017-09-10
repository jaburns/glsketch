#include "input.h"

#include <stdbool.h>
#include <math.h>
#define PI 3.14159f

static const float MOVE_SPEED = 0.05f;
static const float MOUSE_SENSITIVITY = 1.0f / 200.0f;

static bool key_w = false;
static bool key_s = false;
static bool key_a = false;
static bool key_d = false;
static bool key_space = false;
static bool key_lshift = false;

static float facing = 0.0f;
static float tilt = 0.0f;

static InputState state = {
    { 0.0f, 0.0f, -1.0f },
    { 0.0f, 0.0f, 0.0f },
    false
};

static void update_state()
{
    vec3_set(state.look_dir, 
        cosf(facing - 3.14159265f / 2.0f),
        0.0f,
        sinf(facing - 3.14159265f / 2.0f)
    );

    vec3_set(state.movement, 0.0f, 0.0f, 0.0f);

    vec3 move_front;
    vec3_scale(move_front, state.look_dir, MOVE_SPEED);

    vec3 move_side = { -state.look_dir[2], 0.0f, state.look_dir[0] };
    vec3_scale(move_side, move_side, MOVE_SPEED);

    if (key_w) vec3_add(state.movement, state.movement, move_front);
    if (key_s) vec3_sub(state.movement, state.movement, move_front);
    if (key_a) vec3_sub(state.movement, state.movement, move_side);
    if (key_d) vec3_add(state.movement, state.movement, move_side);

    state.look_dir[1] = tilt;
    if (key_space)  state.movement[1] += MOVE_SPEED;
    if (key_lshift) state.movement[1] -= MOVE_SPEED;
}

void input_handle_key_event(SDL_Keycode keycode, bool press)
{
    if (keycode == SDLK_w) key_w = press;
    if (keycode == SDLK_s) key_s = press;
    if (keycode == SDLK_a) key_a = press;
    if (keycode == SDLK_d) key_d = press;
    if (keycode == SDLK_SPACE) key_space = press;
    if (keycode == SDLK_LSHIFT) key_lshift = press;

    update_state();
}

void input_handle_mouse_click(bool button_down)
{
    state.clicking = button_down;

    update_state();
}

void input_handle_mouse_motion(SDL_MouseMotionEvent event)
{
    facing += (float)event.xrel * MOUSE_SENSITIVITY; 
    tilt -= (float)event.yrel * MOUSE_SENSITIVITY;
    if (tilt < -2.0f) tilt = -2.0f;
    if (tilt >  2.0f) tilt =  2.0f;

    update_state();
}

InputState input_read_state()
{
    return state;
}