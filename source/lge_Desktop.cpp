#include "../hdrs/Desktop.hpp"
#include <stdexcept>
#include <iostream>
#include <vector>
#include <algorithm>
#include <GL/glew.h>
#include "../hdrs/Lge.hpp"

unsigned int lgeDesktop::getDesktopHeight() { return desktopHeight;}

unsigned int lgeDesktop::getDesktopWidth() { return desktopWidth;}

void lgeDesktop::setDesktopHeight(unsigned int height) { desktopHeight = height;}

void lgeDesktop::setDesktopWidth(unsigned int width) { desktopWidth = width;}

void lgeDesktop::setDesktopData()
{
	SDL_DisplayMode dm;
	if (SDL_GetCurrentDisplayMode(0, &dm) != 0)
		return;
	desktopWidth = dm.w - 200;
	desktopHeight = dm.h - 150;
};
lgeDesktop::lgeDesktop() : desktopWidth(1000), desktopHeight(800), running(false)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
		throw std::runtime_error(SDL_GetError());
	setDesktopData();
	window = SDL_CreateWindow("Line Engine",
						  SDL_WINDOWPOS_CENTERED,
						  SDL_WINDOWPOS_CENTERED,
						  desktopWidth,
						  desktopHeight,
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
	glViewport(0, 0, desktopWidth, desktopHeight);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	if (SDL_GL_SetSwapInterval(1) < 0)
		std::cerr << "Warning: Unable to set VSync: " << SDL_GetError() << std::endl;
}

lgeDesktop::~lgeDesktop()
{
	if (glContext)
		SDL_GL_DeleteContext(glContext);
	if (window)
		SDL_DestroyWindow(window);
	SDL_Quit();
}

void addVertex(std::vector<float> &vertex, const Pixel &data)
{
	vertex.insert(vertex.end(), {
		data.position.x,
		data.position.y,
		data.position.z,
		data.color.r / 255,
		data.color.g / 255,
		data.color.b / 255,
		data.color.a / 255
		}
	);
}

void	Lge::vertexes1D()
{
	std::vector<float> vertex;

	for (size_t i = 0; i < height; ++i)
	{
		for (size_t j = 0; j < width - 1; ++j)
		{
			addVertex(vertex, mapPixels[i][j]);
			addVertex(vertex, mapPixels[i][j + 1]);
		}
	}

	for (size_t i = 0; i < height - 1; ++i)
	{
		for (size_t j = 0; j < width; ++j)
		{
			addVertex(vertex, mapPixels[i][j]);
			addVertex(vertex, mapPixels[i + 1][j]);
		}
	}

	// Store vertex count
	vertexCount = vertex.size() / 7; // 7 floats per vertex (3 pos + 4 color)

	// Create VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Create VBO (Vertex Buffer Object)
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(float), vertex.data(), GL_STATIC_DRAW);

    // Position attribute (3 floats)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute (4 floats)
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind VAO
    glBindVertexArray(0);
}
