#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

namespace lge
{
	typedef void (*InfoLogFunc)(GLuint, GLsizei, GLsizei*, GLchar*);
	constexpr unsigned int MAX_INFO_LOG  = 512;

	class Shader
	{
		private:
			unsigned int	programId {0};
		private:
			static void			checkShaderError(int success, unsigned int program, InfoLogFunc logInfo);
			static const char	*readShaderFile(const char *path);
			static unsigned int	createCompileShader(const char *code, GLenum type);
			void				createShaderProgram(const char *vertexCode, const char *fragmentCode);
		public:
			Shader(const char *vertexPath, const char *fragmentPath);
			~Shader();

			unsigned int	getId() const;
			void			use() const;
			void			setBoolUniform(const std::string &name, bool value) const ;
			void			setIntUniform(const std::string &name, int value) const;
			void			setFloatUniform(const std::string &name, float value) const;
			void			setVec3Uniform(const std::string &name, const glm::vec3 &value) const;
			void			setVec4Uniform(const std::string &name, const glm::vec4 &value) const;
			void			setMat4Uniform(const std::string &name, const glm::mat4 &value) const;
	};
}
