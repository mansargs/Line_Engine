#include "../hdrs/Lge.hpp"
#include <fstream>
#include <iostream>
#include <cctype>
#include <sstream>
#include <iomanip>

size_t	Lge::getWidth() const { return width;}

size_t	Lge::getHeight() const { return height;}

Lge::Lge() : width(0), height(0), VAO(0), VBO(0), shader(nullptr), vertexCount(0) {}

Lge::~Lge()
{
	if (VAO != 0)
		glDeleteVertexArrays(1, &VAO);
	if (VBO != 0)
		glDeleteBuffers(1, &VBO);
	if (shader != nullptr)
		delete shader;
}

// void	Lge::printMap(const gridContent &mapContent) const
// {
// 	for (const auto &row : mapContent)
// 	{
// 		for (const auto &cell : row)
// 		{
// 			std::cout << std::setw(2) << cell << " ";
// 		}
// 		std::cout << std::endl;
// 	}
// 	std::cout << "Map Width: " << width << ", Height: " << height << std::endl;
// }


