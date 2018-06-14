#include "TriangleApplication.h"

#include "BSpline.h"

#include <iostream>
#include <stdexcept>
#include <functional>

int main() {
	TriangleApplication app;
	try {
		app.run();
	}
	catch (const std::runtime_error& err){
		std::cerr << err.what() << std::endl;
		getchar();
		return EXIT_FAILURE;
	}
	

	int degree = 2;
	std::vector<float> knotVect(11);
	knotVect[0] = 0; knotVect[1] = 0; knotVect[2] = 0;
	knotVect[3] = 1; knotVect[4] = 2; knotVect[5] = 3;
	knotVect[6] = 4; knotVect[7] = 4; knotVect[8] = 5;
	knotVect[9] = 5; knotVect[10] = 5;
	std::vector<glm::vec3> controlPoints(11 - 2 - 1);

	BSpline mySpline(degree, knotVect, controlPoints);
}