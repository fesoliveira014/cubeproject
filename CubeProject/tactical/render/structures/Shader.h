#ifndef _SHADER_H_
#define _SHADER_H_

#include "../../Common.h"
#include "../../utils/Utils.h"

#define SHADER_ATTRIBUTE_VERTEX 0
#define SHADER_ATTRIBUTE_NORMAL 1
#define SHADER_ATTRIBUTE_UV     2
#define SHADER_ATTRIBUTE_COLOUR 3

namespace tactical
{
	namespace render
	{
		class Shader
		{
		public:
			Shader(const char* vertexShaderPath, const char* fragShaderPath,
				const char* geomShaderPath);
			~Shader();

			bool LoadShaders(const char* vertexShaderPath, const char* fragShaderPath, 
							 const char* geomShaderPath);
			bool CreateAndLinkProgram();

			void Enable();
			void Disable();
			bool IsEnabled();

			bool AddAttribute(const std::string& attribute);
			bool AddUniform(const std::string& uniform);

			GLuint GetProgram();
			GLuint GetAttributeLocation(std::string attribute);
			GLuint GetUniformLocation(std::string uniform);

			void SetUniformBool(std::string name, bool value);
			void SetUniform1i(std::string name, int value);
			void SetUniform1f(std::string name, float value);
			void SetUniform2fv(std::string name, const glm::vec2 &vec2);
			void SetUniform3fv(std::string name, const glm::vec3 &vec3);
			void SetUniform4fv(std::string name, const glm::vec4 &vec4);
			void SetUniformMat2fv(std::string name, const glm::mat2 &mat2);
			void SetUniformMat3fv(std::string name, const glm::mat3 &mat3);
			void SetUniformMat4fv(std::string name, const glm::mat4 &mat4);

			bool IsShaderOK() { return m_shaderOK; }

		protected:
			bool LoadShaderFromFile(GLenum shaderType, const char* filename);
			bool Compile(const char* shaderSource, GLenum shaderType);
			void ParseUniforms();
			void DeleteShaderProgram();

			GLuint m_vertexShader;
			GLuint m_fragmentShader;
			GLuint m_geometryShader;

			GLuint m_program;

			std::map<std::string, GLuint> m_attributeList;
			std::map<std::string, GLuint> m_uniformList;

			bool m_shaderOK, m_isEnabled;

			std::vector<std::string> m_sources;
		};
	}
}

#endif