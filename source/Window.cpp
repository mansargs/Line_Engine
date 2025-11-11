#include "../hdrs/Window.hpp"
#include <GL/glew.h>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace lge {

	size_t Window::getWindowHeight() const { return windowHeight; }

	size_t Window::getWindowWidth() const { return windowWidth; }

	void Window::setWindowHeight(unsigned int height) { windowHeight = height; }

	void Window::setWindowWidth(unsigned int width) { windowWidth = width; }

	void Window::startLoop() { running = true; }

	bool Window::isRunning() const { return running; }

	void Window::swapBuffers() {
		if (window)
			SDL_GL_SwapWindow(window);
	}

	void Window::initWindowParam() {
		SDL_DisplayMode dm;
		if (SDL_GetCurrentDisplayMode(0, &dm) != 0)
			return;
		windowHeight = windowWidth = std::min(dm.w, dm.h) * WINDOW_MARGIN;
	}

	void Window::checkSdlError(const std::string& context) {
		const char* err = SDL_GetError();
		if (*err != '\0') {
			std::string msg = context + ": SDL error - " + err;
			SDL_ClearError();
			throw std::runtime_error(msg);
		}
	}

	Window::Window() : windowWidth(DEFAULT_WIDTH), windowHeight(DEFAULT_HEIGHT), running(false) {
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
			throw std::runtime_error("SDL_Init failed: " + std::string(SDL_GetError()));
		initWindowParam();
		window = SDL_CreateWindow("Line Engine",
								  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
								  static_cast<int>(windowWidth),
								  static_cast<int>(windowHeight),
								  SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
		checkSdlError("Create Window");
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

	Window::~Window() {
		if (glContext)
			SDL_GL_DeleteContext(glContext);
		if (window)
			SDL_DestroyWindow(window);
		SDL_Quit();
	}

	Window& Window::getInstance() {
		static Window instance;
		return instance;
	}

	void Window::pollEvents() {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT)
				running = false;
		}
	}

	void Window::render() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// TODO: User draw calls (e.g., buffer.bind(); glDrawArrays(...))
	}

}  // namespace lge
