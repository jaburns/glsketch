#include "main.h"

#include <stdlib.h>

#include "../gfx.h"
#include "../resources.h"
#include "../linmath.h"
#include "../timer.h"
#include "world.h"
#include "teapot_renderer.h"
#include "skybox_renderer.h"

static SkyboxRenderer *s_skybox_renderer;
static TeapotRenderer *s_teapot_renderer;

static World s_world_now;
static World s_world_prev;

static uint64_t s_current_time;
static uint64_t s_accumulator;

static const uint64_t NANOS_PER_TICK = 16666667ull;  // 60 tps

void scene_teapots_init()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glClearColor(0.16f, 0.17f, 0.18f, 1.0f);

    s_skybox_renderer = skybox_renderer_create();
    s_teapot_renderer = teapot_renderer_create(skybox_get_cubemap(s_skybox_renderer));

    world_initialize(&s_world_now);
    world_copy(&s_world_prev, &s_world_now);

    s_current_time = ns();
    s_accumulator = 0;
}

void scene_teapots_destroy()
{
    skybox_renderer_destroy(s_skybox_renderer);
    teapot_renderer_destroy(s_teapot_renderer);
}

static void render(const World *world, int width, int height)
{
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4x4 p;
    mat4x4_perspective(p, 3.14159f / 3.0f, width / (float)height, 0.1f, 1024.0f);

    vec3 zero3 = { 0.0f, 0.0f, 0.0f };

    mat4x4 v;
    mat4x4_look_at(v, zero3, world->camera_look, world->camera_up);
    mat4x4_translate_in_place(v, -world->camera_position[0], -world->camera_position[1], -world->camera_position[2]);

    teapot_renderer_use(s_teapot_renderer, world->camera_position, v, p);
    for (int i = 0; i < world->teapot_count; ++i) {
        teapot_renderer_draw(s_teapot_renderer, &world->teapots[i].transform);
    }

    skybox_renderer_draw_once(s_skybox_renderer, v, p);
}

void scene_teapots_step(const InputState *input, int width, int height)
{
    uint64_t new_time = ns();
    uint64_t diff = new_time - s_current_time;
    s_current_time = new_time;
    s_accumulator += diff;

    while (s_accumulator >= NANOS_PER_TICK) {
        world_copy(&s_world_prev, &s_world_now);
        world_step(&s_world_now, input);
        s_accumulator -= NANOS_PER_TICK;
    }

    World lerped;
    world_lerp(&lerped, &s_world_prev, &s_world_now, (float)s_accumulator / (float)NANOS_PER_TICK);

    render(&lerped, width, height);
}