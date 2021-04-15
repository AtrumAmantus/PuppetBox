#ifdef _USE_SDL2
#pragma once

#include <string>

#include <sdl2/SDL.h>

#include "IHardwareInitializer.h"
#include "Logger.h"
#include "TypeDef.h"

namespace PB
{
	class Sdl2Initializer : public IHardwareInitializer
	{
	public:
		void init(std::string windowTitle, uint32_t windowWidth, uint32_t windowHeight)
		{
			if (SDL_Init(SDL_INIT_EVERYTHING) >= 0)
			{
				int context_flags = 0;

#ifdef _DEBUG
				context_flags |= SDL_GL_CONTEXT_DEBUG_FLAG;
#endif
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, context_flags);
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
				SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
				SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
				SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
				SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
				SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
				SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
				SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

				window_ = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
				if (window_ != nullptr)
				{
					SDL_GLContext glContext = SDL_GL_CreateContext(window_);
					if (glContext != nullptr)
					{
						SDL_GL_MakeCurrent(window_, glContext);
					}
					else
					{
						error_ = true;
						LOGGER_ERROR("Failed to create context");
					}
				}
				else
				{
					error_ = true;
					LOGGER_ERROR("Failed to create window");
				}
			}
			else
			{
				error_ = true;
				LOGGER_ERROR("Failed to initialize SDL");
			}
		};
		bool hadError() const
		{
			return error_;
		};
		void postLoopCommands() const
		{
			SDL_GL_SwapWindow(window_);
		};
		ProcAddress getProcAddress() const
		{
			return SDL_GL_GetProcAddress;
		};
		std::string initializerName() const
		{
			return "SDL2";
		};
	private:
		SDL_Window* window_ = nullptr;
		bool error_ = false;
	};
}

#endif //_USE_SDL2