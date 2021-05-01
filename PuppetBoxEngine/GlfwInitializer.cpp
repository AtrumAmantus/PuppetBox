#include "pch.h"
#ifdef _USE_GLFW

#include <iostream>

#include "GlfwInitializer.h"
#include "Logger.h"
#include "WindowProperties.h"

namespace PB
{
	namespace
	{
		WindowProperties windowProperties{};

		// Update the window size on resize events
		void glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height)
		{
			windowProperties.resizeWindow(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
		}

		void glfw_mouse_callback(GLFWwindow* window, double xpos, double ypos)
		{
			windowProperties.updateMouse(static_cast<uint32_t>(xpos), static_cast<uint32_t>(ypos));
		}

		void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
		{
			windowProperties.mouse.scrollDirection = static_cast<uint32_t>(yoffset / abs(yoffset));
		}
	}

	void GlfwInitializer::init(std::string windowTitle, uint32_t windowWidth, uint32_t windowHeight)
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //<-- Mac only

		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

		// Used to translate window level information to input processor
		windowProperties.resizeWindow(windowWidth, windowHeight);
		windowProperties.mouse.sensitivity = 0.1f;

		// Create a window
		window_ = glfwCreateWindow(windowWidth, windowHeight, windowTitle.c_str(), NULL, NULL);

		if (window_ != NULL)
		{
			// Set window as main context of current thread
			glfwMakeContextCurrent(window_);

			// Set callback for resize events
			glfwSetFramebufferSizeCallback(window_, glfw_framebuffer_size_callback);

			// Set initial cursor pos (avoids initial movement jolts)
			glfwSetCursorPos(window_, (double)windowWidth / 2.0, (double)windowHeight / 2.0);

			// Set mouse input callback
			glfwSetCursorPosCallback(window_, glfw_mouse_callback);

			// Set mouse scroll callback
			glfwSetScrollCallback(window_, glfw_scroll_callback);

			// Lock and hide cursor
			//glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			error_ = true;
			LOGGER_ERROR("Failed to create window");
		}
	}

	void GlfwInitializer::shutdown()
	{
		glfwTerminate();
	}

	bool GlfwInitializer::hadError() const
	{
		return error_;
	}

	void GlfwInitializer::postLoopCommands() const
	{
		glfwSwapBuffers(window_);
		glfwPollEvents();
	}

	void GlfwInitializer::initializeGameTime()
	{
		lastFrameTime_ = glfwGetTime();
	}

	float GlfwInitializer::updateElapsedTime()
	{
		float NOW = glfwGetTime();
		float deltaTime = NOW - lastFrameTime_;
		lastFrameTime_ = NOW;
		return deltaTime;
	}

	ProcAddress GlfwInitializer::getProcAddress() const
	{
		return (ProcAddress)glfwGetProcAddress;
	}

	std::string GlfwInitializer::initializerName() const
	{
		return "GLFW";
	}

	GLFWwindow& GlfwInitializer::getWindow() const
	{
		return *window_;
	}

	WindowProperties* GlfwInitializer::getWindowProperties() const
	{
		return &windowProperties;
	}
}

#endif //_USE_GLFW