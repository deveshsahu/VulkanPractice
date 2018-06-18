#include "TriangleApplication.h"

#include <stb_image.h>
#include <stdexcept>
#include <iostream>
#include <set>
#include <algorithm>

const int WIDTH = 800;
const int HEIGHT = 600;


TriangleApplication::TriangleApplication()
{
	//myCurrentRenderer = &myOpenglRenderer;
	myCurrentRenderer = &myVulkanRenderer;
}

TriangleApplication::~TriangleApplication()
{
}

void TriangleApplication::run()
{
	myInitWindow();
	myMainLoop();
	myCleanup();
}

void TriangleApplication::myInitWindow()
{
	glfwInit();
	auto renderertype = myCurrentRenderer->whichRenderer();
	if ( renderertype == BaseRenderer::VULKAN)
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	}
	else if (renderertype == BaseRenderer::OPENGL)
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_ANY_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	}

	myWindow = glfwCreateWindow(WIDTH, HEIGHT, "Triangle", nullptr, nullptr);

	glfwSetWindowUserPointer(myWindow, this);
	glfwSetWindowSizeCallback(myWindow, TriangleApplication::onWindowResized);
	glfwSetCursorPosCallback(myWindow, TriangleApplication::onCursorMove);
	glfwSetMouseButtonCallback(myWindow, TriangleApplication::onMouseClicked);
	
	myCurrentRenderer->addWindow(myWindow);
	try
	{
		myCurrentRenderer->init();
	}
	catch(const std::string err)
	{
		throw (err);
	}
}

void TriangleApplication::myMainLoop()
{
	while (!glfwWindowShouldClose(myWindow))
	{
		glfwPollEvents();
		myCurrentRenderer->mainLoop();
	}
	myCurrentRenderer->postLoop();
}

void TriangleApplication::myCleanup()
{
	glfwDestroyWindow(myWindow);
	myCurrentRenderer->cleanup();
	glfwTerminate();
}

void TriangleApplication::myResizeCallback(int width, int height)
{
	myCurrentRenderer->resizeCallback(width, height);
}

void TriangleApplication::onWindowResized(GLFWwindow * window, int width, int height)
{
	if (width == 0 || height == 0)
		return;
	auto app = reinterpret_cast<TriangleApplication*> (glfwGetWindowUserPointer(window));
	app->myResizeCallback(width, height);
}

void TriangleApplication::onCursorMove(GLFWwindow * window, double xpos, double ypos)
{
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (state == GLFW_PRESS)
		std::cout << "Called" << std::endl;
}

void TriangleApplication::onMouseClicked(GLFWwindow * window, int button, int action, int mods)
{

}

