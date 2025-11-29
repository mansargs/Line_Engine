#include "../hdrs/Map.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <limits>
#include <algorithm>
#include <glm/glm.hpp>

namespace lge {
	#include <algorithm>  // For std::clamp
#include <cmath>      // For std::fabs, std::round

void Map::cellColor(Pixel &cell, const std::string &color) {
    if (color.empty()) {
        // Handle z=0 distinctly with a very light blue + green (pale cyan-mint), blending toward white
        if (std::fabs(cell.position.z) < 0.1f) {  // Epsilon for floating-point precision
            cell.color = {230.0f, 255.0f, 240.0f};  // Very light blue-green (mint cream tint) for z≈0
            return;
        }

        // Determine level based on |z|: Fine bands (10s) for |z|<100, coarse (100s) for |z|>=100
        float absZ = std::fabs(cell.position.z);
        float level;
        if (absZ < 100.0f) {
            level = std::round(absZ / 10.0f) * 10.0f;  // e.g., 15→20, 85→90
        } else {
            level = std::round(absZ / 100.0f) * 100.0f;  // e.g., 150→100, 250→200, ..., 850→800
        }
        level = std::clamp(level, 10.0f, 900.0f);  // Cap at 900 for extremes

        glm::vec3 col;
        if (cell.position.z < 0) {
            // Negative depths: Progressive cool blues/cyans, distinct per band (unchanged)
            // Fine bands: -10, -20, ..., -90
            if (level <= 10) col = {173.0f, 216.0f, 230.0f};  // Light sky blue for -10
            else if (level <= 20) col = {135.0f, 206.0f, 250.0f};  // Sky blue for -20
            else if (level <= 30) col = {100.0f, 149.0f, 237.0f};  // Cornflower for -30
            else if (level <= 40) col = {70.0f, 130.0f, 180.0f};   // Steel blue for -40
            else if (level <= 50) col = {65.0f, 105.0f, 225.0f};   // Royal blue for -50
            else if (level <= 60) col = {25.0f, 25.0f, 112.0f};    // Midnight blue for -60
            else if (level <= 70) col = {30.0f, 144.0f, 255.0f};   // Dodger blue for -70
            else if (level <= 80) col = {0.0f, 191.0f, 255.0f};    // Deep sky blue for -80
            else if (level <= 90) col = {0.0f, 0.0f, 139.0f};      // Dark blue for -90
            // Coarse bands: -100, -200, ..., -900
            else if (level <= 100) col = {0.0f, 0.0f, 128.0f};     // Navy for -100
            else if (level <= 200) col = {0.0f, 0.0f, 100.0f};     // Dark slate for -200
            else if (level <= 300) col = {25.0f, 25.0f, 112.0f};   // Midnight for -300
            else if (level <= 400) col = {0.0f, 0.0f, 50.0f};      // Very dark blue for -400
            else if (level <= 500) col = {0.0f, 0.0f, 30.0f};      // Ultra deep for -500
            else if (level <= 600) col = {10.0f, 10.0f, 20.0f};    // Near-black blue for -600
            else if (level <= 700) col = {5.0f, 5.0f, 15.0f};      // Abyss shadow for -700
            else if (level <= 800) col = {3.0f, 3.0f, 10.0f};      // Void blue for -800
            else col = {1.0f, 1.0f, 5.0f};  // Blackish for -900+
        } else {
            // Positive elevations: Progressive warm earth tones to reds, distinct per band
            // Fine bands: +10, +20, ..., +90 (unchanged)
            if (level <= 10) col = {124.0f, 252.0f, 0.0f};    // Lawn green for +10
            else if (level <= 20) col = {50.0f, 205.0f, 50.0f};  // Lime for +20
            else if (level <= 30) col = {107.0f, 142.0f, 35.0f}; // Olive drab for +30
            else if (level <= 40) col = {85.0f, 107.0f, 47.0f};  // Dark olive for +40
            else if (level <= 50) col = {139.0f, 69.0f, 19.0f};  // Saddle brown for +50
            else if (level <= 60) col = {160.0f, 82.0f, 45.0f};  // Saddle for +60
            else if (level <= 70) col = {184.0f, 134.0f, 11.0f}; // Dark goldenrod for +70
            else if (level <= 80) col = {218.0f, 165.0f, 32.0f}; // Goldenrod for +80
            else if (level <= 90) col = {255.0f, 215.0f, 0.0f};  // Gold for +90
            // Coarse bands: +100, +200, ..., +900
            else if (level <= 100) col = {255.0f, 140.0f, 0.0f}; // Dark orange for +100
            else if (level <= 200) col = {255.0f, 69.0f, 0.0f};  // Red orange for +200
            else if (level <= 300) col = {220.0f, 20.0f, 60.0f}; // Crimson for +300
            else if (level <= 400) col = {139.0f, 0.0f, 0.0f};   // Dark red for +400
            else if (level <= 500) col = {205.0f, 0.0f, 0.0f};   // Medium red for +500
            else if (level <= 600) col = {255.0f, 0.0f, 0.0f};   // Bright red for +600
            else if (level <= 700) col = {255.0f, 127.0f, 0.0f}; // Fiery orange for +700
            else if (level <= 800) col = {255.0f, 165.0f, 0.0f}; // Vibrant orange for +800
            else col = {255.0f, 255.0f, 255.0f};  // Pure white for +900+ (snowy peaks)
        }

        // Assign to cell (0-255 RGB floats)
        cell.color = {col.r, col.g, col.b};
        return;
    }

    // Existing hex parsing (unchanged)
    unsigned int colorValue = std::stoul(color, nullptr, 0);
    cell.color.r = static_cast<float>((colorValue >> 16) & 0xFF);
    cell.color.g = static_cast<float>((colorValue >> 8) & 0xFF);
    cell.color.b = static_cast<float>(colorValue & 0xFF);
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
					color = mapContent[i][j].substr(found + 1);
				}
				if (!validNumber(zAxis))
					return -1;
				if (!validColor(color))
					return -1;
				mapData[i][j].position.x = static_cast<float> (j);
				mapData[i][j].position.y = static_cast<float> (i);
				mapData[i][j].position.z = static_cast<float> (std::stoi(zAxis));
				if (mapData[i][j].position.z > maxZ)
					maxZ = mapData[i][j].position.z;
				else if (mapData[i][j].position.z < minZ)
					minZ = mapData[i][j].position.z;
				cellColor(mapData[i][j], color);
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

		// Reset Z bounds for new map (use extreme values so any real value updates them)
		minZ = std::numeric_limits<float>::max();
		maxZ = std::numeric_limits<float>::lowest();

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
