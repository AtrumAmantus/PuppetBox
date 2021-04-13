#ifdef _USE_GLFW

#include <iostream>

#include <glad/glad.h>

#include "GlfwInitializer.h"

namespace
{
	//// Update the window size on resize events
	//void glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height)
	//{
	//	// Grab window property data from UserPointer to update screen size values
	//	WindowProperties* windowProperties = (WindowProperties*)glfwGetWindowUserPointer(window);
	//	windowProperties->resizeWindow(width, height);

	//	// Adjust render window to new size
	//	glViewport(0, 0, width, height);
	//}

	//void glfw_mouse_callback(GLFWwindow* window, double xpos, double ypos)
	//{
	//	// Grab window property data from UserPointer to update mouse coords
	//	WindowProperties* windowProperties = (WindowProperties*)glfwGetWindowUserPointer(window);
	//	windowProperties->updateMouse(xpos, ypos);
	//}

	//void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	//{
	//	// Grab window property data from UserPointer to update mouse coords
	//	WindowProperties* windowProperties = (WindowProperties*)glfwGetWindowUserPointer(window);
	//	windowProperties->setMouseScroll((yoffset > 0.0) ? 1 : -1);
	//}
}

void GlfwInitializer::init(std::string windowTitle, uint32_t windowWidth, uint32_t windowHeight)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //<-- Mac only

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

	//// We use this to reference the window size throughout the application
	//WindowProperties* windowProperties = new WindowProperties;
	//windowProperties->resizeWindow(INIT_RESOLUTION_WIDTH, INIT_RESOLUTION_HEIGHT);
	//windowProperties->setMouseSensitivity(0.1f);

	// Create a window
	window_ = glfwCreateWindow(windowWidth, windowHeight, windowTitle.c_str(), NULL, NULL);

	if (window_ != NULL)
	{
		// Set window as main context of current thread
		glfwMakeContextCurrent(window_);

		//// Set callback for resize events
		//glfwSetFramebufferSizeCallback(window_, glfw_framebuffer_size_callback);

		// SEt initial cursor pos (avoids initial movement jolts)
		glfwSetCursorPos(window_, (double)windowWidth / 2.0, (double)windowHeight / 2.0);

		//// Set mouse input callback
		//glfwSetCursorPosCallback(window_, glfw_mouse_callback);

		//// Set mouse scroll callback
		//glfwSetScrollCallback(window_, glfw_scroll_callback);

		//// Store window propreties in UserPointer to retrieve later in window resize callback
		//glfwSetWindowUserPointer(window_, windowProperties);

		// Initialize GLAD so required function pointers are available
		if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			const uint8_t* string = glGetString(GL_VERSION);
			std::cout << "OpenGL Version: " << string << std::endl;
		}
	}
}

bool GlfwInitializer::hadError() const
{
	return error_;
}

GLFWwindow& GlfwInitializer::getWindow() const
{
	return *window_;
}

#endif //_USE_GLFW