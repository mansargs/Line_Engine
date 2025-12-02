#include "../hdrs/Window.hpp"
#include "../hdrs/Shader.hpp"
#include "../hdrs/Buffer.hpp"
#include <GL/glew.h>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

namespace lge {
	size_t Window::getWindowHeight() const { return windowHeight; }

	size_t Window::getWindowWidth() const { return windowWidth; }

	void Window::setWindowHeight(unsigned int height) { windowHeight = height; }

	void Window::setWindowWidth(unsigned int width) { windowWidth = width; }

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
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
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

	void Window::resetView(bool iso) {
		if (iso) {
			Config.setRotateX(35.0f);
			Config.setRotateY(45.0f);
		} else {
			Config.setRotateX(0.0f);
			Config.setRotateY(0.0f);
		}
		Config.setRotateZ(0.0f);
		Config.setOffsetX(0.0f);
		Config.setOffsetY(0.0f);
		Config.setScaleFactor(1.0f);
		Config.setTopView(!iso);
		Config.setIsometric(iso);
		Config.setDragging(false);
		Config.setMouseLastX(0);
		Config.setMouseLastY(0);
	}

	void Window::keydownHandler(const SDL_Keysym &keyEvent) {
		if (keyEvent.sym == SDLK_ESCAPE)
			running = false;
		else if (keyEvent.sym == SDLK_i)
			resetView(true);
		else if (keyEvent.sym == SDLK_t)
			resetView(false);
		else if (keyEvent.sym == SDLK_UP || keyEvent.sym == SDLK_w)
			Config.setOffsetY(Config.getOffsetY() - 10.0f);
		else if (keyEvent.sym == SDLK_DOWN || keyEvent.sym == SDLK_s)
			Config.setOffsetY(Config.getOffsetY() + 10.0f);
		else if (keyEvent.sym == SDLK_LEFT || keyEvent.sym == SDLK_a)
			Config.setOffsetX(Config.getOffsetX() - 10.0f);
		else if (keyEvent.sym == SDLK_RIGHT || keyEvent.sym == SDLK_d)
			Config.setOffsetX(Config.getOffsetX() + 10.0f);
		else if (keyEvent.sym == SDLK_x && Config.getIsometric())
			Config.setRotateX(Config.getRotateX() + 5.0f);
		else if (keyEvent.sym == SDLK_y && Config.getIsometric())
			Config.setRotateY(Config.getRotateY() + 5.0f);
		else if (keyEvent.sym == SDLK_z && Config.getIsometric())
			Config.setRotateZ(Config.getRotateZ() + 5.0f);
		else if (keyEvent.sym == SDLK_j && Config.getIsometric())
			Config.setRotateX(Config.getRotateX() - 5.0f);
		else if (keyEvent.sym == SDLK_k && Config.getIsometric())
			Config.setRotateY(Config.getRotateY() - 5.0f);
		else if (keyEvent.sym == SDLK_l && Config.getIsometric())
			Config.setRotateZ(Config.getRotateZ() - 5.0f);
		else if (keyEvent.sym == SDLK_EQUALS || keyEvent.sym == SDLK_PLUS)
			Config.setScaleFactor(Config.getScaleFactor() + 0.1f);
		else if (keyEvent.sym == SDLK_MINUS)
			Config.setScaleFactor(std::max(0.5f, Config.getScaleFactor() - 0.1f));
	}

	void Window::pollEvents() {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT)
				running = false;
			else if (e.type == SDL_KEYDOWN) {
				keydownHandler(e.key.keysym);
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

	void Window::setupRendering() {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glLineWidth(Config.getLineWidth());
		running = true;
	}

	glm::mat4 Window::Rotation() {
		glm::mat4 R = glm::mat4(1.0f);
		R *= glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1, 0, 0));
		if (Config.getRotateY() != 0.0f)
			R *= glm::rotate(glm::mat4(1.0f), glm::radians(Config.getRotateY()), glm::vec3(0, 1, 0));
		if (Config.getRotateX() != 0.0f)
			R *= glm::rotate(glm::mat4(1.0f), glm::radians(Config.getRotateX()), glm::vec3(1, 0, 0));
		if (Config.getRotateZ() != 0.0f)
			R *= glm::rotate(glm::mat4(1.0f), glm::radians(Config.getRotateZ()), glm::vec3(0, 0, 1));
		return R;
	}

	glm::mat4 Window::Projection(float projZoom, const Map &lgeMap) {
		float padding = 1.05f;
		float aspect = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
		float halfX =  lgeMap.getMapWidth() / 2.0f;
		float halfY = lgeMap.getMapHeight() / 2.0f;
		float halfZ = Config.getZScale() * ((lgeMap.getMaxZ() - lgeMap.getMinZ()) / 2.0f);
		float halfDiag = std::sqrt(halfX * halfX + halfY * halfY + halfZ * halfZ);
		float viewHalf = (halfDiag / projZoom) * padding;
		float orthoHalfX, orthoHalfY;
		if (aspect >= 1.0f) {
			orthoHalfY = viewHalf;
			orthoHalfX = orthoHalfY * aspect;
		} else {
			orthoHalfX = viewHalf;
			orthoHalfY = orthoHalfX / aspect;
		}
		float zHalf = halfDiag;
		float zBuffer = 1.1f;
		glm::mat4 Proj = glm::ortho(-orthoHalfX, orthoHalfX, -orthoHalfY, orthoHalfY,
									-zHalf * zBuffer, zHalf * zBuffer);
		return Proj;
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
