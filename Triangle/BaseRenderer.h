#pragma once
/**
* @brief Base class for all types of renderer
*/
class BaseRenderer
{
public:
	BaseRenderer() = default;
	virtual ~BaseRenderer() = default;

	enum RENDERER_TYPE
	{
		OPENGL,
		VULKAN,
		SIZE
	};
	/** Initialize renderer */
	virtual void init() = 0;
	/** Run inside the application's glfw loop*/
	virtual void mainLoop() = 0;
	/** Any additional checks to be performed after glfw loop exits */
	virtual void postLoop() {};
	/** Cleanup variables, buffers etc*/
	virtual void cleanup() = 0;

	/**
	* Store window pointer for convenient access
	* by the Renderer
	*/
	virtual void addWindow(GLFWwindow * window) { myWindow = window; }

	/** 
	* Returns the renderer type
	*/
	virtual RENDERER_TYPE whichRenderer() const = 0;

	/** Window resizing callback  */
	virtual void resizeCallback(int width, int height) = 0;

	bool isBroken() const { return myBroken; }
	void flagAsBroken() { myBroken = true; }

protected:
	GLFWwindow * myWindow = nullptr;
	bool myBroken = false;

};

