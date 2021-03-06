#include <stdio.h>
#include <stdlib.h>
#include <webscope.h>
#include "gfx.h"
#include "input.h"
#include "teapots/main.h"

static SDL_Window* s_window;
static SDL_GLContext s_context;
static int s_window_width, s_window_height;
static bool s_should_close_window = false;

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
    exit(EXIT_FAILURE);
}

static void init_opengl()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        exit(EXIT_FAILURE);
    }

    s_window_width = 1280;
    s_window_height = 720;

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); // MSAA
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetSwapInterval(1); // 1: vsync, 0: fast

    s_window = SDL_CreateWindow(
        "GL Sketch",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        s_window_width, s_window_height,
        SDL_WINDOW_OPENGL
    );

//  SDL_SetWindowResizable(s_window, SDL_TRUE);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    s_context = SDL_GL_CreateContext(s_window);

#ifndef __APPLE__
    glewExperimental = GL_TRUE;
    const int glewInitResult = glewInit();
    if (glewInitResult != GLEW_OK) {
        printf("ERROR: %s\n", glewGetErrorString(glewInitResult));
        exit(EXIT_FAILURE);
    }
#endif
}

static void poll_sdl_events()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                s_should_close_window = true;
                break;

            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    s_window_width = event.window.data1;
                    s_window_height = event.window.data2;
                    glViewport(0, 0, s_window_width, s_window_height);
                }
                break;

            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    s_should_close_window = true;
                }
                input_handle_key_event(event.key.keysym.sym, true);
                break;

            case SDL_KEYUP:
                input_handle_key_event(event.key.keysym.sym, false);
                break;

            case SDL_MOUSEMOTION:
                input_handle_mouse_motion(event.motion);
                break;

            case SDL_MOUSEBUTTONDOWN:
                input_handle_mouse_click(true);
                break;

            case SDL_MOUSEBUTTONUP:
                input_handle_mouse_click(false);
                break;
        }
    }
}

int main(int argc, char** argv)
{
    init_opengl();
    webscope_open(1337);

    scene_teapots_init();

    while (!s_should_close_window) {
        webscope_update();

        const InputState input_state = input_read_state();

        scene_teapots_step(&input_state, s_window_width, s_window_height);

        SDL_GL_SwapWindow(s_window);
        poll_sdl_events();
    }

    webscope_close();
    scene_teapots_destroy();

    SDL_GL_DeleteContext(s_context);
    SDL_DestroyWindow(s_window);
    SDL_Quit();

    return 0;
}
