#include <iostream>
#include "input.hpp"
#include "world.hpp"
#include "render.hpp"

int main(int argc, char** argv)
{
    Renderer renderer;
    World world;
    Input::bind_handlers(renderer.raw_glfw_window());

    while (!renderer.should_close_window()) {
        auto input_state = Input::read_state();
        world = world.step(input_state);
        renderer.render(world);
    }

    #ifdef _WIN32
        std::cout << "Enter a char to quit"; char x; std::cin >> x;
    #endif

    return 0;
}