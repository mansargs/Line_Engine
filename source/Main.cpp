#include "../hdrs/Window.hpp"
#include "../hdrs/Map.hpp"
#include "../hdrs/Buffer.hpp"
#include "../hdrs/Shader.hpp"
#include "../hdrs/RenderConfig.hpp"
#include "../hdrs/Paths.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <string>

namespace {

std::string resolveMapPath(int argc, const char *argv[]) {
	if (argc == 2)
		return argv[1];

#if defined(PLATFORM_LINUX)
	std::string command =
		"zenity --file-selection "
		"--title='Select a LineEngine map file' "
		"--file-filter='LineEngine maps | *.lge' 2>/dev/null";

	FILE *pipe = popen(command.c_str(), "r");
	if (pipe) {
		char buffer[4096];
		if (fgets(buffer, sizeof(buffer), pipe)) {
			std::string selected(buffer);
			while (!selected.empty() && (selected.back() == '\n' || selected.back() == '\r'))
				selected.pop_back();
			pclose(pipe);
			if (!selected.empty())
				return selected;
		}
		pclose(pipe);
	}
#endif

	std::cerr << "\033[31mUsage: LineEngine <map.lge>\033[0m\n";
	std::cerr << "Provide a .lge file path, or launch from the desktop file picker.\n";
	return {};
}

} // namespace

int main(int argc, const char *argv[]) {
	try {
		const std::string mapPath = resolveMapPath(argc, argv);
		if (mapPath.empty())
			return EXIT_FAILURE;

		if (!lge::Map::verifyFile(mapPath))
			return EXIT_FAILURE;

		const std::string dataDir = lge::resolveDataDir();
		lge::Map &lgeMap = lge::Map::getInstance();
		if (lgeMap.parseFile(mapPath) != 0)
			return EXIT_FAILURE;

		lge::Window &lgeWindow = lge::Window::getInstance();
		lgeWindow.Config.setZScale(lgeMap.calculateZScale());
		std::vector<float> vertices = lgeMap.getMapVertexes();
		const GLsizei vertexCount = static_cast<GLsizei>(vertices.size() / 7);
		lge::Buffer lineBuffer(vertices.data(), static_cast<GLsizei>(vertices.size() * sizeof(float)));

		lge::Shader shader(
			lge::shaderPath(dataDir, "vertex.glsl").c_str(),
			lge::shaderPath(dataDir, "fragment.glsl").c_str());
		shader.use();
		lgeWindow.setupRendering();

		while (lgeWindow.isRunning()) {
			lgeWindow.pollEvents();
			float projZoom = 0.9f * lgeWindow.Config.getScale() * lgeWindow.Config.getScaleFactor();
			float translationStep = 0.5f / projZoom;
			float offsetX = lgeWindow.Config.getOffsetX() * translationStep;
			float offsetY = -lgeWindow.Config.getOffsetY() * translationStep;
			glm::vec3 center(lgeMap.getMapWidth() / 2.0f, lgeMap.getMapHeight() / 2.0f,
							 (lgeMap.getMinZ() + lgeMap.getMaxZ()) / 2.0f);
			glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, lgeWindow.Config.getZScale()));
			glm::mat4 M = lgeWindow.Rotation() * S * glm::translate(glm::mat4(1.0f), -center);
			glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(offsetX, offsetY, 0.0f));

			glm::mat4 MVP = lgeWindow.Projection(projZoom, lgeMap) * T * M;
			shader.setMat4Uniform("uMVP", MVP);
			lgeWindow.render(shader, lineBuffer, vertexCount);
			lgeWindow.swapBuffers();
		}
	} catch (const std::exception &e) {
		std::cerr << "\033[31mError: " << e.what() << "\033[0m\n";
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
