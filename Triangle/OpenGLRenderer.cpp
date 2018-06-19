#include "OpenGLRenderer.h"

#include "LineStripRenderable.h"
#include "BezierRenderable.h"
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
	myCamera.evaluate(width, height);
}

void OpenGLRenderer::addWindow(GLFWwindow * window)
{
	myWindow = window;
}

bool OpenGLRenderer::myInitOpengl()
{
	
	return true;
}

void OpenGLRenderer::myCreateScene()
{
	auto lines = std::make_shared<LineStripRenderable>();
	lines->addProgram(myProgramManager.getGLSLProgram(GLSLProgramManager::PROGRAM_TYPE::SIMPLE));
	mySceneObjectList.push_back(lines);
	auto bezier = std::make_shared<BezierRenderable>();
	bezier->addProgram(myProgramManager.getGLSLProgram(GLSLProgramManager::PROGRAM_TYPE::BEZIER));
	mySceneObjectList.push_back(bezier);
	int width, height;
	glfwGetWindowSize(myWindow, &width, &height);
	myCamera.evaluate(width, height);
}

void OpenGLRenderer::myRender()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0f);
	
	for (auto& object : mySceneObjectList)
		object->render(myCamera.getMVP());
}

void OpenGLRenderer::mySwapBuffer()
{
	glfwSwapBuffers(myWindow);
}
