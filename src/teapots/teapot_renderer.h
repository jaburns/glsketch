#pragma once

#include "../linmath.h"
#include "../types.h"
#include "../resources.h"

typedef struct TeapotRenderer TeapotRenderer;

TeapotRenderer *teapot_renderer_create(CubeMapRef skybox_cubemap);
void teapot_renderer_use(TeapotRenderer *renderer, const vec3 *camera_pos, const mat4x4 *view, const mat4x4 *projection);
void teapot_renderer_draw(TeapotRenderer *renderer, const Transform *transform);
void teapot_renderer_destroy(TeapotRenderer *renderer);