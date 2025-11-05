#pragma once

#include <GL/glew.h>
#include <string>

namespace lge {
class Buffer {
private:
  GLuint vbo{0};
  GLuint vao{0};
  GLsizei data_size{0};
  bool is_initialized{false};

public:
  Buffer(const void *vertex_data, GLsizei size, GLenum usage = GL_STATIC_DRAW);
  ~Buffer();

  [[nodiscard]] std::string checkError(const std::string &context = "");
  void bind() const;
  void unbind() const;
  Buffer(const Buffer &) = delete;
  Buffer(Buffer &&) = delete;
  Buffer &operator=(const Buffer &) = delete;
  Buffer &operator=(const Buffer &&) = delete;

private:
  void cleanupPartial();
  void setVertexAttr();
};
} // namespace lge
