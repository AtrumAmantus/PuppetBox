#include "pch.h"

#include "Engine.h"
#include "Logger.h"

namespace PB
{
	bool Engine::init()
	{
		return true;
	}

	void Engine::run()
	{
		loop();
	}

	void Engine::shutdown()
	{

	}

	void Engine::loop()
	{
		while (!inputProcessor_.window.windowClose)
		{
			gfxApi_.preLoopCommands();

			processInput();

			hardwareInitializer_.postLoopCommands();
		}
	}

	void Engine::processInput()
	{
		inputProcessor_.loadCurrentState();

		if (inputProcessor_.keyboard.isPressed(KEY_ESCAPE))
		{
			inputProcessor_.window.windowClose = true;
		}

		if (inputProcessor_.keyboard.isPressed(KEY_W))
		{
			LOGGER_DEBUG("'w' key is pressed");
		}

		if (inputProcessor_.mouse.deltaX != 0 || inputProcessor_.mouse.deltaY != 0)
		{
			LOGGER_DEBUG("Mouse Coords: " + std::to_string(inputProcessor_.mouse.deltaX) + ", " + std::to_string(inputProcessor_.mouse.deltaY));
		}

		if (inputProcessor_.window.newWidth != 0 || inputProcessor_.window.newHeight != 0)
		{
			LOGGER_DEBUG("Window Size: " + std::to_string(inputProcessor_.window.newWidth) + "x" + std::to_string(inputProcessor_.window.newHeight));
		}
	}
}
