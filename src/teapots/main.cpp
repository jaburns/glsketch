#include "main.hpp"

#include <vector>
#include <utility>
#include <stdlib.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>
#include <math.h>

static const float TIME_SPEED_UP = 1.5f;
static const float TIME_SLOW_DOWN = 1.3f;
static const int MAX_POTS = 600;

static TeapotFountain::World step_world(const TeapotFountain::World& old_world, const InputState& input)
{
    TeapotFountain::World world = old_world;

    world.frame_counter++;
    world.camera_position += 2.0f * input.movement;
    world.camera_look = input.look_dir;

    if (old_world.frame_counter == 1 || old_world.frame_counter == 45 || old_world.frame_counter == 105) {
        world.parent_pot_tilt = glm::angleAxis(glm::linearRand(0.0f, 3.14159f), glm::sphericalRand(1.0f));
    }

    if (old_world.frame_counter < 120) {
        float sin_frame = sin(old_world.frame_counter / 60.0f * 2.0f * 3.14159f);
        float scale = 0.01f + (0.002f + 0.006f * old_world.frame_counter / 120.0f) * sin_frame;
        world.teapots.front().transform.scale = glm::vec3(scale);
        world.teapots.front().transform.rotation = glm::mix(glm::quat(), world.parent_pot_tilt, 0.5f * (0.5f + 0.5f*sin_frame));
        return world;
    }
    else if (old_world.frame_counter == 120) {
        world.teapots.clear();
    }

    if (input.clicking) {
        if (world.time_factor > 0.01f) {
            world.time_factor /= TIME_SLOW_DOWN;
        } else {
            world.time_factor = 0.01f;
        }
    } else {
        if (world.time_factor < 1.0f) {
            world.time_factor *= TIME_SPEED_UP;
        } else { 
            world.time_factor = 1.0f;
        }
    }

    if (world.time_factor > 0.9f) {
        TeapotFountain::Teapot ntp;
        ntp.transform.position.z = -3.0f;
        ntp.transform.scale = glm::vec3(glm::linearRand(0.005f, 0.01f));
        ntp.velocity.position = glm::sphericalRand(0.1f);
        ntp.velocity.rotation = glm::rotate(glm::quat(), glm::linearRand(-0.2f, 0.2f), glm::sphericalRand(1.0f));
        world.teapots.push_back(ntp);
    }

    if (world.teapots.size() > MAX_POTS) {
        world.teapots.pop_front();
    }

    for (auto& tp : world.teapots) {
        tp.velocity.position.y -= world.time_factor * 0.001f;
        tp.transform.position += world.time_factor * tp.velocity.position;
        tp.transform.rotation *= glm::mix(glm::quat(), tp.velocity.rotation, world.time_factor);
    }

    return world;
}

TeapotFountain::TeapotFountain()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glClearColor(0.16f, 0.17f, 0.18f, 1.0f);

    m_skybox_renderer = std::make_unique<SkyboxRenderer>();
    m_teapot_renderer = std::make_unique<TeapotRenderer>(m_skybox_renderer->get_skybox_cubemap());

    world.camera_position = { 0.0f, 0.0f, 0.0f };
    world.camera_up = { 0.0f, 1.0f, 0.0f };
    world.camera_look = { 0.0f, 0.0f, -1.0f };
    world.time_factor = 1.0f;
    world.frame_counter = 0;

    Teapot ntp;
    ntp.transform.position.z = -3.0f;
    ntp.transform.scale = glm::vec3(0.01f);
    world.teapots.push_back(ntp);
}

void TeapotFountain::step(const InputState& input, int fb_width, int fb_height)
{
    world = step_world(world, input);

    glViewport(0, 0, fb_width, fb_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto p = glm::perspective(3.14159f / 3.0f, fb_width / (float)fb_height, 0.1f, 1024.0f);
    auto v = glm::translate(
        glm::lookAt(glm::vec3(0.0f), world.camera_look, world.camera_up),
        -world.camera_position
    );

    m_teapot_renderer->use(world.camera_position, v, p);
    for (auto tp : world.teapots) {
        m_teapot_renderer->draw(tp.transform);
    }

    m_skybox_renderer->draw_once(v, p);
}