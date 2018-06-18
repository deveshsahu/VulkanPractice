#pragma once

#include<glm\glm.hpp>
#include <vector>

struct Parameters
{
	unsigned int myDegree;
	std::vector<float> myKnot;

	int findSpan(float u);
	std::vector<float> evaluateBasisFunctions(int i, float u);
};

class Surface
{
public:
	Surface() = default;
	virtual ~Surface() = default;

	glm::vec3 getSurfacePoint(float u, float v);


private:
	glm::vec3 myGetControlPoint(int uind, int vind);
	std::vector<glm::vec3> myControlPoints;

	Parameters U, V;
};

