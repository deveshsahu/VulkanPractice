#include "GLSLProgram.h"

#include <iostream>

GLSLProgram::GLSLProgram()
{
}

GLSLProgram::~GLSLProgram()
{
}

void GLSLProgram::use()
{
	glUseProgram(myProgram);
}

void GLSLProgram::createProgram(std::vector<Shader>& shaderList)
{
	myProgram = glCreateProgram();
	if (myProgram == 0)
	{
		std::cout << "ERROR: Failed to create GLSL program" << std::endl;
		return;
	}
	std::vector<GLuint> myShaderIDList;
	myShaderIDList.reserve(shaderList.size());
	for (auto& shader : shaderList)
	{
		GLuint id = glCreateShader(shader.shaderType);
		if (id == 0)
		{
			std::cout << "ERROR: could not create shader object" << std::endl;
			return;
		}
		const char * code[] = { shader.shaderCode.data() };
		glShaderSource(id, 1, code, nullptr);
		glCompileShader(id);
		GLint result;
		glGetShaderiv(id, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE)
		{
			std::cout << "ERROR: Failed compiling shader " << shader.shaderType << std::endl;
			GLint logLen;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLen);
			if (logLen > 0)
			{
				GLchar* log = new GLchar[logLen];
				GLsizei written;
				glGetShaderInfoLog(id, logLen, &written, log);
				std::cout << log << std::endl;
				delete[] log;
			}
			continue;
		}
		glAttachShader(myProgram, id);
	}
	glLinkProgram(myProgram);
	GLint status;
	glGetProgramiv(myProgram, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		std::cout << "ERROR: Failed to link Shader" << std::endl;
		GLint logLen;
		glGetProgramiv(myProgram, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0)
		{
			GLsizei written;
			GLchar * log = new GLchar[logLen];
			glGetProgramInfoLog(myProgram, logLen, &written, log);
			std::cout << log << std::endl;
			delete[] log;
		}
	}

}

void GLSLProgram::setUniform(const glm::mat4 & mat, const char * name)
{
	auto location = glGetUniformLocation(myProgram, name);
	if (location >= 0)
		glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]);
}

void GLSLProgram::setUniform(float num, const char * name)
{
	auto location = glGetUniformLocation(myProgram, name);
	if (location >= 0)
		glUniform1f(location, num);
}

void GLSLProgram::setUniform(int num, const char * name)
{
	auto location = glGetUniformLocation(myProgram, name);
	if (location >= 0)
		glUniform1i(location, num);
}