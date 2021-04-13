#include "Engine.h"
#include "Logger.h"

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
	while (!inputProcessor_->window.windowClose)
	{
		processInput();

		hardwareInitializer_->postLoopCommands();
	}
}

void Engine::processInput()
{
	inputProcessor_->loadCurrentState();

	if (inputProcessor_->keyboard.isPressed(KEY_W))
	{
		LOGGER_DEBUG("'w' key is pressed");
	}

	if (inputProcessor_->mouse.deltaX != 0 || inputProcessor_->mouse.deltaY != 0)
	{
		LOGGER_DEBUG(std::to_string(inputProcessor_->mouse.deltaX) + ", " + std::to_string(inputProcessor_->mouse.deltaY));
	}
}