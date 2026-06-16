#include "../hdrs/Paths.hpp"

#include <cstdlib>
#include <filesystem>
#include <stdexcept>

namespace lge {

namespace {

bool hasShaderDir(const std::filesystem::path &base) {
	const auto vertex = base / "Shaders" / "vertex.glsl";
	const auto fragment = base / "Shaders" / "fragment.glsl";
	return std::filesystem::exists(vertex) && std::filesystem::exists(fragment);
}

} // namespace

std::string getExecutableDir() {
#if defined(PLATFORM_LINUX)
	std::error_code ec;
	const auto exePath = std::filesystem::read_symlink("/proc/self/exe", ec);
	if (!ec)
		return exePath.parent_path().string();
#elif defined(PLATFORM_MAC)
	std::error_code ec;
	const auto exePath = std::filesystem::read_symlink("/proc/self/exe", ec);
	if (!ec)
		return exePath.parent_path().string();
#endif
	return std::filesystem::current_path().string();
}

std::string resolveDataDir() {
	if (const char *envDir = std::getenv("LGE_DATA_DIR")) {
		const std::string dir(envDir);
		if (hasShaderDir(dir))
			return dir;
		throw std::runtime_error(std::string("LGE_DATA_DIR is set but shaders were not found in: ") + dir);
	}

	const std::string exeDir = getExecutableDir();
	if (hasShaderDir(exeDir))
		return exeDir;

#ifdef LGE_DATA_DIR
	{
		const std::string installDir(LGE_DATA_DIR);
		if (hasShaderDir(installDir))
			return installDir;
	}
#endif

#ifdef LGE_DEV_DATA_DIR
	{
		const std::string devDir(LGE_DEV_DATA_DIR);
		if (hasShaderDir(devDir))
			return devDir;
	}
#endif

	throw std::runtime_error(
		"Could not locate LineEngine data directory (Shaders/). "
		"Set LGE_DATA_DIR or reinstall the application.");
}

std::string shaderPath(const std::string &dataDir, const char *filename) {
	return (std::filesystem::path(dataDir) / "Shaders" / filename).string();
}

} // namespace lge
