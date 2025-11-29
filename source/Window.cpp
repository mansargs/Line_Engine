#include "../hdrs/Window.hpp"
#include "../hdrs/Shader.hpp"
#include "../hdrs/Buffer.hpp"
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

	void Window::setWindowByDisplay() {
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

	Window::Window() {
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
			throw std::runtime_error("SDL_Init failed: " + std::string(SDL_GetError()));
		setWindowByDisplay();
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 5);
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
		glEnable(GL_MULTISAMPLE);
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

	void Window::computeScaleOffset(float mapW, float mapH, Window &lgeWindow, bool isometric) {
		float centerX, centerY, scaleX, scaleY;
		if (isometric) {
			centerX = (mapW + mapH) * 0.5f;
			centerY = (mapH * 0.25f);
			scaleX = (0.95f * lgeWindow.getWindowWidth()) / (mapW + mapH);
			scaleY = (0.95f * lgeWindow.getWindowHeight()) / (mapH * 0.5f);
		} else {
			centerX = mapW * 0.5f;
			centerY = mapH * 0.5f;
			scaleX = (0.95f * lgeWindow.getWindowWidth()) / mapW;
			scaleY = (0.95f * lgeWindow.getWindowHeight()) / mapH;
		}
		lgeWindow.Config.setScale(std::min(scaleX, scaleY));
		lgeWindow.Config.setOffsetX((lgeWindow.getWindowWidth() * 0.5f)
									- (lgeWindow.Config.getScale() * centerX));
		lgeWindow.Config.setOffsetY((lgeWindow.getWindowHeight() * 0.5f)
									- (lgeWindow.Config.getScale() * centerY));
	}


	void Window::pollEvents() {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT)
				running = false;
			else if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.sym == SDLK_ESCAPE)
					running = false;
				else if (e.key.keysym.sym == SDLK_i)
					Config.setIsometric(true);
				else if (e.key.keysym.sym == SDLK_t)
					Config.setTopView(true);
				else if (e.key.keysym.sym == SDLK_UP)
					Config.setOffsetY(Config.getOffsetY() - 10.0f);
				else if (e.key.keysym.sym == SDLK_DOWN)
					Config.setOffsetY(Config.getOffsetY() + 10.0f);
				else if (e.key.keysym.sym == SDLK_LEFT)
					Config.setOffsetX(Config.getOffsetX() - 10.0f);
				else if (e.key.keysym.sym == SDLK_RIGHT)
					Config.setOffsetX(Config.getOffsetX() + 10.0f);
				else if (e.key.keysym.sym == SDLK_x)
					Config.setRotateX(Config.getRotateX() + 5.0f);
				else if (e.key.keysym.sym == SDLK_y)
					Config.setRotateY(Config.getRotateY() + 5.0f);
				else if (e.key.keysym.sym == SDLK_z)
					Config.setRotateZ(Config.getRotateZ() + 5.0f);
				else if (e.key.keysym.sym == SDLK_j)
					Config.setRotateX(Config.getRotateX() - 5.0f);
				else if (e.key.keysym.sym == SDLK_k)
					Config.setRotateY(Config.getRotateY() - 5.0f);
				else if (e.key.keysym.sym == SDLK_l)
					Config.setRotateZ(Config.getRotateZ() - 5.0f);
				else if (e.key.keysym.sym == SDLK_EQUALS || e.key.keysym.sym == SDLK_PLUS)
					Config.setScaleFactor(Config.getScaleFactor() + 0.1f);
				else if (e.key.keysym.sym == SDLK_MINUS)
					Config.setScaleFactor(std::max(0.5f, Config.getScaleFactor() - 0.1f));
			}
			else if (e.type == SDL_MOUSEWHEEL) {
				if (e.wheel.y > 0)
					Config.setScaleFactor(Config.getScaleFactor() + 0.1f);
				else if (e.wheel.y < 0)
					Config.setScaleFactor(std::max(0.5f, Config.getScaleFactor() - 0.1f));
			}
			else if (e.type == SDL_MOUSEBUTTONDOWN) {
				if (e.button.button == SDL_BUTTON_LEFT) {
					Config.setDragging(true);
					Config.setMouseLastX(e.button.x);
					Config.setMouseLastY(e.button.y);
				}
			}
			else if (e.type == SDL_MOUSEBUTTONUP) {
				if (e.button.button == SDL_BUTTON_LEFT)
					Config.setDragging(false);
			}
			else if (e.type == SDL_MOUSEMOTION && Config.getDragging()) {
			int dx = e.motion.x - Config.getMouseLastX();
			int dy = e.motion.y - Config.getMouseLastY();
			Config.setOffsetX(Config.getOffsetX() + dx);
			Config.setOffsetY(Config.getOffsetY() + dy);
			Config.setMouseLastX(e.motion.x);
			Config.setMouseLastY(e.motion.y);
			}
		}
	}

	void Window::render(const Shader &shader, const Buffer &lineBuffer, GLsizei vertexCount) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		std::string err = Buffer::checkError("Post-Clear");
		if (!err.empty())
			std::cerr << "Clear Error: " << err << std::endl;
		shader.use();
		err = Buffer::checkError("Shader Use");
		if (!err.empty())
			std::cerr << "Shader Error: " << err << std::endl;
		lineBuffer.bind();
		err = lge::Buffer::checkError("Buffer Bind");
		if (!err.empty())
			std::cerr << "Bind Error: " << err << std::endl;
		glDrawArrays(GL_LINES, 0, vertexCount);
		err = lge::Buffer::checkError("DrawArrays");
		if (!err.empty())
			std::cerr << "Draw Error: " << err << std::endl;
		lineBuffer.unbind();
	}

}  // namespace lge
