#pragma once

#include <vector>
#include <string>
#include <SDL2/SDL.h>
#include <GL/glew.h>

namespace lge
{

	using gridContent = std::vector<std::vector<std::string>>;

	struct Coord3
	{
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};

	struct Rgba
	{
		float r = 0.0f;
		float g = 0.0f;
		float b = 0.0f;
		float a = 0.0f;
	};

struct Pixel
{
	Coord3		position;
	Rgba		color;
};

	class Map
	{
		using gridPixels = std::vector<std::vector<Pixel>>;
		private:
			size_t		mapWidth;
			size_t		mapHeight;
			gridPixels	mapPixels;
		private:
			int			parseLine(const std::string &line, gridContent &mapContent);
			int			convertToPixels(const gridContent &mapContent);
			void		printMap(const gridContent &mapContent) const;
			static bool	validColor(const std::string &strColor);
			static bool	validNumber(const std::string &strNumber);
			void		cellColor(Pixel &cell, const std::string &color);
		public:
			Map();
			~Map();
			int		parseFile(const std::string &filename);
			size_t	getWindowWidth() const;
			size_t	getWindowHeight() const;
			static bool	verifyFile(const std::string &filename);

	};

}
