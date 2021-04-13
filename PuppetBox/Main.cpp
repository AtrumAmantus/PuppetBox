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

// Entry point is in SDL2
int main(int argc, char** argv)
{
	PB::Init();

#ifdef _USE_SDL2
	Sdl2InputProcessor inputProcessor{};
	Sdl2Initializer hardwareInitializer{};
#endif //_USE_SDL2

#ifdef _USE_GLFW
	GlfwInitializer hardwareInitializer{};
#endif //_UES_GLFW

	hardwareInitializer.init("PuppetBox", 1024, 768);
	if (!hardwareInitializer.hadError())
	{
#ifdef _USE_GLFW
		GlfwInputProcessor inputProcessor{ hardwareInitializer.getWindow() };
		inputProcessor.init();
#endif //_USE_GLFW

		Engine engine{ &hardwareInitializer, &inputProcessor };

		if (engine.init())
		{
			engine.run();
		}
		else
		{
			return 1;
		}
	}
	else
	{
		return 1;
	}

	return 0;
}