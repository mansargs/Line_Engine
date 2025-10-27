#include "../hdrs/GlBuffer.hpp"
#include <GL/glew.h>
#include <string>
#include <stdexcept>
#include <iostream>

namespace lge {
	void GlBuffer::bind() const
	{
		if (is_initialized)
		{
			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
		}
	}

	void GlBuffer::unbind() const
	{
		if (is_initialized)
		{
			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}

	std::string GlBuffer::checkError(const std::string& context)
	{
		std::string full_context = context;
		std::string first_error = "";
		GLenum err;

		while ((err = glGetError()) != GL_NO_ERROR)
		{
			std::string err_str = full_context + ": OpenGL error ";
			switch (err)
			{
				case GL_INVALID_ENUM: err_str += " (GL_INVALID_ENUM)"; break;
				case GL_INVALID_VALUE: err_str += " (GL_INVALID_VALUE)"; break;
				case GL_INVALID_OPERATION: err_str += " (GL_INVALID_OPERATION)"; break;
				case GL_OUT_OF_MEMORY: err_str += " (GL_OUT_OF_MEMORY)"; break;
				default: err_str += " (Unknown)"; break;
			}
			std::cerr << err_str << std::endl;
			if (first_error.empty()) first_error = err_str;
		}
		return first_error;
	}

	void GlBuffer::cleanupPartial()
	{
		unbind();
		if (vao != 0) {
			glDeleteVertexArrays(1, &vao);
			vao = 0;
		}
		if (vbo != 0) {
			glDeleteBuffers(1, &vbo);
			vbo = 0;
		}
	}

	GlBuffer::GlBuffer(const void* vertex_data, GLsizei size, GLenum usage = GL_STATIC_DRAW) : data_size(size)
	{
		if (size <= 0 || !vertex_data)
			throw std::invalid_argument("Invalid vertex_data or size");
		glGenVertexArrays(1, &vao);
		std::string err = checkError("Gen VAO");
		if (!err.empty())
		{
			cleanupPartial();
			throw std::runtime_error(err);
		}
		glBindVertexArray(vao);
		err = checkError("Bind VAO");
		if (!err.empty())
		{
			cleanupPartial();
			throw std::runtime_error(err);
		}
		glGenBuffers(1, &vbo);
		err = checkError("Gen VBO");
		if (!err.empty())
		{
			cleanupPartial();
			throw std::runtime_error(err);
		}
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		err = checkError("Bind VBO");
		if (!err.empty())
		{
			cleanupPartial();
			throw std::runtime_error(err);
		}
		glBufferData(GL_ARRAY_BUFFER, data_size, vertex_data, usage);
		err = checkError("BufferData");
		if (!err.empty())
		{
			cleanupPartial();
			throw std::runtime_error(err);
		}
		is_initialized = true;
		unbind();
		err = checkError("Unbind in ctor");
		if (!err.empty())
			std::cerr << "Warning: " << err << std::endl;
	}

	GlBuffer::~GlBuffer()
	{
		if (is_initialized)
		{
			glDeleteVertexArrays(1, &vao);
			glDeleteBuffers(1, &vbo);
			is_initialized = false;
		}
	}
}
