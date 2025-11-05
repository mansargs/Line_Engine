#include "../hdrs/Desktop.hpp"
#include <GL/glew.h>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace lge {
size_t Desktop::getWindowWidth() const { return windowWidth; }
size_t Desktop::getWindowHeight() const { return windowHeight; }
void Desktop::setWindowWidth(unsigned int width) { windowWidth = width; }
void Desktop::setWindowHeight(unsigned int height) { windowHeight = height; }

void Desktop::initWindowParam() {
    SDL_DisplayMode dm;
    if (SDL_GetCurrentDisplayMode(0, &dm) != 0) return;
    windowWidth = dm.w - WINDOW_MARGIN_HORIZONTAL;
    windowHeight = dm.h - WINDOW_MARGIN_VERTICAL;
}

void Desktop::checkSdlError(const std::string& context) {
    const char* err = SDL_GetError();
    if (*err != '\0') {
        std::string msg = context + ": SDL error - " + err;
        SDL_ClearError();
        throw std::runtime_error(msg);
    }
}

Desktop::Desktop()
    : windowWidth(DEFAULT_WIDTH), windowHeight(DEFAULT_HEIGHT), running(false) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
        throw std::runtime_error("SDL_Init failed: " + std::string(SDL_GetError()));

    initWindowParam();

    window = SDL_CreateWindow("Line Engine",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              static_cast<int>(windowWidth),
                              static_cast<int>(windowHeight),
                              SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    checkSdlError("Create Window");

    constexpr int GL_MAJOR = 3;
    constexpr int GL_MINOR = 3;
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GL_MAJOR);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, GL_MINOR);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    glContext = SDL_GL_CreateContext(window);
    checkSdlError("Create GL Context");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
        throw std::runtime_error("Failed to initialize OpenGL functionality");

    glViewport(0, 0, static_cast<GLsizei>(windowWidth), static_cast<GLsizei>(windowHeight));
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    if (SDL_GL_SetSwapInterval(1) < 0)
        std::cerr << "Warning: Unable to set VSync: " << SDL_GetError() << std::endl;
}

Desktop::~Desktop() {
    if (glContext) SDL_GL_DeleteContext(glContext);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

Desktop& Desktop::getInstance() {
    static Desktop instance;
    return instance;
}

void Desktop::pollEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) running = false;
        if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED) {
            windowWidth = e.window.data1;
            windowHeight = e.window.data2;
            glViewport(0, 0, static_cast<GLsizei>(windowWidth),
                       static_cast<GLsizei>(windowHeight));
        }
    }
}

void Desktop::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // TODO: User draw calls (e.g., buffer.bind(); glDrawArrays(...))
}

}  // namespace lge
