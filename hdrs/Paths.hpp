#pragma once

#include <string>

namespace lge {

std::string getExecutableDir();
std::string resolveDataDir();
std::string shaderPath(const std::string &dataDir, const char *filename);

} // namespace lge
