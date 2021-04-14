#pragma once

#include "KeyCode.h"

#define MAX_KEY_CODES		120
#define MAX_MOUSE_CODES		8

class KeyboardInput
{
public:
	uint8_t previousKeyState[KEY_LAST + 1]{};			//Stores previous state, indexed by key code
	uint8_t keyState[KEY_LAST + 1]{};					//Stores state, indexed by key code
public:
	bool isDown(int key) const
	{
		return keyState[key] == KEY_IS_DOWN;
	};
	bool isUp(int key) const
	{
		return keyState[key] != KEY_IS_DOWN;
	};
	bool isPressed(int key) const
	{
		return keyState[key] == KEY_IS_DOWN && previousKeyState[key] != KEY_IS_DOWN;
	};
	bool isReleased(int key) const
	{
		return keyState[key] != KEY_IS_DOWN && previousKeyState[key] == KEY_IS_DOWN;
	};
};

class MouseInput
{
public:
	uint8_t previousMouseState[MAX_MOUSE_CODES]{};
	uint8_t mouseState[MAX_MOUSE_CODES]{};
	uint32_t x = 0;
	uint32_t y = 0;
	int32_t deltaX = 0;
	int32_t deltaY = 0;
public:
	bool isDown(int btn) const
	{
		return mouseState[btn] == KEY_IS_DOWN;
	};
	bool isUp(int btn) const
	{
		return mouseState[btn] != KEY_IS_DOWN;
	};
	bool isPressed(int btn) const
	{
		return mouseState[btn] == KEY_IS_DOWN && previousMouseState[btn] != KEY_IS_DOWN;
	};
	bool isReleased(int btn) const
	{
		return mouseState[btn] != KEY_IS_DOWN && previousMouseState[btn] == KEY_IS_DOWN;
	};
};

class WindowInput
{
public:
	bool windowClose = false;
	uint32_t width = 0;
	uint32_t height = 0;
	uint32_t newWidth = 0;
	uint32_t newHeight = 0;
public:
	bool isClosed()
	{
		return windowClose;
	};
};

class AbstractInputProcessor
{
public:
	KeyboardInput keyboard;
	MouseInput mouse;
	WindowInput window;
public:
	virtual void loadCurrentState() = 0;
};