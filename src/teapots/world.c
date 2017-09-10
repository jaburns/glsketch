#include "world.h"

static const float TIME_SPEED_UP = 1.5f;
static const float TIME_SLOW_DOWN = 1.3f;
static const int MAX_POTS = 600;

World *world_create()
{
    World *world = (World*)malloc(sizeof(World));

    world->camera_position[0] = 0.0f;  
    world->camera_position[1] = 0.0f;  
    world->camera_position[2] = 0.0f;  

    world->camera_up[0] = 0.0f;
    world->camera_up[1] = 1.0f;
    world->camera_up[2] = 0.0f;

    world->camera_look[0] =  0.0f;
    world->camera_look[1] =  0.0f;
    world->camera_look[2] = -1.0f;

    world->time_factor = 1.0f;
    world->frame_counter = 0;

    quat_identity(world->parent_pot_tilt);

    world->teapot_count = 1;
    world->teapots = (Teapot*)malloc(sizeof(Teapot));

    world->teapots[0].transform.position[0] =  0.0f;
    world->teapots[0].transform.position[1] =  0.0f;
    world->teapots[0].transform.position[2] = -3.0f;

    world->teapots[0].transform.scale[0] = 0.01f;
    world->teapots[0].transform.scale[1] = 0.01f;
    world->teapots[0].transform.scale[2] = 0.01f;

    quat_identity(world->teapots[0].transform.rotation);

    return world;
}

void world_step(World *world, const InputState *input)
{
    world->frame_counter++;
//  world->camera_position[0] += 2.0f * input->movement;
//  world->camera_look = input.look_dir;

    if (world->frame_counter == 1 || world->frame_counter == 45 || world->frame_counter == 105) {
        vec3 axis;
        axis[0] = 0.707f;
        axis[1] = 0.707f;
        axis[2] = 0.0f;

        quat_identity(world->parent_pot_tilt);
        quat_rotate(world->parent_pot_tilt, 1.7f, axis);
    //  s_world.parent_pot_tilt = glm::angleAxis(glm::linearRand(0.0f, 3.14159f), glm::sphericalRand(1.0f));
    }

    if (world->frame_counter < 120) {
        float sin_frame = sinf(world->frame_counter / 60.0f * 2.0f * 3.14159f);
        float scale = 0.01f + (0.002f + 0.006f * world->frame_counter / 120.0f) * sin_frame;

        world->teapots[0].transform.scale[0] = scale;
        world->teapots[0].transform.scale[1] = scale;
        world->teapots[0].transform.scale[2] = scale;

        float t = 0.5f * (0.5f + 0.5f*sin_frame);

        quat id;
        quat_identity(id);

        world->teapots[0].transform.rotation[0] = id[0] + (world->parent_pot_tilt[0] - id[0]) * t;
        world->teapots[0].transform.rotation[1] = id[1] + (world->parent_pot_tilt[1] - id[1]) * t;
        world->teapots[0].transform.rotation[2] = id[2] + (world->parent_pot_tilt[2] - id[2]) * t;
        world->teapots[0].transform.rotation[3] = id[3] + (world->parent_pot_tilt[3] - id[3]) * t;
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

void world_destroy(World *world)
{
    free(world);
}