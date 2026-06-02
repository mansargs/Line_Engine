#include "../hdrs/Map.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace lge {

	bool Map::verifyFile(const std::string &filename) {
		std::filesystem::path filePath = filename;
		if (!std::filesystem::exists(filePath)) {
			std::cerr << "\033[31mError: File doesn't exist\033[0m\n";
			return false;
		}
		if (filePath.extension() != ".lge") {
			std::cerr << "\033[31mError: Extension is not lge\033[0m\n";
			return false;
		}
		std::ifstream file(filePath);
		if (!file.is_open()) {
			std::cerr << "\033[31mError: File cannot open file\033[0m\n";
			return false;
		}
		return true;
	}

	bool Map::validNumber(const std::string &strNumber) {
		if (strNumber.empty())
			return false;
		size_t startIdx = 0;
		if (strNumber[0] == '-') {
			if (strNumber.length() == 1)
				return false;
			startIdx = 1;
		}
		if (strNumber.length() == startIdx + 1 && strNumber[startIdx] == '0')
			return true;
		if (strNumber[startIdx] == '0' && strNumber.length() > startIdx + 1)
			return false;
		if (strNumber.length() - startIdx > 5)
			return false;
		for (size_t i = startIdx; i < strNumber.length(); ++i) {
			if (!std::isdigit(static_cast<unsigned char>(strNumber[i])))
				return false;
		}
		return true;
	}

	bool Map::validColor(const std::string &strColor) {
		size_t i = 0;
		if (strColor.empty())
			return true;
		if (strColor[i] != '0' || (strColor[i + 1] != 'x' && strColor[i + 1] != 'X'))
			return false;
		i += 2;
		if ( strColor.length() < 3 || strColor.length() > 8)
			return false;
		for (; i < strColor.length(); ++i) {
			if (!std::isxdigit(static_cast<unsigned char>(strColor[i])))
				return false;
		}
		return true;
	}
} 
