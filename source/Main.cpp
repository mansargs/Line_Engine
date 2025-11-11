#include "../hdrs/Window.hpp"
#include "../hdrs/Map.hpp"
#include "../hdrs/Buffer.hpp"
#include "../hdrs/Shader.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <cstdlib>
#include <filesystem>
#include <fstream>
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
	lge::Window &lgeWindow = lge::Window::getInstance();
	std::cout << " height --->  " << lgeWindow.getWindowHeight() << " \nWidth ----->   "
				<< lgeWindow.getWindowWidth() << std::endl;
	if (lgeMap.parseFile(argv[1]) != 0)
		return EXIT_FAILURE;
	std::vector<float> vertices = lgeMap.getMapVertexes();
	const GLsizei vertexCount = static_cast<GLsizei>(vertices.size() / 7);
	lge::Buffer lineBuffer(vertices.data(), static_cast<GLsizei>(vertices.size() * sizeof(float)));
	std::cerr << "Map size (w x h): " << lgeMap.getMapWidth() << " x " << lgeMap.getMapHeight()
						<< ", vertices: " << vertexCount << "\n";

	lge::Shader shader("Shaders/vertex.glsl", "Shaders/fragment.glsl");
	shader.use();
	shader.setFloatUniform("windowHeight", static_cast<float>(lgeWindow.getWindowHeight()));
	shader.setFloatUniform("windowWidth", static_cast<float>(lgeWindow.getWindowWidth()));
	shader.setFloatUniform("gridHeight", static_cast<float>(lgeMap.getMapHeight()));
	shader.setFloatUniform("gridWidth", static_cast<float>(lgeMap.getMapWidth()));

 	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
 	glLineWidth(1.0f);

	lgeWindow.startLoop();
	while (lgeWindow.isRunning()) {
		lgeWindow.pollEvents();

    glClear(GL_COLOR_BUFFER_BIT);
    std::string err = lge::Buffer::checkError("Post-Clear");  // Add this
    if (!err.empty()) std::cerr << "Clear Error: " << err << std::endl;

    shader.use();
    err = lge::Buffer::checkError("Shader Use");
    if (!err.empty()) std::cerr << "Shader Error: " << err << std::endl;

    lineBuffer.bind();
    err = lge::Buffer::checkError("Buffer Bind");
    if (!err.empty()) std::cerr << "Bind Error: " << err << std::endl;

    glDrawArrays(GL_LINES, 0, vertexCount);  // Use your pre-computed vertexCount
    err = lge::Buffer::checkError("DrawArrays");
    if (!err.empty()) std::cerr << "Draw Error: " << err << std::endl;

    lineBuffer.unbind();
    lgeWindow.swapBuffers();
	}

	return EXIT_SUCCESS;
}
