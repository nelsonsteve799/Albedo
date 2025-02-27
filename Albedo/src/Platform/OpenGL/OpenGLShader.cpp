#include "AlbedoPreCompiledHeader.h"

#include "OpenGLShader.h"
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include "Albedo/Core/Log.h"

namespace Albedo {

	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		if (type == "compute")
			return GL_COMPUTE_SHADER;
		if (type == "tess_control")
			return GL_TESS_CONTROL_SHADER;
		if (type == "tess_evaluation")
			return GL_TESS_EVALUATION_SHADER;
		if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;

		Albedo_Core_WARN("Unknown shader type!");
		return 0;
	}

	static std::string ShaderTypeToString(GLenum type)
	{
		if (type == GL_VERTEX_SHADER)
			return "vertex";
		if (type == GL_COMPUTE_SHADER)
			return "compute";
		if (type == GL_TESS_CONTROL_SHADER)
			return "tesselation control";
		if (type == GL_TESS_EVALUATION_SHADER)
			return "tesselation evaluation";
		if (type == GL_FRAGMENT_SHADER)
			return "fragment";

		Albedo_Core_WARN("Unknown Opengl shader type!");
		return "none";
	}

	OpenGLShader::OpenGLShader(const std::string& filePath)
		:m_Path(filePath)
	{
		if (!m_Path.empty())
		{
			if (m_Path.find('/') != std::string::npos)
			{
				size_t pos = m_Path.find_last_of('/');
				m_Name = m_Path.substr(pos + 1, m_Path.size());
			}
			else if (m_Path.find('\\') != std::string::npos)
			{
				size_t pos = m_Path.find_last_of('\\');
				m_Name = m_Path.substr(pos + 1, m_Path.size());
			}
			else
			{
				m_Name = m_Path;
			}
		}

 		std::string source = readFile(filePath);
		auto shaderSources = PreProcess(source);
		Compile(shaderSources);	

		//size_t nameStart = filePath.find_last_of("/\\");
		//nameStart = nameStart == std::string::npos ? 0 : nameStart + 1;
		//size_t nameEnd = filePath.rfind(".");
		//size_t nameLength = nameEnd == std::string::npos ? filePath.size() - nameStart : nameEnd - nameStart;
		//m_Name = filePath.substr(nameStart, nameLength);
	}

	std::string OpenGLShader::readFile(const std::string& filePath)
	{
		std::string result;
		std::ifstream in(filePath, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else
			Albedo_Core_ERROR("Couldn't open the file {}", filePath);
		return result;
	}
	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
	{
		Albedo_PROFILE_FUNCTION();
		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLen = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);

		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			size_t beg = pos + typeTokenLen + 1;
			std::string type = source.substr(beg, eol - beg);
			if(!ShaderTypeFromString(type))
				Albedo_Core_ERROR("Invalid shader type specified");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);
			shaderSources[ShaderTypeFromString(type)] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}
		return shaderSources;
	}
	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		Albedo_PROFILE_FUNCTION();
		std::array<GLenum, 4> glShaderIDs;
		int glShaderIDIndex = 0;

		m_ShaderID = glCreateProgram();
		GLuint shaderProgram = m_ShaderID;
		for (auto& kv : shaderSources)
		{
			GLenum type = kv.first;
			const std::string& source = kv.second;

			GLuint shader = glCreateShader(type);
	
			const GLchar* souceCStr = source.c_str();

			glShaderSource(shader, 1, &souceCStr, NULL);
			glCompileShader(shader);

			int  success = 0;
			char infoLog[512];
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

			if (!success)
			{
				glGetShaderInfoLog(shader, 512, NULL, infoLog);
				std::string& shaderType = ShaderTypeToString(type);
				Albedo_Core_ERROR("Shader compilation failed: {}", shaderType);
				glDeleteShader(shader);
			}

			glAttachShader(shaderProgram, shader);
			glShaderIDs[glShaderIDIndex++] = shader;
		}

		glLinkProgram(shaderProgram);

		GLint isLinked = false;
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &isLinked);
		if (!isLinked)
		{
			char infoLog[512];
			Albedo_Core_ERROR("Shader linking failed");
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			glDeleteProgram(shaderProgram);

			for (auto id : glShaderIDs)
				glDeleteShader(id);

			return;
		}

		for (auto id : glShaderIDs)
			glDetachShader(shaderProgram, id);
	}

	OpenGLShader::OpenGLShader(const std::string& name, const char* vertexSrc, const char* fragmentSrc)
		:m_Name(name)
	{
		Albedo_PROFILE_FUNCTION();
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		glShaderSource(vertexShader, 1, &vertexSrc, NULL);
		glCompileShader(vertexShader);

		int  success;
		char infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}


		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(fragmentShader, 1, &fragmentSrc, NULL);
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}


		m_ShaderID = glCreateProgram();
		GLuint shaderProgram = m_ShaderID;

		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);

		GLint isLinked = false;
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &isLinked);
		if (!isLinked)
		{
			Albedo_Core_TRACE("Shader linking failed");

			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
			glDeleteProgram(shaderProgram);

			return;
		}

		glDetachShader(shaderProgram, vertexShader);
		glDetachShader(shaderProgram, fragmentShader);
	}

	OpenGLShader::~OpenGLShader()
	{
		Albedo_PROFILE_FUNCTION();
		glDeleteProgram(m_ShaderID);
	}

	void OpenGLShader::Bind()
	{
		Albedo_PROFILE_FUNCTION();
		glUseProgram(m_ShaderID);
	}

	void OpenGLShader::Unbind()
	{
		Albedo_PROFILE_FUNCTION();
		glUseProgram(0);
	}

	void OpenGLShader::SetUniformInt1(const std::string& name, int value)
	{
		Albedo_PROFILE_FUNCTION();
		UploadUniformInt1(name, value);
	}

	void OpenGLShader::SetUniformIntArray(const std::string& name, int* values, uint32_t count)
	{
		UploadUniformIntArray(name, values, count);
	}

	void OpenGLShader::SetUniformFloat(const std::string& name, float value)
	{
		Albedo_PROFILE_FUNCTION();
		UploadUniformFloat(name, value);
	}

	void OpenGLShader::SetUniformFloat2(const std::string& name, const glm::vec2& value)
	{
		Albedo_PROFILE_FUNCTION();
		UploadUniformFloat2(name, value);
	}

	void OpenGLShader::SetUniformFloat3(const std::string& name, const glm::vec3& value)
	{
		Albedo_PROFILE_FUNCTION();
		UploadUniformFloat3(name, value);
	}

	void OpenGLShader::SetUniformFloat4(const std::string& name, const glm::vec4& value)
	{
		Albedo_PROFILE_FUNCTION();
		UploadUniformFloat4(name, value);
	}

	void OpenGLShader::SetUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		Albedo_PROFILE_FUNCTION();
		UploadUniformMat3(name, matrix);
	}

	void OpenGLShader::SetUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		Albedo_PROFILE_FUNCTION();
		UploadUniformMat4(name, matrix);
	}

	//-------------------------------------------------------------------------------//

	void OpenGLShader::UploadUniformInt1(const std::string& name, int value)
	{
		GLuint location = glGetUniformLocation(m_ShaderID, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_ShaderID, name.c_str());
		glUniform1iv(location, count, values);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
	{
		GLuint location = glGetUniformLocation(m_ShaderID, name.c_str());
		glUniform1f(location, value);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
	{
		GLuint location = glGetUniformLocation(m_ShaderID, name.c_str());
		glUniform2f(location, value.x, value.y);
	}

	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
	{
		GLuint location = glGetUniformLocation(m_ShaderID, name.c_str());
		glUniform3f(location, value.x, value.y, value.z);
	}

	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
	{
		GLuint location = glGetUniformLocation(m_ShaderID, name.c_str());
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		GLuint location = glGetUniformLocation(m_ShaderID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		GLuint location = glGetUniformLocation(m_ShaderID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

}