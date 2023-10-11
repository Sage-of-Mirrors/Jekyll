#pragma once

#include "application/AApplication.hpp"


struct GLFWwindow;
class AJekyllContext;

class AJekyllApplication : public AApplication {
	GLFWwindow* mWindow;
	AJekyllContext* mContext;

	virtual bool Execute(float deltaTime) override;

public:
	AJekyllApplication();
	virtual ~AJekyllApplication() {}

	virtual bool Setup() override;
	virtual bool Teardown() override;
};

void GLFWDropCallback(GLFWwindow* window, int count, const char* paths[]);
