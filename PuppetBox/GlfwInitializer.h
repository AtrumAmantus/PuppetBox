#ifdef _USE_GLFW

#pragma once

#include <string>

#include <GLFW/glfw3.h>

#include "IHardwareInitializer.h"

class GlfwInitializer : public IHardwareInitializer
{
public:
	void init(std::string windowTitle, uint32_t windowWidth, uint32_t windowHeight);
	bool hadError() const;
	GLFWwindow& getWindow() const;
private:
	GLFWwindow* window_ = nullptr;
	bool error_ = false;
};

#endif //_USE_GLFW