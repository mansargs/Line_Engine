#include "../hdrs/Map.hpp"
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace lge {
	size_t Map::getMapHeight() const { return mapHeight; }

	size_t Map::getMapWidth() const { return mapWidth; }

	void Map::addVertex(std::vector<float> &vertex, const Pixel &data) const {
		vertex.insert(vertex.end(), {data.position.x,
																data.position.y,
																data.position.z,
																data.color.r / 255.0f,
																data.color.g / 255.0f,
																data.color.b / 255.0f,
																data.color.a / 255.0f});
	}

	std::vector<float> Map::getMapVertexes() const {
		std::vector<float> vertexes;
		for (size_t i = 0; i < mapHeight; ++i) {
			for (size_t j = 0; j < mapWidth - 1; ++j) {
				addVertex(vertexes, mapData[i][j]);
				addVertex(vertexes, mapData[i][j + 1]);
			}
		}
		for (size_t i = 0; i < mapHeight - 1; ++i) {
			for (size_t j = 0; j < mapWidth; ++j) {
				addVertex(vertexes, mapData[i][j]);
				addVertex(vertexes, mapData[i + 1][j]);
			}
		}
		return vertexes;
	}

	Map &Map::getInstance() {
		static Map instance;
		return instance;
	}
} // namespace lge
