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
	std::vector<float> vertices = lgeMap.getMapVertexes();
	const GLsizei vertexCount = static_cast<GLsizei>(vertices.size() / 7);
	lge::Buffer lineBuffer(vertices.data(), static_cast<GLsizei>(vertices.size() * sizeof(float)));
	lge::Shader shader("Shaders/vertex.glsl", "Shaders/fragment.glsl");
	shader.use();
	float maxZ = lgeMap.getMaxZ();
	float minZ = lgeMap.getMinZ();
	shader.setFloatUniform("maxZ", maxZ);
	shader.setFloatUniform("minZ", minZ);
	lgeWindow.computeScaleOffset(lgeMap.getMapWidth(), lgeMap.getMapHeight(),
												lgeWindow, lgeWindow.Config.getIsometric());

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glLineWidth(lgeWindow.Config.getScaleFactor());

	lgeWindow.startLoop();
	while (lgeWindow.isRunning()) {
		lgeWindow.pollEvents();

		float usedScale = lgeWindow.Config.getScale() * lgeWindow.Config.getScaleFactor();
		glm::mat4 P = glm::mat4(1.0f);
		glm::mat4 M = glm::mat4(1.0f);
		glm::mat4 R = glm::mat4(1.0f);
		if (lgeWindow.Config.getRotateX() != 0.0f)
			R *= glm::rotate(R, glm::radians(lgeWindow.Config.getRotateX()),
							glm::vec3(1.0f, 0.0f, 0.0f));
		if (lgeWindow.Config.getRotateY() != 0.0f)
			R *= glm::rotate(R, glm::radians(lgeWindow.Config.getRotateY()),
							glm::vec3(0.0f, 1.0f, 0.0f));
		if (lgeWindow.Config.getRotateZ() != 0.0f)
			R *= glm::rotate(R, glm::radians(lgeWindow.Config.getRotateZ()),
							glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(usedScale, usedScale, 1.0f));
		glm::mat4 T = glm::translate(glm::mat4(1.0f),
										glm::vec3(lgeWindow.Config.getOffsetX() + 5.0f,
											lgeWindow.Config.getOffsetY(), 0.0f));
		glm::mat4 N = glm::scale(glm::mat4(1.0f),
							 glm::vec3(2.0f / lgeWindow.getWindowWidth(),
									   -2.0f / lgeWindow.getWindowHeight(),
									   1.0f));
		glm::mat4 B = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 1.0f, 0.0f));

		glm::mat4 MVP = B * N * T * S * R * P * M;
		shader.setMat4Uniform("uMVP", MVP);
		lgeWindow.render(shader, lineBuffer, vertexCount);
		lgeWindow.swapBuffers();
	}

	return EXIT_SUCCESS;
}
