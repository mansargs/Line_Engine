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
	// Z scaling & centers (extrude Z=10 to ~43 units for visibility)
float mapCenterX = static_cast<float>(lgeMap.getMapWidth()) / 2.0f;
float mapCenterY = static_cast<float>(lgeMap.getMapHeight()) / 2.0f;
float mapCenterZ = (lgeMap.getMaxZ() + lgeMap.getMinZ()) / 2.0f;
float zRange = lgeMap.getMaxZ() - lgeMap.getMinZ();
float xyDiagonal = std::sqrt(mapCenterX * mapCenterX + mapCenterY * mapCenterY);
float zScale = (zRange > 0.0f) ? (xyDiagonal * 2.0f) / zRange : 1.0f;
zScale *= 2.0f;  // Exaggerate 2x—tune to 1.5f if too tall

// Drop unused uniforms
// shader.setFloatUniform("maxZ", maxZ);
// shader.setFloatUniform("minZ", minZ);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glLineWidth(lgeWindow.Config.getScaleFactor());

	lgeWindow.startLoop();
	while (lgeWindow.isRunning()) {
		lgeWindow.pollEvents();

		float usedScale = lgeWindow.Config.getScale() * lgeWindow.Config.getScaleFactor();

// Fixed rotations (fresh mat4—no compounding bug)
glm::mat4 R = glm::mat4(1.0f);
if (lgeWindow.Config.getRotateX() != 0.0f)
    R *= glm::rotate(glm::mat4(1.0f), glm::radians(lgeWindow.Config.getRotateX()), glm::vec3(1,0,0));
if (lgeWindow.Config.getRotateY() != 0.0f)
    R *= glm::rotate(glm::mat4(1.0f), glm::radians(lgeWindow.Config.getRotateY()), glm::vec3(0,1,0));
if (lgeWindow.Config.getRotateZ() != 0.0f)
    R *= glm::rotate(glm::mat4(1.0f), glm::radians(lgeWindow.Config.getRotateZ()), glm::vec3(0,0,1));

// Model: Center pivot (no corner swing)
glm::mat4 M = glm::translate(glm::mat4(1.0f), glm::vec3(mapCenterX, mapCenterY, mapCenterZ)) *
              R *
              glm::translate(glm::mat4(1.0f), glm::vec3(-mapCenterX, -mapCenterY, -mapCenterZ));

// Scale: XY uniform, Z exaggerated
glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(usedScale, usedScale, usedScale * zScale));

// Translate (drop +5 hack)
glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(lgeWindow.Config.getOffsetX(), lgeWindow.Config.getOffsetY(), 0.0f));

// Dynamic ortho (no clip, Y flip for upright map)
float modelExtent = xyDiagonal + (zRange * zScale / 2.0f);  // Buffer for Z
float projBuffer = usedScale * modelExtent * 1.2f;
glm::mat4 Proj = glm::ortho(-projBuffer, projBuffer, projBuffer, -projBuffer,  // Y flip: row 0 at top
                            -zRange * zScale * 1.5f, zRange * zScale);  // Tight Z

// MVP (drop P/M identity)
glm::mat4 MVP = Proj * T * S * M;
shader.setMat4Uniform("uMVP", MVP);
		lgeWindow.render(shader, lineBuffer, vertexCount);
		lgeWindow.swapBuffers();
	}

	return EXIT_SUCCESS;
}
