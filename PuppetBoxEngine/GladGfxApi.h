#pragma once

#include <iostream>

#include <glad/glad.h>

#include "IGfxApi.h"
#include "IHardwareInitializer.h"
#include "Logger.h"
#include "TypeDef.h"

namespace PB
{
	class GladGfxApi : public IGfxApi
	{
	public:
		bool load(const IHardwareInitializer& hardwareInitializer) const
		{
			bool error = false;

			if (gladLoadGLLoader(hardwareInitializer.getProcAddress()))
			{
				std::cout << "OpenGL Version: " << GLVersion.major << "." << GLVersion.minor << std::endl;
				std::cout << "OpenGL Shading Language Version: " << (char*)glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
				std::cout << "OpenGL Vendor: " << (char*)glGetString(GL_VENDOR) << std::endl;
				std::cout << "OpenGL Renderer: " << (char*)glGetString(GL_RENDERER) << std::endl;
			}
			else
			{
				error = true;
				LOGGER_ERROR("Failed to initialize OpenGL api pointers");
			}

			return !error;
		}
		void preLoopCommands() const
		{
			glViewport(0, 0, width_, height_);
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		void setRenderDimensions(uint32_t width, uint32_t height)
		{
			width_ = width;
			height_ = height;
		}
	private:
		uint32_t width_ = 0;
		uint32_t height_ = 0;
	};
}
