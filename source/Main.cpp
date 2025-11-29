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

	float windowHeight = static_cast<float>(lgeWindow.getWindowHeight());
	float windowWidth = static_cast<float>(lgeWindow.getWindowWidth());
	float gridHeight = static_cast<float>(lgeMap.getMapHeight());
	float gridWidth = static_cast<float>(lgeMap.getMapWidth());
	float maxZ = lgeMap.getMaxZ();
	float minZ = lgeMap.getMinZ();

	// Set fixed uniforms
	shader.setFloatUniform("windowHeight", windowHeight);
	shader.setFloatUniform("windowWidth", windowWidth);
	shader.setFloatUniform("gridHeight", gridHeight);
	shader.setFloatUniform("gridWidth", gridWidth);
	shader.setFloatUniform("maxZ", maxZ);
	shader.setFloatUniform("minZ", minZ);

	// Auto-scaling and centering computation
	float cos30 = 0.86602540378f;
	float sin30 = 0.5f;

	// Compute projected bounds (extents in isometric space)
	// Assumes world coords: x [0, gridWidth], y [0, gridHeight], z_norm [0,1]
	float projX_min = 0.0f * cos30 - 1.0f * sin30;  // x_min * cos - z_norm_max * sin = -sin30
	float projX_max = gridWidth * cos30 - 0.0f * sin30;  // x_max * cos - z_norm_min * sin
	float projY_min = 0.0f + 0.0f * sin30 + 0.0f * cos30;  // y_min + x_min * sin + z_norm_min * cos = 0
	float projY_max = gridHeight + gridWidth * sin30 + 1.0f * cos30;  // y_max + x_max * sin + z_norm_max * cos

	float proj_centerX = (projX_min + projX_max) * 0.5f;
	float proj_centerY = (projY_min + projY_max) * 0.5f;

	float proj_width = projX_max - projX_min;
	float proj_height = projY_max - projY_min;

	// Compute fitting scale (min of X/Y to preserve aspect ratio)
	float scaleX = (0.95f * windowWidth) / proj_width;   // 95% margin
	float scaleY = (0.95f * windowHeight) / proj_height;
	float scale = std::min(scaleX, scaleY);

	// Compute offsets to center (accounting for the +0.5 pixel shift in shader)
	float offsetX = (windowWidth * 0.5f) - (scale * proj_centerX) - 0.5f;
	float offsetY = (windowHeight * 0.5f) - (scale * proj_centerY) - 0.5f;

	// Build combined transformation matrix (MVP) that reproduces
	// the previous per-vertex math (isometric projection, scale, offset, NDC)

	// P: isometric projection from (x, y, z_norm) -> (projX, projY)
	glm::mat4 P(0.0f);
	// Note: glm is column-major: m[col][row]
	P[0][0] = cos30;   // x' = cos30 * x + ...
	P[2][0] = -sin30;  // x' += -sin30 * z

	P[0][1] = sin30;   // y' += sin30 * x
	P[1][1] = 1.0f;    // y' += 1.0 * y
	P[2][1] = cos30;   // y' += cos30 * z

	P[3][3] = 1.0f;

	// S: scale in screen space
	glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, 1.0f));

	// T: translate by offset + 0.5 (pixel correction applied in original shader)
	glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(offsetX + 0.5f, offsetY + 0.5f, 0.0f));

	// N: normalize to NDC (scale x by 2/windowWidth, y by -2/windowHeight)
	glm::mat4 N = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f / windowWidth, -2.0f / windowHeight, 1.0f));

	// B: final bias translation to account for the -1 / +1 in NDC
	glm::mat4 B = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 1.0f, 0.0f));

	glm::mat4 MVP = B * N * T * S * P;

	shader.setMat4Uniform("uMVP", MVP);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glLineWidth(lgeWindow.Config.getScaleFactor());

	lgeWindow.startLoop();
	while (lgeWindow.isRunning()) {
		lgeWindow.pollEvents();
		// Recompute MVP each frame using current RenderConfig values
		// Auto-scale computed earlier (scale) is used as base; multiply by user scaleFactor
		float cfgScale = lgeWindow.Config.getScaleFactor();
		float usedScale = scale * cfgScale;

		float cfgOffsetX = lgeWindow.Config.getOffsetX();
		float cfgOffsetY = lgeWindow.Config.getOffsetY();

		// P: projection matrix (isometric or top-down)
		glm::mat4 P(0.0f);
		if (lgeWindow.Config.getTopView()) {
			// top view: x -> x, y -> y (ignore z for projection)
			P[0][0] = 1.0f;
			P[1][1] = 1.0f;
		} else {
			// isometric
			P[0][0] = cos30;   // x' = cos30 * x + ...
			P[2][0] = -sin30;  // x' += -sin30 * z

			P[0][1] = sin30;   // y' += sin30 * x
			P[1][1] = 1.0f;    // y' += 1.0 * y
			P[2][1] = cos30;   // y' += cos30 * z
		}
		P[3][3] = 1.0f;

		// Model rotations from config (applied before projection)
		glm::mat4 M = glm::mat4(1.0f);
		float rx = glm::radians(lgeWindow.Config.getRotateX());
		float ry = glm::radians(lgeWindow.Config.getRotateY());
		float rz = glm::radians(lgeWindow.Config.getRotateZ());
		if (rx != 0.0f) M = glm::rotate(M, rx, glm::vec3(1.0f, 0.0f, 0.0f));
		if (ry != 0.0f) M = glm::rotate(M, ry, glm::vec3(0.0f, 1.0f, 0.0f));
		if (rz != 0.0f) M = glm::rotate(M, rz, glm::vec3(0.0f, 0.0f, 1.0f));

		// S: scale in screen space
		glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(usedScale, usedScale, 1.0f));

		// T: translate by offset + user offset + 0.5 pixel correction
		glm::mat4 T = glm::translate(glm::mat4(1.0f),
							 glm::vec3(offsetX + cfgOffsetX + 0.5f,
								   offsetY + cfgOffsetY + 0.5f, 0.0f));

		// N: normalize to NDC (scale x by 2/windowWidth, y by -2/windowHeight)
		glm::mat4 N = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f / windowWidth,
									 -2.0f / windowHeight, 1.0f));

		// B: bias to shift to NDC range [-1,1]
		glm::mat4 B = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 1.0f, 0.0f));

		glm::mat4 MVP = B * N * T * S * P * M;
		shader.setMat4Uniform("uMVP", MVP);

		lgeWindow.render(shader, lineBuffer, vertexCount);
		lgeWindow.swapBuffers();
	}

	return EXIT_SUCCESS;
}
