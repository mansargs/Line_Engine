#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include "../hdrs/Lge.hpp"
#include "../hdrs/Desktop.hpp"
#include <SDL2/SDL.h>
#include <GL/glew.h>

int main(int argc, const char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "\033[31mProgram must have one argument with the .lge extension!\033[0m\n";
        return EXIT_FAILURE;
    }
    if (!Lge::verifyFile(std::string(argv[1])))
        return EXIT_FAILURE;

    Lge content;
    if (content.parseFile(std::string(argv[1])) != 0)
        return EXIT_FAILURE;

    try
    {
        lgeDesktop engine;

        // Set up OpenGL viewport
        glViewport(0, 0, engine.getDesktopWidth(), engine.getDesktopHeight());

        content.draw(engine.window);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return EXIT_SUCCESS;
}
