#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <string>
#include <vector>

namespace lge {

	using gridFile = std::vector<std::vector<std::string>>;

	struct Coord3 {
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};

	struct Rgba {
		float r = 0.0f;
		float g = 0.0f;
		float b = 0.0f;
		float a = 255.0f;
	};

	struct Pixel {
		Coord3 position;
		Rgba color;
	};

	class Map {
		using gridPixels = std::vector<std::vector<Pixel>>;

		private:
			size_t mapWidth{0};
			size_t mapHeight{0};
			gridPixels mapData;
		private:
			int parseLine(const std::string &line, gridFile &mapContent);
			int convertToPixels(const gridFile &mapContent);
			static bool validColor(const std::string &strColor);
			static bool validNumber(const std::string &strNumber);
			void cellColor(Pixel &cell, const std::string &color);
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
			static bool verifyFile(const std::string &filename);
			std::vector<float> getMapVertexes() const;
	};
}
