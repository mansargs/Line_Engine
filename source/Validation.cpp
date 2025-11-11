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
		if (strNumber.length() == 1 && strNumber[0] == '0')
			return true;
		if (strNumber[0] == '0')
			return false;
		if (strNumber.length() > 3)
			return false;
		for (char c : strNumber) {
			if (!std::isdigit(static_cast<unsigned char>(c)))
				return false;
		}
		return true;
	}

	bool Map::validColor(const std::string &strColor) {
		size_t i = 1;
		if (strColor.empty())
			return true;
		if (strColor.length() != 6 && strColor.length() != 7
			&& strColor.length() != 9 && strColor.length() != 11)
			return false;
		if (strColor[i] != '0' || (strColor[i + 1] != 'x' && strColor[i + 1] != 'X'))
			return false;
		i += 2;
		for (; i < strColor.length(); ++i) {
			if (!std::isxdigit(static_cast<unsigned char>(strColor[i])))
				return false;
		}
		return true;
	}
} // namespace lge
