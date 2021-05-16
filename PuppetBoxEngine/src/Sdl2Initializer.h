#pragma once

#include <string>

#include <sdl2/SDL.h>

#include "IHardwareInitializer.h"
#include "Logger.h"
#include "TypeDef.h"

namespace PB
{
	/**
	* \brief SDL2 specific implementation for the IHardwareInitializer for hardware interactions.
	*/
	class Sdl2Initializer : public IHardwareInitializer
	{
	public:
		/**
		* \brief Invokes the SDL2 specific functions to initialize hardware configuration for future interactions.
		* 
		* \param windowTitle	The desired title for the window to be created.
		* \param windowWidth	The desired width for the window to be created.
		* \param windowHeight	The desired height for the window to be created.
		*/
		void init(std::string windowTitle, std::int32_t windowWidth, std::int32_t windowHeight) override
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

		/**
		* \brief Releases any allocated resources and cleans up SDL2 specific configurations.
		*/
		void destroy() override
		{
			if (window_) SDL_DestroyWindow(window_);
			SDL_Quit();
		};

		/**
		* \brief Determines if an error occured during hardware initialization.
		* 
		* \return True if an error occured during initialization, False otherwise.
		*/
		bool hadError() const override
		{
			return error_;
		};

		/**
		* \brief The SDL2 specific commands to be executed after each loop, such as buffer swapping.
		*/
		void postLoopCommands() const override
		{
			SDL_GL_SwapWindow(window_);
		};

		/**
		* \brief Initialize the game time for later tracking time deltas between frames.
		*/
		void initializeGameTime() override
		{
			lastFrameTime_ = SDL_GetPerformanceCounter();
		};

		/**
		* \brief Re-calculate time elapsed since last invocation.
		* 
		* \return The amount of time (in Milliseconds) since the last time the method was invoked.
		*/
		float updateElapsedTime() override
		{
			std::uint64_t NOW = SDL_GetPerformanceCounter();
			std::uint64_t delta = (NOW - lastFrameTime_);
			lastFrameTime_ = NOW;

			return static_cast<float>(delta) / static_cast<float>(SDL_GetPerformanceFrequency());
		};

		/**
		* \brief Get a reference to the process address for configuring function pointers.
		* 
		* \return The SDL2 specific process address for function pointers.
		*/
		ProcAddress getProcAddress() const override
		{
			return SDL_GL_GetProcAddress;
		};

		/**
		* \brief Identifies the specific IHardwareInitializer by a string value.
		* 
		* \return The specific IHardwareInitializer identifier for this hardware library implementation.
		*/
		std::string initializerName() const override
		{
			return "SDL2";
		};
	private:
		std::uint64_t lastFrameTime_ = 0;
		SDL_Window* window_ = nullptr;
		bool error_ = false;
	};
}