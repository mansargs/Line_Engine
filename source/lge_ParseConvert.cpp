#include <iostream>
#include <sstream>
#include <fstream>
#include "../hdrs/Lge.hpp"

void	Lge::cellColor(Pixel &cell, const std::string &color)
{
	if (color.empty())
		return ;
	unsigned int coloring = std::stoul(color, nullptr, 16);
	cell.color.r = (coloring >> 16) & 0xFF;
	cell.color.g = (coloring >> 8) & 0xFF;
	cell.color.b = coloring & 0xFF;
	if (color.length() == 4 || color.length() == 8)
		cell.color.a = (coloring >> 24) & 0xFF;
}

int	Lge::convertToPixels(const gridContent &mapContent)
{
	mapPixels.resize(height);
	for (auto &row : mapPixels)
	row.resize(width);

	for (size_t i = 0; i < height; ++i)
	{
		for (size_t j = 0; j < width; ++j)
		{
			std::string zAxis, color;
			size_t found = mapContent[i][j].find(',');
			if (found == std::string::npos)
				zAxis = mapContent[i][j];
			else
			{
				zAxis = mapContent[i][j].substr(0, found);
				color = mapContent[i][j].substr(found);
			}
			if (!validNumber(zAxis))
				return -1;
			if (!validColor(color))
				return -1;
			mapPixels[i][j].position.x = j;
			mapPixels[i][j].position.y = i;
			mapPixels[i][j].position.z = std::stoi(zAxis);
			cellColor(mapPixels[i][j], color.erase(0, 3));
		}
	}
	return 0;
}

int Lge::parseLine(const std::string &line, gridContent &mapContent)
{
	std::istringstream			lineStream(line);
	std::string					cell;
	size_t						widthEach = 0;
	std::vector<std::string>	row;

	while (lineStream >> cell)
	{
		row.push_back(cell);
		++widthEach;
	}
	if (!lineStream.eof())
	{
		std::cerr << "Error reading line\n";
		return -1;
	}
	mapContent.push_back(row);
	++height;
	if (widthEach > width)
		width = widthEach;
	return 0;
}

int	Lge::parseFile(const std::string &filename)
{
	std::ifstream	file(filename);
	std::string		line;
	gridContent		mapContent;

	if (file.is_open())
	{
		while (std::getline(file, line))
		{
			if (parseLine(line, mapContent) != 0)
				return -1;
		}
		if (!file.eof())
		{
			std::cerr << "Error reading file\n";
			return -1;
		}
	}
	else
	{
		std::cerr << "Error opening file\n";
		return -1;
	}
	convertToPixels(mapContent);
	// printMap(mapContent);
	return 0;
};
