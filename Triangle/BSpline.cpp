#include "BSpline.h"



BSpline::BSpline(int degree, std::vector<float>& knotVector, std::vector<glm::vec3>& controlPoints):
	myDegree(degree),
	myControlPoints(controlPoints),
	myKnot(knotVector)
{
}

int BSpline::findSpan(float u)
{
	int numBasisFunc = myKnot.size() - myDegree - 1;
	if (u == myKnot[numBasisFunc+1])
		return numBasisFunc;
	// Perform binary search to retrieve interval
	// where u belongs
	int low = myDegree,
		hi = numBasisFunc;
	int mid = 0.5 * (low + hi);
	while (u < myKnot[mid] || u >= myKnot[mid + 1])
	{
		if (u < myKnot[mid])
			hi = mid;
		else
			low = mid;
		mid = 0.5 * (low + hi);
	}
	return mid;
}

std::vector<float> BSpline::evaluateBasisFunctions(int i, float u)
{
	std::vector<float> basisFunctionValues(myKnot.size() - myDegree-1);
	basisFunctionValues[0] = 0.f;
	std::vector<float> left(myDegree), right(myDegree);
	float saved = 0.0;
	for (int j = 1; j <= myDegree; ++j)
	{
		left[j] = u - myKnot[i + 1 - j];
		right[j] = myKnot[i - j] - u;	
		for (int r = 0; r < j; ++r)
		{
			float temp = basisFunctionValues[r] / (right[r + 1] + left[j - r]);
			basisFunctionValues[r] = saved + right[r + 1] * temp;
			saved = left[j - r] * temp;
		}
		basisFunctionValues[j] = saved;
	}
	return basisFunctionValues;
}

glm::vec3 BSpline::curveValue(float u)
{
	int span = findSpan(u);
	auto basisFunctions = evaluateBasisFunctions(span, u);
	glm::vec3 curveValue;
	for (int i = 0; i <= myDegree; ++i)
	{
		curveValue += basisFunctions[i] * myControlPoints[span - myDegree + i];
	}
	return curveValue;
}
