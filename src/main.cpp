#include "application/AJekyllApplication.hpp"

#include <iostream>

int main(int argc, char* argv[]) {
	AJekyllApplication app;

	if (!app.Setup()) {
		std::cout << "Failed to set up Jekyll, please contact Gamma!" << std::endl;
		return 0;
	}

	app.Run();

	if (!app.Teardown()) {
		std::cout << "Something went wrong on teardown, please contact Gamma!" << std::endl;
		return 0;
	}
}
