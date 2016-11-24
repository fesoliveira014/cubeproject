#include "Shader.h"

namespace tactical
{
	namespace render
	{
		Shader::Shader(const char* vertexShaderPath, const char* fragShaderPath,
			const char* geomShaderPath)
		{
			m_vertexShader = 0;
			m_fragmentShader = 0;
			m_geometryShader = 0;

			m_program = 0;

			m_attributeList.clear();
			m_uniformList.clear();

			m_shaderOK = LoadShaders(vertexShaderPath, fragShaderPath, geomShaderPath);
		}

		Shader::~Shader()
		{
			DeleteShaderProgram();
			m_program = 0;

			m_attributeList.clear();
			m_uniformList.clear();
		}

		bool Shader::LoadShaders(const char* vertexShaderPath, const char* fragShaderPath,
			const char* geomShaderPath)
		{
			bool resultVertex = true;
			bool resultFrag = true;
			bool resultGeom = true;
			bool resultLink = true;

			if (vertexShaderPath != NULL) resultVertex = LoadShaderFromFile(GL_VERTEX_SHADER, vertexShaderPath);
			if (fragShaderPath != NULL) resultFrag = LoadShaderFromFile(GL_FRAGMENT_SHADER, fragShaderPath);
			if (geomShaderPath != NULL) resultGeom = LoadShaderFromFile(GL_GEOMETRY_SHADER, geomShaderPath);

			resultLink = CreateAndLinkProgram();

			ParseUniforms();

			m_sources.clear();

			return resultVertex && resultFrag && resultGeom && resultLink;
		}

		bool Shader::LoadShaderFromFile(GLenum shaderType, const char* filename)
		{
			bool result;
			std::string shaderSource;

			result = utils::read_file(filename, shaderSource);
			if (!result) {
				LOG << LOGTYPE::LOG_ERROR << "Failed to read file " + std::string(filename) + ".";
				return false;
			}

			result = Compile(shaderSource.c_str(), shaderType);
			if (!result) {
				LOG << LOGTYPE::LOG_ERROR << "Failed to compile shader file " + std::string(filename) + ".";
				return false;
			}

			m_sources.push_back(shaderSource);

			//LOG << LOGTYPE::LOG_INFO << "Shader " + std::string(filename) + " successfully initialized.";
			return true;
		}

		bool Shader::CreateAndLinkProgram()
		{
			m_program = glCreateProgram();
			if (m_program == 0) {
				LOG << LOGTYPE::LOG_ERROR << "Shader program could not be created.";
				return false;
			}

			if (m_vertexShader != 0) glAttachShader(m_program, m_vertexShader);
			if (m_fragmentShader != 0) glAttachShader(m_program, m_fragmentShader);
			if (m_geometryShader != 0) glAttachShader(m_program, m_geometryShader);

			GLint success = 0;

			glLinkProgram(m_program);
			glGetProgramiv(m_program, GL_LINK_STATUS, &success);

			if (!success) {
				GLchar errorLog[1024] = { 0 };
				glGetProgramInfoLog(m_program, sizeof(errorLog), NULL, errorLog);
				LOG << LOGTYPE::LOG_ERROR << "Invalid shader program: " + std::string(errorLog);
				return false;
			}

			glDeleteShader(m_vertexShader);
			glDeleteShader(m_fragmentShader);
			glDeleteShader(m_geometryShader);

			return true;
		}

		bool Shader::Compile(const char* shaderSource, GLenum shaderType)
		{
			GLuint shaderObject = glCreateShader(shaderType);
			if (shaderObject == 0) {
				LOG << LOGTYPE::LOG_ERROR << "Error creating shader of type " + std::to_string(shaderType);
				return false;
			}

			const GLchar* source[1] = { shaderSource };
			GLint length[1] = { (GLint)strlen(shaderSource) };

			glShaderSource(shaderObject, 1, source, length);
			glCompileShader(shaderObject);

			GLint success;
			glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &success);
			if (!success) {
				GLchar errorLog[1024];
				glGetShaderInfoLog(shaderObject, sizeof(errorLog), NULL, errorLog);
				LOG << LOGTYPE::LOG_ERROR << "Error compiling shader of type " + std::to_string(shaderType);
				LOG << LOGTYPE::LOG_ERROR << "Error log: " << errorLog;
				return false;
			}

			if (shaderType == GL_VERTEX_SHADER)
				m_vertexShader = shaderObject;
			if (shaderType == GL_FRAGMENT_SHADER)
				m_fragmentShader= shaderObject;
			if (shaderType == GL_GEOMETRY_SHADER)
				m_geometryShader= shaderObject;

			return true;
		}

