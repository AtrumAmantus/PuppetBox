#include "pch.h"

#include "../include/puppetbox/AbstractInputProcessor.h"

#define KEY_IS_DOWN	1

namespace PB
{

	bool KeyboardInput::isDown(int key) const
	{
		return keyState[key] == KEY_IS_DOWN;
	}

	bool KeyboardInput::isUp(int key) const
	{
		return keyState[key] != KEY_IS_DOWN;
	}

	bool KeyboardInput::isPressed(int key) const
	{
		return keyState[key] == KEY_IS_DOWN && previousKeyState[key] != KEY_IS_DOWN;
	}

	bool KeyboardInput::isReleased(int key) const
	{
		return keyState[key] != KEY_IS_DOWN && previousKeyState[key] == KEY_IS_DOWN;
	}


	bool MouseInput::isDown(int btn) const
	{
		return mouseState[btn] == KEY_IS_DOWN;
	}

	bool MouseInput::isUp(int btn) const
	{
		return mouseState[btn] != KEY_IS_DOWN;
	}

	bool MouseInput::isPressed(int btn) const
	{
		return mouseState[btn] == KEY_IS_DOWN && previousMouseState[btn] != KEY_IS_DOWN;
	}

	bool MouseInput::isReleased(int btn) const
	{
		return mouseState[btn] != KEY_IS_DOWN && previousMouseState[btn] == KEY_IS_DOWN;
	}


	bool WindowInput::isClosed()
	{
		return windowClose;
	}
}