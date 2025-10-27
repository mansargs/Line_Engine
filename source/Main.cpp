#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include "../hdrs/Map.hpp"
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
	if (!lge::Map::verifyFile(std::string(argv[1])))
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
