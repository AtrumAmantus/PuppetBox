#include "Engine.h"
#include "Logger.h"

bool Engine::init()
{

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
	while (!inputProcessor_->window.windowClose)
	{
		processInput();

		if (inputProcessor_->mouse.deltaX != 0 || inputProcessor_->mouse.deltaY != 0)
		{
			LOGGER_DEBUG(std::to_string(inputProcessor_->mouse.deltaX) + ", " + std::to_string(inputProcessor_->mouse.deltaY));
		}
	}
}

void Engine::processInput()
{
	inputProcessor_->loadCurrentState();
}