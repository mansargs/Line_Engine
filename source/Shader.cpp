#include "Shader.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace lge
{
	const char *Shader::readShaderFile(const char *path)
	{
		std::ifstream		shaderFile;
		std::stringstream	shaderStream;

		shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		shaderFile.open(path);
		shaderStream << shaderFile.rdbuf();
		shaderFile.close();
		return shaderStream.str().c_str();
	}

	void	Shader::checkShaderError(int success, unsigned int program, InfoLogFunc logInfo)
	{
		char info[MAX_INFO_LOG];

		if (!success)
		{
			logInfo(program, MAX_INFO_LOG, NULL, info);
			throw std::runtime_error(info);
		}
	}

	unsigned int	Shader::createCompileShader(const char *code, GLenum type)
	{
		int success;
		unsigned int shader;

		shader = glCreateShader(type);
		glShaderSource(shader, 1, &code, NULL);
		glCompileShader(shader);
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		checkShaderError(success, shader, glGetShaderInfoLog);
		return shader;
	}

	void	Shader::createShaderProgram(const char *vertexCode, const char *fragmentCode)
	{
		unsigned int vertex, fragment;
		int success;

		vertex = createCompileShader(vertexCode, GL_VERTEX_SHADER);
		fragment = createCompileShader(fragmentCode, GL_FRAGMENT_SHADER);
		programId = glCreateProgram();
		glAttachShader(programId, vertex);
		glAttachShader(programId, fragment);
		glLinkProgram(programId);
		glGetProgramiv(programId, GL_LINK_STATUS, &success);
		checkShaderError(success, programId, glGetProgramInfoLog);
	}

	Shader::Shader(const char *vertexPath, const char *fragmentPath)
	{
		const GLchar *vertexCode = readShaderFile(vertexPath),
					 *fragmentCode = readShaderFile(fragmentCode);

		createShaderProgram(vertexCode, fragmentCode);
	}

	Shader::~Shader()
	{
		if (programId != 0)
			glDeleteProgram(programId);
	}

	unsigned int Shader::getId() const{ return programId; }

	void	Shader::use() const { glUseProgram(programId); }

	void	Shader::setBoolUniform(const std::string &name, bool value) const
	{
		glUniform1i(glGetUniformLocation(programId, name.c_str()), (int) value);
	}

	void Shader::setFloatUniform(const std::string &name, float value) const
	{
		glUniform1f(glGetUniformLocation(programId, name.c_str()), value);
	}

	void Shader::setIntUniform(const std::string &name, int value) const
	{
		glUniform1i(glGetUniformLocation(programId, name.c_str()), value);
	}

	void Shader::setVec3Uniform(const std::string &name, const glm::vec3 &value) const
	{
		glUniform3fv(glGetUniformLocation(programId, name.c_str()), 1, &value[0]);
	}

	void Shader::setVec4Uniform(const std::string &name, const glm::vec4 &value) const
	{
		glUniform4fv(glGetUniformLocation(programId, name.c_str()), 1, &value[0]);
	}

	void Shader::setMat4Uniform(const std::string &name, const glm::mat4 &value) const
	{
		glUniformMatrix4fv(glGetUniformLocation(programId, name.c_str()), 1, GL_FALSE, &value[0][0]);
	}
}
