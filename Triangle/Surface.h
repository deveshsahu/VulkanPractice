#pragma once

#include<glm\glm.hpp>
#include <vector>
class Surface
{
public:
	Surface() = default;
	virtual ~Surface() = default;

	glm::vec3 getSurfacePoint(float u, float v);


private:
	glm::vec3 myGetControlPoint(int uind, int vind);
	std::vector<glm::vec3> myControlPoints;
};

