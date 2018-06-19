#pragma once
#include "GLSLProgram.h"
#include <memory>
class GLSLProgramManager
{
public:
	enum PROGRAM_TYPE
	{
		SIMPLE,
		BEZIER,
		SIZE
	};
	GLSLProgramManager() = default;
	~GLSLProgramManager() = default;

	void init();
	std::weak_ptr<GLSLProgram> getGLSLProgram(int type);
private:
	std::vector<std::shared_ptr<GLSLProgram>> myProgramList;
};