		void Shader::ParseUniforms()
		{
			bool readingStruct = false;
			std::string currentStruct = "", uniform = "";


			for (std::string source : m_sources) {
				std::istringstream sourceStream(source);
				for (std::string line; std::getline(sourceStream, line);) {
					int index = line.find_first_not_of(" \t");
					if (index >= 0)
						line = line.substr(index, line.size() - index);

					std::vector<std::string> tokens = utils::split(line, ' ');
					if (!tokens.empty()) {
						if (readingStruct) {
							if (!tokens[0].compare("//"))
								continue;
							if (!tokens[0].compare("};")) {
								readingStruct = false;
								currentStruct = "";
							}
							else if (tokens[0].compare("")) {
								if (tokens[1].back() == ';')
									tokens[1].pop_back();
								m_structs[currentStruct].push_back(tokens[1]);
							}
						}
						else if (!tokens[0].compare(std::string("uniform"))) {
							uniform = tokens[2];
							if (uniform.back() == ';')
								uniform.pop_back();

							if (m_structs.find(tokens[1]) != m_structs.end()) {
								LOG_INFO("Adding uniform attributes for struct " + tokens[1]);
								for (auto attribute : m_structs[tokens[1]]) {
									LOG_INFO("	Attribute: " + attribute);
									AddUniform(uniform + "." + attribute);
								}
							}
							else 
								AddUniform(uniform);
						}
						else if (!tokens[0].compare("struct")) {
							m_structs[tokens[1]] = std::vector<std::string>();
							readingStruct = true;
							currentStruct = tokens[1];
						}
					}
				}
			}
		}

		void Shader::Enable()
		{
			glUseProgram(m_program);
			m_isEnabled = true;
		}

		void Shader::Disable()
		{
			glUseProgram(0);
			m_isEnabled = false;
		}

		bool Shader::IsEnabled()
		{
			return m_isEnabled;
		}

		bool Shader::AddAttribute(const std::string& attribute)
		{
			GLuint location = glGetAttribLocation(m_program, attribute.c_str());

			if (location == 0xffffffff) {
				LOG << LOGTYPE::LOG_ERROR << "Attribute '" + attribute + "' not found in the program.";
				return false;
			}

			m_attributeList[attribute] = location;
			return true;
		}

		bool Shader::AddUniform(const std::string& uniform)
		{
			GLuint location = glGetUniformLocation(m_program, uniform.c_str());

			if (location == 0xffffffff) {
				LOG << LOGTYPE::LOG_ERROR << "Uniform '" + uniform + "' not found in the program.";
				return false;
			}

			m_uniformList[uniform] = location;
			return true;
		}

		GLuint Shader::GetProgram()
		{
			return m_program;
		}

		GLuint Shader::GetAttributeLocation(std::string attribute)
		{
			if (m_attributeList.find(attribute) == m_attributeList.end()) {
				LOG << LOGTYPE::LOG_ERROR << "There is no attribute '" + attribute + "' in the shader program.";
				return 0;
			}

			return m_attributeList[attribute];
		}

		GLuint Shader::GetUniformLocation(std::string uniform)
		{
			if (m_uniformList.find(uniform) == m_uniformList.end()) {
				LOG << LOGTYPE::LOG_ERROR << "There is no uniform '" + uniform + "' in the shader program.";
				return 0;
			}

			return m_uniformList[uniform];
		}

		void Shader::SetUniformBool(std::string name, bool value)
		{
			glUniform1i(m_uniformList[name], static_cast<GLint>(value));
		}

		void Shader::SetUniform1i(std::string name, int value)
		{
			glUniform1i(m_uniformList[name], value);
		}

		void Shader::SetUniform1f(std::string name, float value)
		{
			glUniform1f(m_uniformList[name], value);
		}

		void Shader::SetUniform2fv(std::string name, const glm::vec2 &vec2)
		{
			glUniform2f(m_uniformList[name], vec2.x, vec2.y);
		}

		void Shader::SetUniform3fv(std::string name, const glm::vec3 &vec3)
		{
			glUniform3f(m_uniformList[name], vec3.x, vec3.y, vec3.z);
		}

		void Shader::SetUniform4fv(std::string name, const glm::vec4 &vec4)
		{
			glUniform4f(m_uniformList[name], vec4.x, vec4.y, vec4.z, vec4.w);
		}

		void Shader::SetUniformMat2fv(std::string name, const glm::mat2 &mat2)
		{
			glUniformMatrix2fv(m_uniformList[name], 1, GL_FALSE, glm::value_ptr(mat2));
		}

		void Shader::SetUniformMat3fv(std::string name, const glm::mat3 &mat3)
		{
			glUniformMatrix3fv(m_uniformList[name], 1, GL_FALSE, glm::value_ptr(mat3));
		}

		void Shader::SetUniformMat4fv(std::string name, const glm::mat4 &mat4)
		{
			glUniformMatrix4fv(m_uniformList[name], 1, GL_FALSE, glm::value_ptr(mat4));
		}

		void Shader::DeleteShaderProgram()
		{
			glDeleteShader(m_program);
		}
	}
}