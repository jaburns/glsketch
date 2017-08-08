#pragma once

#include <deque>
#include <memory>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include "../glfw.hpp"
#include "../resources.hpp"
#include "../renderer/teapot.hpp"
#include "../renderer/skybox.hpp"
#include "../input.hpp"
#include "../types.hpp"

class TeapotFountain
{
public:
    struct Teapot
    {
        Transform transform;
        Transform velocity;
    };

    struct World
    {
        glm::vec3 camera_position;
        glm::vec3 camera_up;
        glm::vec3 camera_look;

        glm::quat parent_pot_tilt;
        std::deque<Teapot> teapots;

        int frame_counter;
        float time_factor;
    };

private:
    std::unique_ptr<SkyboxRenderer> m_skybox_renderer;
    std::unique_ptr<TeapotRenderer> m_teapot_renderer;

    World world;

    TeapotFountain(const TeapotFountain&) =delete;
    TeapotFountain& operator=(const TeapotFountain&) =delete;

public:
    TeapotFountain();

    void step(const InputState& input, int fb_width, int fb_height);
};