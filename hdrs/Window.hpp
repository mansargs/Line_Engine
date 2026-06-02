#pragma once

#include <SDL2/SDL.h>
#include <string>
#include "../hdrs/Shader.hpp"
#include "../hdrs/Buffer.hpp"
#include "../hdrs/Window.hpp"
#include "../hdrs/Map.hpp"
#include "../hdrs/RenderConfig.hpp"

namespace lge {
	constexpr size_t DEFAULT_WIDTH = 700;
	constexpr size_t DEFAULT_HEIGHT = 700;
	constexpr float WINDOW_MARGIN = 0.85f;
	constexpr int GL_MAJOR = 3;
	constexpr int GL_MINOR = 3;

	class Window {
		private:
			size_t windowWidth{DEFAULT_WIDTH};
			size_t windowHeight{DEFAULT_HEIGHT};
			bool running{false};
			SDL_Window *window{nullptr};
			SDL_GLContext glContext{nullptr};

			void checkSdlError(const std::string& context);
			void keydownHandler(const SDL_Keysym &keyEvent);
			void resetView(bool iso);
			Window();
			Window(const Window&) = delete;
			Window& operator=(const Window&) = delete;
			Window(Window&&) = delete;
			Window& operator=(Window&&) = delete;
		public:
			RenderConfig Config;
			~Window();
			static Window& getInstance();
			size_t getWindowHeight() const;
			size_t getWindowWidth() const;
			void setWindowHeight(unsigned int height);
			void setWindowWidth(unsigned int width);
			void setWindowByDisplay();
			void pollEvents();
			void render(const Shader &shader, const Buffer &lineBuffer, GLsizei vertexCount);
			bool isRunning() const;
			void swapBuffers();
			void setupRendering();
			glm::mat4 Rotation();
			glm::mat4 Projection(float projZoom, const Map &lgeMap);
	};
}
