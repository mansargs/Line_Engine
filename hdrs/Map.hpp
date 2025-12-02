#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <glm/glm.hpp>

namespace lge {

	using gridFile = std::vector<std::vector<std::string>>;

	struct Pixel {
		glm::vec3 position{0.0f};
		glm::vec3 color{0.0f};
	};

	class Map {
		using gridPixels = std::vector<std::vector<Pixel>>;

		private:
			size_t mapWidth{0};
			size_t mapHeight{0};
			float maxZ{0.0f};
			float minZ{0.0f};
			gridPixels mapData;
		private:
			int parseLine(const std::string &line, gridFile &mapContent);
			int convertToPixels(const gridFile &mapContent);
			static bool validColor(const std::string &strColor);
			static bool validNumber(const std::string &strNumber);
			void cellColor(Pixel &cell, const std::string &color);
			glm::vec3 getLeveledColor(float level, float z);
			void addVertex(std::vector<float> &vertex, const Pixel &data) const;
		private:
			Map() = default;
			Map(const Map &) = delete;
			Map &operator=(const Map &) = delete;
			Map(Map &&) = delete;
			Map &operator=(Map &&) = delete;
		public:
			static Map &getInstance();
			int parseFile(const std::string &filename);
			size_t getMapWidth() const;
			size_t getMapHeight() const;
			float getMaxZ() const;
			float getMinZ() const;
			static bool verifyFile(const std::string &filename);
			float calculateZScale();
			std::vector<float> getMapVertexes() const;
	};
}
