#include "world.h"

#include <stdlib.h>

static const float TIME_SPEED_UP = 1.5f;
static const float TIME_SLOW_DOWN = 1.3f;
static const int MAX_POTS = 600;

World *world_create()
{
    World *world = (World*)malloc(sizeof(World));

    vec3_set(world->camera_position, 0.0f, 0.0f,  0.0f);
    vec3_set(world->camera_up,       0.0f, 1.0f,  0.0f);
    vec3_set(world->camera_look,     0.0f, 0.0f, -1.0f);

    world->time_factor = 1.0f;
    world->frame_counter = 0;

    quat_identity(world->parent_pot_tilt);

    world->teapot_count = 1;
    world->teapot_write_pos = 0;
    world->teapots = (Teapot*)malloc(MAX_POTS * sizeof(Teapot));

    vec3_set(world->teapots[0].transform.position, 0.0f,  0.0f, -3.0f);
    vec3_set(world->teapots[0].transform.scale,    0.01f, 0.01f, 0.01f);
    quat_identity(world->teapots[0].transform.rotation);

    return world;
}

static float linear_rand(float min, float max)
{
    float rand01 = (float)rand() / (float)RAND_MAX;
    return min + rand01 * (max - min);
}

static void spherical_rand(vec3 result)
{
    result[0] = linear_rand(-1.0f, 1.0f);
    result[1] = linear_rand(-1.0f, 1.0f);
    result[2] = linear_rand(-1.0f, 1.0f);
    vec3_norm(result, result);
}

void world_step(World *world, const InputState *input)
{
    world->frame_counter++;

    vec3 scaled_movement;
    vec3_scale(scaled_movement, input->movement, 2.0f);
    vec3_add(world->camera_position, world->camera_position, scaled_movement);

    vec3_copy(world->camera_look, input->look_dir);

    if (world->frame_counter == 1 || world->frame_counter == 45 || world->frame_counter == 105) {
        vec3 axis;
        spherical_rand(axis);

        quat_identity(world->parent_pot_tilt);
        quat_rotate(world->parent_pot_tilt, linear_rand(0.0f, 3.14159f), axis);
    }

    if (world->frame_counter < 120) {
        float sin_frame = sinf(world->frame_counter / 60.0f * 2.0f * 3.14159f);
        float scale = 0.01f + (0.002f + 0.006f * world->frame_counter / 120.0f) * sin_frame;
        vec3_set(world->teapots[0].transform.scale, scale, scale, scale);

        quat id;
        quat_identity(id);
        quat_lerp(world->teapots[0].transform.rotation, id, world->parent_pot_tilt, 0.5f * (0.5f + 0.5f*sin_frame));

        return;
    }

    if (world ->frame_counter == 120) {
        world->teapot_count = 0;
        world->teapot_write_pos = 0;
    }

    if (input->clicking) {
        if (world->time_factor > 0.01f) {
            world->time_factor /= TIME_SLOW_DOWN;
        } else {
            world->time_factor = 0.01f;
        }
    } else {
        if (world->time_factor < 1.0f) {
            world->time_factor *= TIME_SPEED_UP;
        } else { 
            world->time_factor = 1.0f;
        }
    }

    if (world->time_factor > 0.9f) {
        int i = world->teapot_write_pos;

        float scale = linear_rand(0.005f, 0.01f);
        vec3_set(world->teapots[i].transform.position,  0.0f, 0.0f, -3.0f);
        vec3_set(world->teapots[i].transform.scale,   scale, scale, scale);
        quat_identity(world->teapots[i].transform.rotation);

        spherical_rand(world->teapots[i].velocity.position, 0.1f);
        vec3_scale(world->teapots[i].velocity.position, world->teapots[i].velocity.position, 0.1f);

        vec3 new_pot_axis;
        spherical_rand(new_pot_axis);
        quat_identity(world->teapots[i].velocity.rotation);
        quat_rotate(world->teapots[i].velocity.rotation, linear_rand(-0.2f, 0.2f), new_pot_axis);

        if (world->teapot_count < MAX_POTS) {
            world->teapot_count++;
            if (world->teapot_count == MAX_POTS) {
                world->teapot_write_pos = 0;
            } else {
                world->teapot_write_pos++;
            }
        } else {
            world->teapot_write_pos = (world->teapot_write_pos + 1) % world->teapot_count;
        }
    }

    for (int i = 0; i < world->teapot_count; ++i) {
        world->teapots[i].velocity.position[1] -= world->time_factor * 0.001f;

        vec3 scaled_vel;
        vec3_scale(scaled_vel, world->teapots[i].velocity.position, world->time_factor);
        vec3_add(world->teapots[i].transform.position, world->teapots[i].transform.position, scaled_vel);

        quat old_rotation, delta;
        quat_copy(old_rotation, world->teapots[i].transform.rotation);
        quat_identity(delta);
        quat_lerp(delta, delta, world->teapots[i].velocity.rotation, world->time_factor);
        quat_mul(world->teapots[i].transform.rotation, old_rotation, delta);
    }
}

void world_destroy(World *world)
{
    free(world);
}