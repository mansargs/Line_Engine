#include "../hdrs/Desktop.hpp"
#include <stdexcept>
#include <iostream>
#include <vector>
#include <algorithm>
#include <GL/glew.h>
#include "../hdrs/Map.hpp"

size_t lge::Desktop::getWindowHeight() { return windowHeight;}

size_t lge::Desktop::getWindowWidth() { return windowWidth;}

void lge::Desktop::setWindowHeight(unsigned int height) { windowHeight = height;}

void lge::Desktop::setWindowWidth(unsigned int width) {windowWidth = width;}

void lge::Desktop::initWindowParam()
{
	SDL_DisplayMode dm;
	if (SDL_GetCurrentDisplayMode(0, &dm) != 0)
		return;
	windowWidth = dm.w - 200;
	windowHeight = dm.h - 150;
};
lge::Desktop::Desktop() : windowWidth(DEFAULT_WIDTH), windowHeight(DEFAULT_HEIGHT), running(false)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
		throw std::runtime_error(SDL_GetError());
	initWindowParam();
	window = SDL_CreateWindow("Line Engine",
						  SDL_WINDOWPOS_CENTERED,
						  SDL_WINDOWPOS_CENTERED,
						  windowWidth,
						  windowHeight,
						  SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	if (!window)
		throw std::runtime_error(SDL_GetError());
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	glContext = SDL_GL_CreateContext(window);
	if (!glContext)
		throw std::runtime_error(std::string("Failed to create OpenGL context: ") + SDL_GetError());
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
		throw std::runtime_error("Failed to initialize OpenGL functional");
	glViewport(0, 0, windowWidth, windowHeight);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	if (SDL_GL_SetSwapInterval(1) < 0)
		std::cerr << "Warning: Unable to set VSync: " << SDL_GetError() << std::endl;
}

lge::Desktop::~Desktop()
{
	if (glContext)
		SDL_GL_DeleteContext(glContext);
	if (window)
		SDL_DestroyWindow(window);
	SDL_Quit();
}

// void addVertex(std::vector<float> &vertex, const Pixel &data)
// {
// 	vertex.insert(vertex.end(), {
// 		data.position.x,
// 		data.position.y,
// 		data.position.z,
// 		data.color.r / 255,
// 		data.color.g / 255,
// 		data.color.b / 255,
// 		data.color.a / 255
// 		}
// 	);
// }
