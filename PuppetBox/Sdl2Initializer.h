#ifdef _USE_SDL2
#pragma once

#include <string>

#include <glad/glad.h>
#include <sdl2/SDL.h>

#include "IHardwareInitializer.h"
#include "Logger.h"

class Sdl2Initializer : public IHardwareInitializer
{
public:
	void init(std::string windowTitle, uint32_t windowWidth, uint32_t windowHeight)
	{
		if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		{
			error_ = true;
			LOGGER_ERROR("Failed to initialize SDL");
		}
		else
		{
			int context_flags = 0;

#ifdef _DEBUG
			context_flags |= SDL_GL_CONTEXT_DEBUG_FLAG;
#endif
			//SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, context_flags);
			//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
			//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
			//SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
			//SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
			//SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
			//SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
			//SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
			//SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

			window_ = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_OPENGL & SDL_WINDOW_RESIZABLE);
			if (window_ == nullptr)
			{
				error_ = true;
				LOGGER_ERROR("Failed to create window");
			}
			else
			{
				SDL_GLContext glContext = SDL_GL_CreateContext(window_);
				if (glContext == nullptr)
				{
					error_ = true;
					LOGGER_ERROR("Failed to create context");
				}
				else
				{
					SDL_GL_MakeCurrent(window_, glContext);

					// Initialize GLAD so required function pointers are available
					if (gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
					{
						LOGGER_DEBUG("OpenGL Version: " + std::to_string(GLVersion.major) + "." + std::to_string(GLVersion.minor));
						LOGGER_DEBUG("OpenGL Shading Language Version: " + std::string((char*)glGetString(GL_SHADING_LANGUAGE_VERSION)));
						LOGGER_DEBUG("OpenGL Vendor: " + std::string((char*)glGetString(GL_VENDOR)));
						LOGGER_DEBUG("OpenGL Renderer: " + std::string((char*)glGetString(GL_RENDERER)));
					}
					else
					{
						error_ = true;
						LOGGER_ERROR("Failed to initialize OpenGL api pointers");
					}
				}
			}
		}
	}
	bool hadError() const
	{
		return error_;
	}
private:
	SDL_Window* window_ = nullptr;
	bool error_ = false;
};

#endif //_USE_SDL2