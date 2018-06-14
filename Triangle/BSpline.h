#pragma once
/* Data structure to handle BSpline */
#include <vector>
#include <glm\glm.hpp>

class BSpline
{
public:
	BSpline(int degree, std::vector<float>& knotVector, std::vector<glm::vec3>& controlPoints);
	virtual ~BSpline() = default;

	int findSpan(float u);
	std::vector<float> evaluateBasisFunctions(int i, float u);
	glm::vec3 curveValue(float u);

	void setKnotVector(std::vector<float> & vect) { myKnot = vect; }
	void setDegree(int deg) { myDegree = deg; }
	unsigned int getDegree() const { return myDegree; }
	void setControlPoints(std::vector<glm::vec3>& cp) { myControlPoints = cp; }
protected:
	std::vector<float> myKnot; 
	std::vector<glm::vec3> myControlPoints;
	unsigned int myDegree;
};

