#include "Surface.h"

glm::vec3 Surface::getSurfacePoint(float u, float v)
{
	float uspan = U.findSpan(u);
	auto uBasis = U.evaluateBasisFunctions(uspan, u);
	float vspan = V.findSpan(v);
	auto vBasis = V.evaluateBasisFunctions(vspan, v);
	
	float uind = uspan - U.myDegree;
	glm::vec3 S(0.0f);
	for (int l = 0; l <= V.myDegree; ++l)
	{
		glm::vec3 temp(0.f);
		float vind = vspan - V.myDegree + l;
		for (int k = 0; k <= U.myDegree; ++k)
			temp += uBasis[k] * myGetControlPoint(uind+k, vind);
		S += vBasis[l] * temp;
	}
	return S;
}

glm::vec3 Surface::myGetControlPoint(int uind, int vind)
{
	return myControlPoints[uind * (V.myDegree - V.myKnot.size() - 1) + uind];
}

int Parameters::findSpan(float u)
{
	int numBasisFunc = myKnot.size() - myDegree - 1;
	if (u == myKnot[numBasisFunc + 1])
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

std::vector<float> Parameters::evaluateBasisFunctions(int i, float u)
{
	std::vector<float> basisFunctionValues(myKnot.size() - myDegree - 1);
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