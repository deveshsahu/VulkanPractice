#pragma once
#include "gl_core_4_5.h"
#include "glm\glm.hpp"
#include <vector>
#include <string>

struct Shader
{
	std::vector<char> shaderCode;
	GLenum shaderType;
};
class GLSLProgram
{
public:
	
	GLSLProgram();
	~GLSLProgram();

	void use();
	void createProgram(std::vector<Shader>& shaders);
	void setUniform(const glm::mat4& mat, const char* name);
	void setUniform(float num, const char* name);
	void setUniform(int num, const char* name);
private:
	GLuint 	myProgram = -1;
};

