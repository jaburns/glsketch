#pragma once

#include "../types.h"
#include "../input.h"

typedef struct Teapot
{
    Transform transform;
    Transform velocity;
}
Teapot;

#define WORLD_MAX_TEAPOTS 600

typedef struct World
{
    vec3 camera_position;
    vec3 camera_up;
    vec3 camera_look;

    quat parent_pot_tilt;

    Teapot teapots[WORLD_MAX_TEAPOTS];
    int teapot_count;
    int teapot_write_pos;

    int frame_counter;
    float time_factor;
}
World;

void world_initialize(World *world);
void world_step(World *world, const InputState *input);
void world_copy(World *result, const World *other);
void world_lerp(World *result, const World *a, const World *b, float t);