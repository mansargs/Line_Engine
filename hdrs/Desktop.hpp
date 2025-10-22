#pragma once

#include <SDL2/SDL.h>

namespace lge
{
	class Desktop
	{
		private:
			unsigned int desktopWidth;
			unsigned int desktopHeight;
			bool running;
			SDL_Window* window;
			SDL_GLContext glContext;

		public:
			lgeDesktop();
			~lgeDesktop();

			unsigned int getDesktopHeight();
			unsigned int getDesktopWidth();
			void setDesktopHeight(unsigned int height);
			void setDesktopWidth(unsigned int width);
			void setDesktopData();
	};
}
