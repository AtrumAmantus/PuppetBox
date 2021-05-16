#include "puppetbox/AbstractInputProcessor.h"

#define KEY_IS_DOWN	1

namespace PB
{

	bool KeyboardInput::isDown(std::uint16_t key) const
	{
		return keyState[key] == KEY_IS_DOWN;
	}

	bool KeyboardInput::isUp(std::uint16_t key) const
	{
		return keyState[key] != KEY_IS_DOWN;
	}

	bool KeyboardInput::isPressed(std::uint16_t key) const
	{
		return keyState[key] == KEY_IS_DOWN && previousKeyState[key] != KEY_IS_DOWN;
	}

	bool KeyboardInput::isReleased(std::uint16_t key) const
	{
		return keyState[key] != KEY_IS_DOWN && previousKeyState[key] == KEY_IS_DOWN;
	}


	bool MouseInput::isDown(std::uint8_t btn) const
	{
		return mouseState[btn] == KEY_IS_DOWN;
	}

	bool MouseInput::isUp(std::uint8_t btn) const
	{
		return mouseState[btn] != KEY_IS_DOWN;
	}

	bool MouseInput::isPressed(std::uint8_t btn) const
	{
		return mouseState[btn] == KEY_IS_DOWN && previousMouseState[btn] != KEY_IS_DOWN;
	}

	bool MouseInput::isReleased(std::uint8_t btn) const
	{
		return mouseState[btn] != KEY_IS_DOWN && previousMouseState[btn] == KEY_IS_DOWN;
	}


	bool WindowInput::isClosed() const
	{
		return windowClose;
	}
}