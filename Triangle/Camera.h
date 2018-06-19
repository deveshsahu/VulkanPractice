#pragma once
#include "Util.h"
class Camera
{
public:
	enum PROJECTION
	{
		ORTHOGRAPHIC,
		PERSPECTIVE,
		SIZE
	};
	Camera();
	~Camera();

	MVP getMVP() const { return myMVP; }
	void evaluate(int width, int height);

protected:
	void myEvaluateOrtho(int width, int height);
	void myEvaluatePersp(int width, int height);
	MVP myMVP = MVP();
	PROJECTION myProjection = ORTHOGRAPHIC;

	glm::vec3 myCameraLocation = glm::vec3(0.0f, 0.0, -10.0);
	glm::vec3 mySceneOrigin = glm::vec3(0.f);
	glm::vec3 myUp = glm::vec3(0.0, 1.0, 0.0f);
};

