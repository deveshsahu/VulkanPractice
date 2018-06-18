#pragma once

#include "VulkanRenderer.h"
#include "OpenGLRenderer.h"
#include <vector>

class TriangleApplication
{
public:
	TriangleApplication();
	~TriangleApplication();

	void run();

	VulkanRenderer& getVulkanRenderer() { return myVulkanRenderer; }
	OpenGLRenderer& getOpenglRenderer() { return myOpenglRenderer; }

	// Static 
	static void onWindowResized(GLFWwindow* window, int width, int height);
	static void onCursorMove(GLFWwindow* window, double xpos, double ypos);
	static void onMouseClicked(GLFWwindow* window, int button, int action, int mods);

private:
	void myInitWindow();
	void myMainLoop();
	void myCleanup();

	void myResizeCallback(int width, int height);
	
	bool myLeftClicked;

	GLFWwindow * myWindow;

	VulkanRenderer myVulkanRenderer;
	OpenGLRenderer myOpenglRenderer;

	BaseRenderer * myCurrentRenderer;
};