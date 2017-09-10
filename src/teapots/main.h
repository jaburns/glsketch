#pragma once

#include "../input.h"

void scene_teapots_init();
void scene_teapots_step(const struct InputState *input, int width, int height);
void scene_teapots_destroy();