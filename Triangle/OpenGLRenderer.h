#pragma once
#include "gl_core_4_5.h"
#include <GLFW\glfw3.h>
#include "Util.h"
#include "BaseRenderer.h"
#include "BaseRenderable.h"
#include "GLSLProgramManager.h"
#include "Camera.h"

class OpenGLRenderer : public BaseRenderer
{
public:
	OpenGLRenderer() = default;
	~OpenGLRenderer() = default;

	void init() override;
	void mainLoop() override;
	void cleanup() override;

	void resizeCallback(int width, int height) override;

	RENDERER_TYPE whichRenderer() const override { return OPENGL; }

	void addWindow(GLFWwindow* window);

private:
	bool myInitOpengl();
	void myCreateScene();
	void myRender();
	void mySwapBuffer();

	Camera myCamera;

	GLSLProgramManager myProgramManager;
	std::vector<std::shared_ptr<BaseRenderable>> mySceneObjectList;
};