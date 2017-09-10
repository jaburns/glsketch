#include "main.h"

#include <stdlib.h>

#include "../gfx.h"
#include "../resources.h"
#include "../types.h"
#include "../linmath.h"
#include "world.h"
#include "teapot_renderer.h"
#include "skybox_renderer.h"

static World *s_world;
static SkyboxRenderer *s_skybox_renderer;
static TeapotRenderer *s_teapot_renderer;

void scene_teapots_init()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glClearColor(0.16f, 0.17f, 0.18f, 1.0f);

    s_world = world_create();
    s_skybox_renderer = skybox_renderer_create();
    s_teapot_renderer = teapot_renderer_create(skybox_get_cubemap(s_skybox_renderer));
}

void scene_teapots_destroy()
{
    world_destroy(s_world);
    skybox_renderer_destroy(s_skybox_renderer);
    teapot_renderer_destroy(s_teapot_renderer);
}

void scene_teapots_step(const InputState *input, int width, int height)
{
    world_step(s_world, input);

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4x4 p;
    mat4x4_perspective(p, 3.14159f / 3.0f, width / (float)height, 0.1f, 1024.0f);

    vec3 zero3 = { 0.0f, 0.0f, 0.0f };

    mat4x4 v;
    mat4x4_look_at(v, zero3, s_world->camera_look, s_world->camera_up);
    mat4x4_translate_in_place(v, -s_world->camera_position[0], -s_world->camera_position[1], -s_world->camera_position[2]);

    teapot_renderer_use(s_teapot_renderer, s_world->camera_position, v, p);
    for (int i = 0; i < s_world->teapot_count; ++i) {
        teapot_renderer_draw(s_teapot_renderer, &s_world->teapots[i].transform);
    }

    skybox_renderer_draw_once(s_skybox_renderer, v, p);
}