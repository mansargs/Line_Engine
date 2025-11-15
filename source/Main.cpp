#include "../hdrs/Window.hpp"
#include "../hdrs/Map.hpp"
#include "../hdrs/Buffer.hpp"
#include "../hdrs/Shader.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <cstdlib>
#include <algorithm>  // For std::min
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

    // Extract dimensions as floats for calculations
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

    // Set the dynamic uniforms using the Shader class method
    shader.setFloatUniform("scale", scale);
    shader.setFloatUniform("offsetX", offsetX);
    shader.setFloatUniform("offsetY", offsetY);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glLineWidth(1.0f);

    lgeWindow.startLoop();
    while (lgeWindow.isRunning()) {
        lgeWindow.pollEvents();
        lgeWindow.render(shader, lineBuffer, vertexCount);
        lgeWindow.swapBuffers();
    }

    return EXIT_SUCCESS;
}
