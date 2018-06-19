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

	std::vector<Shader> bezierShaderList;
	Shader tcsShader, tesShader;
	tcsShader.shaderType = GL_TESS_CONTROL_SHADER;
	tcsShader.shaderCode = readFile("Shaders/oglTess.tcs");
	tcsShader.shaderCode.push_back('\0');
	tesShader.shaderType = GL_TESS_EVALUATION_SHADER;
	tesShader.shaderCode = readFile("Shaders/oglTess.tes");
	tesShader.shaderCode.push_back('\0');
	bezierShaderList.push_back(vtxShader);
	bezierShaderList.push_back(tcsShader);
	bezierShaderList.push_back(tesShader);
	bezierShaderList.push_back(fragShader);

	auto simpleProgram = std::make_shared<GLSLProgram>();
	simpleProgram->createProgram(simpleShaderList);
	myProgramList.push_back(simpleProgram);

	auto bezierProgram = std::make_shared<GLSLProgram>();
	bezierProgram->createProgram(bezierShaderList);
	myProgramList.push_back(bezierProgram);
}

std::weak_ptr<GLSLProgram> GLSLProgramManager::getGLSLProgram(int type)
{
	if (type >= PROGRAM_TYPE::SIZE || type < 0)
		return std::weak_ptr<GLSLProgram>();
	if (myProgramList.size() == 0)
		init();
	return myProgramList[type];
}

