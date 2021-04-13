//#define _USE_SDL2
#define _USE_GLFW

#include <iostream>

#include "Engine.h"
#include "GlfwInitializer.h"
#include "GlfwInputProcessor.h"
#include "Sdl2Initializer.h"
#include "Sdl2InputProcessor.h"

//int sdl2_EventHandler(void* userdata, SDL_Event* event)
//{
//	switch (event->type)
//	{
//	case SDL_WINDOWEVENT:
//	{
//		int x = 0;
//		break;
//	}
//	default:
//		return 0;
//	}
//	return 1;
//}

// Entry point is in SDL2
int main(int argc, char** argv)
{
	PB::Init();

	//SDL_AddEventWatch(sdl2_EventHandler, nullptr);
	//Sdl2InputProcessor inputProcessor{};
	//Sdl2Initializer hardwareInitializer{};

	GlfwInitializer hardwareInitializer{};

	hardwareInitializer.init("PuppetBox", 1024, 768);
	if (!hardwareInitializer.hadError())
	{
		GlfwInputProcessor inputProcessor{ hardwareInitializer.getWindow() };

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