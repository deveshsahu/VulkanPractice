#include "OpenGLRenderer.h"

#include "LineStripRenderable.h"
#include <iostream>

void OpenGLRenderer::init()
{
	if (isBroken())
		return;
	glfwMakeContextCurrent(myWindow);
	int num = ogl_LoadFunctions();
	std::cout << "INFO: " << num << " out of "
		<< ogl_LOAD_SUCCEEDED << "successfully loaded!" << std::endl;
	if (num == 0)
		flagAsBroken();
	if (!myInitOpengl())
		flagAsBroken();
	myCreateScene();
}

void OpenGLRenderer::mainLoop()
{
	if (isBroken())
		return;
	myRender();
	mySwapBuffer();
}

void OpenGLRenderer::cleanup()
{
	if (isBroken())
		return;

}

void OpenGLRenderer::resizeCallback(int width, int height)
{
	glViewport(0, 0, width, height);
}

void OpenGLRenderer::addWindow(GLFWwindow * window)
{
	myWindow = window;
}

bool OpenGLRenderer::myInitOpengl()
{
	int width, height;
	glfwGetWindowSize(myWindow, &width, &height);
	return true;
}

void OpenGLRenderer::myCreateScene()
{
	auto lines = std::make_shared<LineStripRenderable>();
	lines->addProgram(myProgramManager.getGLSLProgram(GLSLProgramManager::PROGRAM_TYPE::SIMPLE));
	myScneObjectList.push_back(lines);

	//myMVP.projection = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, 0.1f, 100.0f);
	myMVP.update();
}

void OpenGLRenderer::myRender()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(1.0, 0.0, 0.0, 1.0f);
	
	for (auto& object : myScneObjectList)
		object->render(myMVP);
}

void OpenGLRenderer::mySwapBuffer()
{
	glfwSwapBuffers(myWindow);
}
