#include "../hdrs/Buffer.hpp"
#include <GL/glew.h>
#include <iostream>
#include <stdexcept>
#include <string>

namespace lge {
	void Buffer::bind() const {
		if (is_initialized) {
			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
		}
	}

	void Buffer::unbind() const {
		if (is_initialized) {
			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}

	std::string Buffer::checkError(const std::string& context) {
		std::string first_error;
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			std::string err_str = context + ": OpenGL error ";
			switch (err) {
				case GL_INVALID_ENUM:        err_str += "(GL_INVALID_ENUM)"; break;
				case GL_INVALID_VALUE:       err_str += "(GL_INVALID_VALUE)"; break;
				case GL_INVALID_OPERATION:   err_str += "(GL_INVALID_OPERATION)"; break;
				case GL_OUT_OF_MEMORY:       err_str += "(GL_OUT_OF_MEMORY)"; break;
				default:                     err_str += "(Unknown)"; break;
			}
			std::cerr << err_str << std::endl;
			if (first_error.empty()) {
				first_error = err_str;
			}
		}
		return first_error;
	}

	void Buffer::cleanupPartial() {
		unbind();
		if (vao != 0) {
			glDeleteVertexArrays(1, &vao);
			vao = 0;
		}
		if (vbo != 0) {
			glDeleteBuffers(1, &vbo);
			vbo = 0;
		}
		is_initialized = false;
	}

	void Buffer::setVertexAttr() {
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float),
							  (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		std::string err = checkError("Vertex Attributes Setup");
		if (!err.empty()) {
			cleanupPartial();
			throw std::runtime_error(err);
		}
	}

	Buffer::Buffer(const void* vertex_data, GLsizei size, GLenum usage)
			: data_size(size) {
		if (size <= 0 || !vertex_data)
			throw std::invalid_argument("Invalid vertex_data or size");

		glGenVertexArrays(1, &vao);
		std::string err = checkError("Gen VAO");
		if (!err.empty()) {
			cleanupPartial();
			throw std::runtime_error(err);
		}

		glBindVertexArray(vao);
		err = checkError("Bind VAO");
		if (!err.empty()) {
			cleanupPartial();
			throw std::runtime_error(err);
		}

		glGenBuffers(1, &vbo);
		err = checkError("Gen VBO");
		if (!err.empty()) {
			cleanupPartial();
			throw std::runtime_error(err);
		}

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		err = checkError("Bind VBO");
		if (!err.empty()) {
			cleanupPartial();
			throw std::runtime_error(err);
		}

		glBufferData(GL_ARRAY_BUFFER, data_size, vertex_data, usage);
		err = checkError("BufferData");
		if (!err.empty()) {
			cleanupPartial();
			throw std::runtime_error(err);
		}

		is_initialized = true;
		setVertexAttr();
		unbind();
		err = checkError("Unbind in ctor");
		if (!err.empty())
			std::cerr << "Warning: " << err << std::endl;
	}

	Buffer::~Buffer() {
		if (is_initialized) {
			unbind();
			glDeleteVertexArrays(1, &vao);
			glDeleteBuffers(1, &vbo);
			is_initialized = false;
		}
	}
}  // namespace lge
