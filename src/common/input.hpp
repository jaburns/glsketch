#pragma once

#include <glm/vec3.hpp>
#include "gfx.hpp"

struct InputState
{
    glm::vec3 look_dir = { 0.0f, 0.0f, -1.0f };
    glm::vec3 movement = { 0.0f, 0.0f, 0.0f };
    bool clicking = false;
};

namespace Input
{
    void bind_handlers(SDL_Window* window);
    InputState read_state();
}