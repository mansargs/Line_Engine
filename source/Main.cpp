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
    float mapWidth = static_cast<float>(lgeMap.getMapWidth());
    float mapHeight = static_cast<float>(lgeMap.getMapHeight());
    float mapCenterX = mapWidth / 2.0f;
    float mapCenterY = mapHeight / 2.0f;
    float mapCenterZ = (lgeMap.getMaxZ() + lgeMap.getMinZ()) / 2.0f;
    float zRange = lgeMap.getMaxZ() - lgeMap.getMinZ();
    float xyDiagonal = std::sqrt(mapCenterX * mapCenterX + mapCenterY * mapCenterY);
    float zScale = (zRange > 0.0f) ? (xyDiagonal * 2.0f) / zRange : 1.0f;
    zScale *= 0.05f; // Very small elevation (25% of XY diagonal) for subtle, near-flat view

    // Translation step size (small steps)
    // float translationStep = 0.5f; // Tune 0.1-0.5 for finer control

    // Ensure window clears to gray for visibility debugging
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    // Drop unused uniforms
    // shader.setFloatUniform("maxZ", maxZ);
    // shader.setFloatUniform("minZ", minZ);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glLineWidth(lgeWindow.Config.getScaleFactor()); // Restore DPI scaling for consistent line thickness

    lgeWindow.startLoop();
    // ... (earlier code unchanged, including zScale *= 0.05f; and prints)

while (lgeWindow.isRunning()) {
    // Your print statement here (unchanged)

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    lgeWindow.pollEvents();

    // NEW: Separate projZoom (for zoom-in effect) from model scale (fixed=1 for XY)
    float projZoom = 1.2f * lgeWindow.Config.getScale() * lgeWindow.Config.getScaleFactor();
    // translationStep now relative to projZoom for screen-consistent panning (tune if needed)
    float translationStep = 0.5f / projZoom;  // Smaller steps when zoomed in

    // Direct translation with Y inversion
    float offsetX = lgeWindow.Config.getOffsetX() * translationStep;
    float offsetY = -lgeWindow.Config.getOffsetY() * translationStep;

    // Rotations (unchanged)
    glm::mat4 R = glm::mat4(1.0f);
    R *= glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1, 0, 0));
    if (lgeWindow.Config.getRotateY() != 0.0f)
        R *= glm::rotate(glm::mat4(1.0f), glm::radians(lgeWindow.Config.getRotateY()), glm::vec3(0, 1, 0));
    if (lgeWindow.Config.getRotateX() != 0.0f)
        R *= glm::rotate(glm::mat4(1.0f), glm::radians(lgeWindow.Config.getRotateX()), glm::vec3(1, 0, 0));
    if (lgeWindow.Config.getRotateZ() != 0.0f)
        R *= glm::rotate(glm::mat4(1.0f), glm::radians(lgeWindow.Config.getRotateZ()), glm::vec3(0, 0, 1));

    // Model: FIXED scale (1 for XY, zScale for Z only) + center
    glm::vec3 center(mapCenterX, mapCenterY, mapCenterZ);
    glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, zScale));  // No projZoom here!
    glm::mat4 M = R * S * glm::translate(glm::mat4(1.0f), -center);

    // View pan (unchanged, but step scaled above)
    glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(offsetX, offsetY, 0.0f));

    // Window aspect (unchanged)
    int windowWidth = lgeWindow.getWindowWidth();
    int windowHeight = lgeWindow.getWindowHeight();
    float aspect = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);

    // NEW: Exact 3D half-diagonal for bounds (covers tilt/rotation)
    float halfX = mapWidth / 2.0f;
    float halfY = mapHeight / 2.0f;
    float halfZ = zScale * (zRange / 2.0f);  // Scaled Z half-extent
    float halfDiag = std::sqrt(halfX * halfX + halfY * halfY + halfZ * halfZ);

    // Proj zoom: Shrink ortho inversely (zoom in = smaller bounds = larger appearance)
    float padding = 1.05f;
    float viewHalf = (halfDiag / projZoom) * padding;  // No unscaledExtent needed!

    // Set ortho XY based on aspect
    float orthoHalfX, orthoHalfY;
    if (aspect >= 1.0f) {
        orthoHalfY = viewHalf;
        orthoHalfX = orthoHalfY * aspect;
    } else {
        orthoHalfX = viewHalf;
        orthoHalfY = orthoHalfX / aspect;
    }

    // Z bounds: Fixed to halfDiag (covers max proj Z after rotation), small buffer
    float zHalf = halfDiag;
    float zBuffer = 1.1f;  // Tight but safe now
    glm::mat4 Proj = glm::ortho(-orthoHalfX, orthoHalfX, -orthoHalfY, orthoHalfY,
                                -zHalf * zBuffer, zHalf * zBuffer);

    // MVP (unchanged)
    glm::mat4 MVP = Proj * T * M;
    shader.setMat4Uniform("uMVP", MVP);
    lgeWindow.render(shader, lineBuffer, vertexCount);
    lgeWindow.swapBuffers();
}
    return EXIT_SUCCESS;
}
