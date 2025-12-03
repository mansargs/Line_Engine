#include "../hdrs/Window.hpp"
#include "../hdrs/Map.hpp"
#include "../hdrs/Buffer.hpp"
#include "../hdrs/Shader.hpp"
#include "../hdrs/RenderConfig.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <cstdlib>
#include <algorithm>
#include <iostream>

int main(int argc, const char *argv[]) {
	try {
		if (argc != 2) {
			std::cerr << "\033[31mProgram must have one argument with the .lge "
					  "extension!\033[0m\n";
			return EXIT_FAILURE;
		}
		if (!lge::Map::verifyFile(std::string(argv[1])))
			return EXIT_FAILURE;
		lge::Map &lgeMap = lge::Map::getInstance();
		if (lgeMap.parseFile(argv[1]) != 0)
			return EXIT_FAILURE;
		lge::Window &lgeWindow = lge::Window::getInstance();
		lgeWindow.Config.setZScale(lgeMap.calculateZScale());
		std::vector<float> vertices = lgeMap.getMapVertexes();
		const GLsizei vertexCount = static_cast<GLsizei>(vertices.size() / 7);
		lge::Buffer lineBuffer(vertices.data(), static_cast<GLsizei>(vertices.size() * sizeof(float)));
		lge::Shader shader("Shaders/vertex.glsl", "Shaders/fragment.glsl");
		shader.use();
		lgeWindow.setupRendering();
		while (lgeWindow.isRunning()) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			lgeWindow.pollEvents();
			float projZoom = 1.2f * lgeWindow.Config.getScale() * lgeWindow.Config.getScaleFactor();
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
	} catch (...){
		std::cerr << "\033[31mAn unexpected error occurred!\033[0m\n";
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
