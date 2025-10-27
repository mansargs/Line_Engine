#pragma once

#include <GL/glew.h>
#include <string>

namespace lge {
	class GlBuffer {
	private:
		GLuint	vbo{0};
		GLuint	vao{0};
		GLsizei	data_size{0};
		bool	is_initialized{false};

	public:
		GlBuffer(const void* vertex_data, GLsizei size, GLenum usage = GL_STATIC_DRAW);
		~GlBuffer();

		[[nodiscard]] std::string checkError(const std::string& context = "");
		void bind() const;
		void unbind() const;
		GlBuffer(const GlBuffer&) = delete;
		GlBuffer(GlBuffer&&) = delete;
		GlBuffer& operator=(const GlBuffer&) = delete;
		GlBuffer& operator=(const GlBuffer&&) = delete;
	private:
		void cleanupPartial();
	};
}
