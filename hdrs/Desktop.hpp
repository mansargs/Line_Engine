#pragma once
#include <SDL2/SDL.h>

namespace lge {
constexpr size_t DEFAULT_WIDTH = 800;
constexpr size_t DEFAULT_HEIGHT = 600;
constexpr size_t WINDOW_MARGIN_VERTICAL = 150;
constexpr size_t WINDOW_MARGIN_HORIZONTAL = 200;

class Desktop {
private:
    size_t windowWidth{DEFAULT_WIDTH};
    size_t windowHeight{DEFAULT_HEIGHT};
    bool running{false};
    SDL_Window* window{nullptr};
    SDL_GLContext glContext{nullptr};

    void checkSdlError(const std::string& context);

    Desktop();
    ~Desktop();  // Non-virtual: no inheritance

    Desktop(const Desktop&) = delete;
    Desktop& operator=(const Desktop&) = delete;
    Desktop(Desktop&&) = delete;
    Desktop& operator=(Desktop&&) = delete;

public:
    static Desktop& getInstance();

    size_t getWindowHeight() const { return windowHeight; }  // Const getter
    size_t getWindowWidth() const { return windowWidth; }
    void setWindowHeight(unsigned int height) { windowHeight = height; }
    void setWindowWidth(unsigned int width) { windowWidth = width; }
    void initWindowParam();

    // Render loop methods
    void pollEvents();
    void render();  // Stub: clear + user draw
    bool isRunning() const { return running; }
    void startLoop() { running = true; }
};
}  // namespace lge
