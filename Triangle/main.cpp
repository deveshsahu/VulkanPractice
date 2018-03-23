#include "TriangleApplication.h"

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
}