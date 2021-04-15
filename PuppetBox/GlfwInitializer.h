#ifdef _USE_GLFW

#pragma once

#include <string>

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>

#include "IHardwareInitializer.h"
#include "TypeDef.h"
#include "WindowProperties.h"

namespace PB
{
	class GlfwInitializer : public IHardwareInitializer
	{
	public:
		void init(std::string windowTitle, uint32_t windowWidth, uint32_t windowHeight);
		bool hadError() const;
		void postLoopCommands() const;
		ProcAddress getProcAddress() const;
		std::string initializerName() const;
		GLFWwindow& getWindow() const;
		WindowProperties* getWindowProperties() const;
	private:
		GLFWwindow* window_ = nullptr;
		bool error_ = false;
	};
}
#endif //_USE_GLFW