#include "world.h"

#include <stdlib.h>
#include <webscope.h>

static const float TIME_SPEED_UP = 1.5f;
static const float TIME_SLOW_DOWN = 1.3f;

void world_initialize(World *world)
{
    vec3_set(world->camera_position, 0.0f, 0.0f,  0.0f);
    vec3_set(world->camera_up,       0.0f, 1.0f,  0.0f);
    vec3_set(world->camera_look,     0.0f, 0.0f, -1.0f);

    world->time_factor = 1.0f;
    world->frame_counter = 0;

    quat_identity(world->parent_pot_tilt);

    world->teapot_count = 1;
    world->teapot_write_pos = 0;

    vec3_set(world->teapots[0].transform.position, 0.0f,  0.0f, -3.0f);
    vec3_set(world->teapots[0].transform.scale,    0.01f, 0.01f, 0.01f);
    quat_identity(world->teapots[0].transform.rotation);
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

//  if (input->clicking) {
//      if (world->time_factor > 0.01f) {
//          world->time_factor /= TIME_SLOW_DOWN;
//      } else {
//          world->time_factor = 0.01f;
//      }
//  } else {
//      if (world->time_factor < 1.0f) {
//          world->time_factor *= TIME_SPEED_UP;
//      } else {
//          world->time_factor = 1.0f;
//      }
//  }

    world->time_factor = webscope_value("Time Factor", 1.0f, 0.001f, 10.0f);

    if (world->time_factor > 0.9f) {
        int i = world->teapot_write_pos;

        float scale = webscope_value("Scale", 0.008f, 0.0001f, 0.1f); // linear_rand(0.005f, 0.01f);
        vec3_set(world->teapots[i].transform.position,  0.0f, 0.0f, -3.0f);
        vec3_set(world->teapots[i].transform.scale,   scale, scale, scale);
        quat_identity(world->teapots[i].transform.rotation);

        spherical_rand(world->teapots[i].velocity.position);
        vec3_scale(world->teapots[i].velocity.position, world->teapots[i].velocity.position, 0.1f);

        vec3 new_pot_axis;
        spherical_rand(new_pot_axis);
        quat_identity(world->teapots[i].velocity.rotation);
        quat_rotate(world->teapots[i].velocity.rotation, linear_rand(-0.2f, 0.2f), new_pot_axis);

        if (world->teapot_count < WORLD_MAX_TEAPOTS) {
            world->teapot_count++;
            if (world->teapot_count == WORLD_MAX_TEAPOTS) {
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

void world_copy(World *result, const World *other)
{
    memcpy(result, other, sizeof(World));
}

static void transform_lerp(Transform *result, const Transform *a, const Transform *b, float t)
{
    vec3_lerp(result->position, a->position, b->position, t);
    vec3_lerp(result->scale, a->scale, b->scale, t);
    quat_lerp(result->rotation, a->rotation, b->rotation, t);
}

void world_lerp(World *result, const World *a, const World *b, float t)
{
    vec3_lerp(result->camera_position, a->camera_position, b->camera_position, t);
    vec3_lerp(result->camera_up, a->camera_up, b->camera_up, t);
    vec3_lerp(result->camera_look, a->camera_look, b->camera_look, t);

    quat_lerp(result->parent_pot_tilt, a->parent_pot_tilt, b->parent_pot_tilt, t);

    result->teapot_count = a->teapot_count;
    result->teapot_write_pos = b->teapot_write_pos;

    for (int i = 0; i < a->teapot_count; ++i) {
        transform_lerp(&result->teapots[i].transform, &a->teapots[i].transform, &b->teapots[i].transform, t);
        transform_lerp(&result->teapots[i].velocity, &a->teapots[i].velocity, &b->teapots[i].velocity, t);
    }

    result->frame_counter = b->frame_counter;
    result->time_factor = float_lerp(a->time_factor, b->time_factor, t);
}
