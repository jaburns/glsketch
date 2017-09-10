#include "main.h"

#include <stdlib.h>

#include "../common/gfx.h"
#include "../common/resources.h"
#include "../common/types.h"
#include "../common/linmath.h"
#include "teapot_renderer.h"
#include "skybox_renderer.h"

static const float TIME_SPEED_UP = 1.5f;
static const float TIME_SLOW_DOWN = 1.3f;
static const int MAX_POTS = 600;

typedef struct Teapot
{
    Transform transform;
    Transform velocity;
}
Teapot;

typedef struct World
{
    vec3 camera_position;
    vec3 camera_up;
    vec3 camera_look;

    quat parent_pot_tilt;

    Teapot *teapots;
    int teapot_count;

    int frame_counter;
    float time_factor;
}
World;

static World s_world;
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

    s_skybox_renderer = skybox_renderer_create();
    s_teapot_renderer = teapot_renderer_create(skybox_get_cubemap(s_skybox_renderer));

    s_world.camera_position[0] = 0.0f;  
    s_world.camera_position[1] = 0.0f;  
    s_world.camera_position[2] = 0.0f;  

    s_world.camera_up[0] = 0.0f;
    s_world.camera_up[1] = 1.0f;
    s_world.camera_up[2] = 0.0f;

    s_world.camera_look[0] =  0.0f;
    s_world.camera_look[1] =  0.0f;
    s_world.camera_look[2] = -1.0f;

    s_world.time_factor = 1.0f;
    s_world.frame_counter = 0;

    quat_identity(s_world.parent_pot_tilt);

    s_world.teapot_count = 1;
    s_world.teapots = (Teapot*)malloc(sizeof(Teapot));

    s_world.teapots[0].transform.position[0] =  0.0f;
    s_world.teapots[0].transform.position[1] =  0.0f;
    s_world.teapots[0].transform.position[2] = -3.0f;

    s_world.teapots[0].transform.scale[0] = 0.01f;
    s_world.teapots[0].transform.scale[1] = 0.01f;
    s_world.teapots[0].transform.scale[2] = 0.01f;

    quat_identity(s_world.teapots[0].transform.rotation);
}

void scene_teapots_destroy()
{
    skybox_renderer_destroy(s_skybox_renderer);
}

static void step_world(const InputState *input)
{
    World old_world = s_world;  // TODO careful, this is copying the teapot array pointer not the array

    s_world.frame_counter++;
//  s_world.camera_position[0] += 2.0f * input->movement;
//  s_world.camera_look = input.look_dir;

    if (old_world.frame_counter == 1 || old_world.frame_counter == 45 || old_world.frame_counter == 105) {
        vec3 axis;
        axis[0] = 0.707f;
        axis[1] = 0.707f;
        axis[2] = 0.0f;

        quat_identity(s_world.parent_pot_tilt);
        quat_rotate(s_world.parent_pot_tilt, 1.7f, axis);
    //  s_world.parent_pot_tilt = glm::angleAxis(glm::linearRand(0.0f, 3.14159f), glm::sphericalRand(1.0f));
    }

    if (old_world.frame_counter < 120) {
        float sin_frame = sin(old_world.frame_counter / 60.0f * 2.0f * 3.14159f);
        float scale = 0.01f + (0.002f + 0.006f * old_world.frame_counter / 120.0f) * sin_frame;

        s_world.teapots[0].transform.scale[0] = scale;
        s_world.teapots[0].transform.scale[1] = scale;
        s_world.teapots[0].transform.scale[2] = scale;

        float t = 0.5f * (0.5f + 0.5f*sin_frame);

        quat id;
        quat_identity(id);

        s_world.teapots[0].transform.rotation[0] = id[0] + (s_world.parent_pot_tilt[0] - id[0]) * t;
        s_world.teapots[0].transform.rotation[1] = id[1] + (s_world.parent_pot_tilt[1] - id[1]) * t;
        s_world.teapots[0].transform.rotation[2] = id[2] + (s_world.parent_pot_tilt[2] - id[2]) * t;
        s_world.teapots[0].transform.rotation[3] = id[3] + (s_world.parent_pot_tilt[3] - id[3]) * t;
    }
//  else if (old_world.frame_counter == 120) {
//      world.teapots.clear();
//  }

//  if (input.clicking) {
//      if (world.time_factor > 0.01f) {
//          world.time_factor /= TIME_SLOW_DOWN;
//      } else {
//          world.time_factor = 0.01f;
//      }
//  } else {
//      if (world.time_factor < 1.0f) {
//          world.time_factor *= TIME_SPEED_UP;
//      } else { 
//          world.time_factor = 1.0f;
//      }
//  }

//  if (world.time_factor > 0.9f) {
//      TeapotFountain::Teapot ntp;
//      ntp.transform.position.z = -3.0f;
//      ntp.transform.scale = glm::vec3(glm::linearRand(0.005f, 0.01f));
//      ntp.velocity.position = glm::sphericalRand(0.1f);
//      ntp.velocity.rotation = glm::rotate(glm::quat(), glm::linearRand(-0.2f, 0.2f), glm::sphericalRand(1.0f));
//      world.teapots.push_back(ntp);
//  }

//  if (world.teapots.size() > MAX_POTS) {
//      world.teapots.pop_front();
//  }

//  for (auto& tp : world.teapots) {
//      tp.velocity.position.y -= world.time_factor * 0.001f;
//      tp.transform.position += world.time_factor * tp.velocity.position;
//      tp.transform.rotation *= glm::mix(glm::quat(), tp.velocity.rotation, world.time_factor);
//  }
}

void scene_teapots_step(const InputState *input, int width, int height)
{
    step_world(input);

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4x4 p;
    mat4x4_perspective(p, 3.14159f / 3.0f, width / (float)height, 0.1f, 1024.0f);

    vec3 zero3 = { 0.0f, 0.0f, 0.0f };

    mat4x4 v;
    mat4x4_look_at(v, zero3, s_world.camera_look, s_world.camera_up);
    mat4x4_translate_in_place(v, -s_world.camera_position[0], -s_world.camera_position[1], -s_world.camera_position[2]);

    teapot_renderer_use(s_teapot_renderer, s_world.camera_position, v, p);
    for (int i = 0; i < s_world.teapot_count; ++i) {
        teapot_renderer_draw(s_teapot_renderer, &s_world.teapots[i].transform);
    }

    skybox_renderer_draw_once(s_skybox_renderer, v, p);
}