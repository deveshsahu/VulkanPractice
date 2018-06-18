#include "GLSLProgramManager.h"
#include "Helper.h"

void GLSLProgramManager::init()
{
	std::vector<Shader> simpleShaderList;
	Shader vtxShader, fragShader;
	vtxShader.shaderType = GL_VERTEX_SHADER;
	vtxShader.shaderCode = readFile("Shaders/oglVert.vs");
	vtxShader.shaderCode.push_back('\0');
	fragShader.shaderType = GL_FRAGMENT_SHADER;
	fragShader.shaderCode = readFile("Shaders/oglFrag.fs");
	fragShader.shaderCode.push_back('\0');
	simpleShaderList.push_back(vtxShader);
	simpleShaderList.push_back(fragShader);

	auto simpleProgram = std::make_shared<GLSLProgram>();
	simpleProgram->createProgram(simpleShaderList);
	myProgramList.push_back(simpleProgram);
}

std::weak_ptr<GLSLProgram> GLSLProgramManager::getGLSLProgram(int type)
{
	if (type >= PROGRAM_TYPE::SIZE || type < 0)
		return std::weak_ptr<GLSLProgram>();
	if (myProgramList.size() == 0)
		init();
	return myProgramList[type];
}

