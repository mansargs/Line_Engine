#include "../hdrs/Map.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

namespace lge {

	void Map::cellColor(Pixel &cell, const std::string &color) {
		if (color.empty()) {
			if (cell.position.z < 0)
				cell.color = {0.0f, 0.0f, 255.0f, 255.0f};
			else if (cell.position.z > 0)
				cell.color = {255.0f, 0.0f, 0.0f, 255.0f};
			else
				cell.color = {255.0f, 255.0f, 255.0f, 255.0f};
			return;
		}
		std::string hex = color;
		if (hex.empty())
			return;
		if (hex.rfind("0x", 0) == 0 || hex.rfind("0X", 0) == 0)
			hex = hex.substr(2);
		if (hex.length() == 3) {
				std::string expanded;
				for (char c : hex) {
					expanded.push_back(c);
					expanded.push_back(c);
				}
				hex = expanded;
		}
		auto padLeft = [](const std::string &value, std::size_t target) {
				if (value.length() >= target)
					return value;
				return std::string(target - value.length(), '0') + value;
		};
		std::string normalized = hex.length() <= 6 ? padLeft(hex, 6) : padLeft(hex, 8);
		unsigned int val = std::stoul(normalized, nullptr, 16);
		if (normalized.length() == 6) {
				cell.color.r = static_cast<float>((val >> 16) & 0xFF);
				cell.color.g = static_cast<float>((val >> 8) & 0xFF);
				cell.color.b = static_cast<float>(val & 0xFF);
				cell.color.a = 255.0f;
		} else {
				cell.color.r = static_cast<float>((val >> 24) & 0xFF);
				cell.color.g = static_cast<float>((val >> 16) & 0xFF);
				cell.color.b = static_cast<float>((val >> 8)  & 0xFF);
				cell.color.a = static_cast<float>(val & 0xFF);
				if (cell.color.a == 0.0f)
					cell.color.a = 255.0f;
		}
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
				mapData[i][j].position.x = static_cast<float> (j);
				mapData[i][j].position.y = static_cast<float> (i);
				mapData[i][j].position.z = static_cast<float> (std::stoi(zAxis));
				cellColor(mapData[i][j], color.empty() ? std::string() : color.substr(1));
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
		if (mapHeight == 1) {
			mapWidth = widthEach;
		} else if (widthEach != mapWidth) {
			std::cerr << "Error: map rows must have equal width\n";
			return -1;
		}
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
