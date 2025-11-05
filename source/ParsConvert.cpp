#include "../hdrs/Map.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

namespace lge {
void Map::cellColor(Pixel &cell, const std::string &color) {
  if (color.empty()) {
    cell.color.r = 255.0f;
    cell.color.g = 255.0f;
    cell.color.b = 255.0f;
    cell.color.a = 255.0f;
    return;
  }
  unsigned int coloring = std::stoul(color, nullptr, 16);
  cell.color.r = (coloring >> 16) & 0xFF;
  cell.color.g = (coloring >> 8) & 0xFF;
  cell.color.b = coloring & 0xFF;
  if (color.length() == 4 || color.length() == 8)
    cell.color.a = (coloring >> 24) & 0xFF;
}

int Map::convertToPixels(const gridFile &mapContent) {
  mapData.resize(mapHeight);
  for (auto &row : mapData)
    row.resize(mapWidth);

  for (size_t i = 0; i < mapHeight; ++i) {
    for (size_t j = 0; j < mapWidth; ++j) {
      std::string zAxis, color;
      size_t found = mapContent[i][j].find(',');
      if (found == std::string::npos)
        zAxis = mapContent[i][j];
      else {
        zAxis = mapContent[i][j].substr(0, found);
        color = mapContent[i][j].substr(found);
      }
      if (!validNumber(zAxis))
        return -1;
      if (!validColor(color))
        return -1;
      mapData[i][j].position.x = j;
      mapData[i][j].position.y = i;
      mapData[i][j].position.z = std::stoi(zAxis);
      cellColor(mapData[i][j], color.substr(1));
    }
  }
  return 0;
}

int Map::parseLine(const std::string &line, gridFile &mapContent) {
  std::istringstream lineStream(line);
  std::string cell;
  size_t widthEach = 0;
  std::vector<std::string> row;

  while (lineStream >> cell) {
    row.push_back(cell);
    ++widthEach;
  }
  if (!lineStream.eof()) {
    std::cerr << "Error reading line\n";
    return -1;
  }
  mapContent.push_back(row);
  ++mapHeight;
  if (widthEach > mapWidth)
    mapWidth = widthEach;
  return 0;
}

int Map::parseFile(const std::string &filename) {
  std::ifstream file(filename);
  std::string line;
  gridFile mapContent;

  if (file.is_open()) {
    while (std::getline(file, line)) {
      if (parseLine(line, mapContent) != 0)
        return -1;
    }
    if (!file.eof()) {
      std::cerr << "Error reading file\n";
      return -1;
    }
  } else {
    std::cerr << "Error opening file\n";
    return -1;
  }
  convertToPixels(mapContent);
  return 0;
};
} // namespace lge
