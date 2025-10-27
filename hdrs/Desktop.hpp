#pragma once

#include <SDL2/SDL.h>

namespace lge
{
	constexpr size_t DEFAULT_WIDTH = 1000;
	constexpr size_t DEFAULT_HEIGHT = 800;

	class Desktop
	{
		private:
			size_t			windowWidth{DEFAULT_WIDTH};
			size_t			windowHeight{DEFAULT_HEIGHT};
			bool			running{false};
			SDL_Window*		window{nullptr};
			SDL_GLContext	glContext{nullptr};

		public:
			Desktop();
			~Desktop();

			size_t	getWindowHeight();
			size_t	getWindowWidth();
			void	setWindowHeight(unsigned int height);
			void	setWindowWidth(unsigned int width);
			void	initWindowParam();
	};
}
