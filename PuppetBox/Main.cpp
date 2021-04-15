#include <iostream>

#include "Engine.h"

#ifdef _USE_GLFW
#include "GlfwInitializer.h"
#include "GlfwInputProcessor.h"
#endif //_USE_GLFW
#ifdef _USE_SDL2
#include "Sdl2Initializer.h"
#include "Sdl2InputProcessor.h"
#endif //_USE_SDL2

#include "GladGfxApi.h"

// Entry point is in SDL2
int main(int argc, char** argv)
{
	// Initialize PuppetBox namespace elements
	PB::Init();

	// Create hardware Api instance
#ifdef _USE_SDL2
	PB::Sdl2InputProcessor inputProcessor{};
	PB::Sdl2Initializer hardwareInitializer{};
#endif //_USE_SDL2

#ifdef _USE_GLFW
	PB::GlfwInitializer hardwareInitializer{};
#endif //_UES_GLFW

	// Initialize hardware Api
	hardwareInitializer.init("PuppetBox", 1024, 768);
	if (!hardwareInitializer.hadError())
	{
		std::cout << hardwareInitializer.initializerName() << " loaded." << std::endl;

		// GLFW needs window reference for input polling
#ifdef _USE_GLFW
		PB::GlfwInputProcessor inputProcessor{ hardwareInitializer.getWindow() };
		inputProcessor.init(hardwareInitializer.getWindowProperties());
#endif //_USE_GLFW

		// Graphics API
		PB::GladGfxApi gfxApi{};
		// TODO: fix the redundancy of this configuration
		gfxApi.setRenderDimensions(1024, 768);

		if (gfxApi.load(hardwareInitializer))
		{
			PB::Engine engine{ &gfxApi, &hardwareInitializer, &inputProcessor };

			if (engine.init())
			{
				engine.run();
			}
			else
			{
				LOGGER_ERROR("Failed to initialize Engine");
				return 1;
			}
		}
		else
		{
			LOGGER_ERROR("Failed to initialize GFX Api");
			return 1;
		}
	}
	else
	{
		LOGGER_ERROR("Failed to initialize hardware");
		return 1;
	}

	return 0;
}