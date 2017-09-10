#pragma once

#include "../types.h"
#include "../input.h"

typedef struct Teapot
{
    Transform transform;
    Transform velocity;
}
Teapot;

typedef struct World
{
    vec3 camera_position;
    vec3 camera_up;
    vec3 camera_look;

    quat parent_pot_tilt;

    Teapot *teapots;
    int teapot_count;
    int teapot_write_pos;

    int frame_counter;
    float time_factor;
}
World;

World *world_create();
void world_step(World *world, const InputState *input);
void world_destroy(World *world);