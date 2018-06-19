#include "Camera.h"



Camera::Camera()
{
}


Camera::~Camera()
{
}

void Camera::evaluate(int width, int height)
{
	myMVP.view = glm::translate(myMVP.view, glm::vec3(0.0, 0.0, 0.0f));
	//myMVP.view = glm::lookAt(myCameraLocation, mySceneOrigin, myUp);
	if (myProjection == ORTHOGRAPHIC)
		myEvaluateOrtho(width, height);
	else
		myEvaluatePersp(width, height);
	myMVP.update();
}

void Camera::myEvaluateOrtho(int width, int height)
{
	float aspectRatio = (float)height / (float)width;
	myMVP.projection = glm::ortho(-1.0f, 1.0f, -aspectRatio, aspectRatio, -1.0f, 1.0f);
}

void Camera::myEvaluatePersp(int width, int height)
{
	float aspectRatio = (float)height / (float)width;
	myMVP.projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
}
