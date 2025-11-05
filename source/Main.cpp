#include "../hdrs/Desktop.hpp"
#include "../hdrs/Map.hpp"
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>

int main(int argc, const char *argv[]) {
  if (argc != 2) {
    std::cerr << "\033[31mProgram must have one argument with the .lge "
                 "extension!\033[0m\n";
    return EXIT_FAILURE;
  }
  if (!lge::Map::verifyFile(std::string(argv[1])))
    return EXIT_FAILURE;
  lge::Map &lgeMap = lge::Map::getInstance();
  lge::Desktop &lgeWindow = lge::Desktop::getInstance();
  lgeMap.parseFile(argv[1]);

  return EXIT_SUCCESS;
}
