#include "Surface.h"

glm::vec3 Surface::getSurfacePoint(float u, float v)
{
	/*float uspan = U.findSpan(u);
	auto uBasis = U.evaluateBasisFunctions(uspan, u);
	float vspan = V.findSpan(v);
	auto vBasis = V.evaluateBasisFunctions(vspan, v);
	auto p = U.getDegree(),
		q = V.getDegree();
	float uind = uspan - p;*/
	glm::vec3 S(0.0f);
	/*for (int l = 0; l <= q; ++l)
	{
		float temp = 0.0;
		float vind = vspan - q + l;
		for (int k = 0; k <= p; ++k)
			temp += uBasis[k]*;
		S += vBasis[l] * temp;
	}*/
	return S;
}
