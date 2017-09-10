#pragma once

#include "../common/linmath.h"
#include "../common/resources.h"

typedef struct SkyboxRenderer SkyboxRenderer;

SkyboxRenderer *skybox_renderer_create();
void skybox_renderer_draw_once(SkyboxRenderer *renderer, const mat4x4 *view, const mat4x4 *projection);
void skybox_renderer_destroy(SkyboxRenderer *renderer);

CubeMapRef skybox_get_cubemap(SkyboxRenderer *renderer);