#pragma once

#include "KeyCode.h"

#define MAX_KEY_CODES		120
#define MAX_MOUSE_CODES		8

namespace PB
{
	/**
	* \brief Helper class for abstracting keyboard input state checks.
	*/
	class KeyboardInput
	{
	public:
		uint8_t previousKeyState[KEY_LAST + 1]{};			//Stores previous state, indexed by key code
		uint8_t keyState[KEY_LAST + 1]{};					//Stores state, indexed by key code
	public:
		/**
		* \brief Used to determine if a key is currently down.
		* 
		* \return True if the key is currently down, False otheriwse.
		*/
		bool isDown(int key) const
		{
			return keyState[key] == KEY_IS_DOWN;
		};

		/**
		* \brief Used to determine if a key is currently up.
		*
		* \return True if the key is currently up, False otheriwse.
		*/
		bool isUp(int key) const
		{
			return keyState[key] != KEY_IS_DOWN;
		};

		/**
		* \brief Used to determine if a key was just pushed down.
		*
		* \return True if the key just switched from being up to being down, False otheriwse.
		*/
		bool isPressed(int key) const
		{
			return keyState[key] == KEY_IS_DOWN && previousKeyState[key] != KEY_IS_DOWN;
		};

		/**
		* \brief Used to determine if a key was just released.
		*
		* \return True if the key just switched from being down to being up, False otheriwse.
		*/
		bool isReleased(int key) const
		{
			return keyState[key] != KEY_IS_DOWN && previousKeyState[key] == KEY_IS_DOWN;
		};
	};

	/**
	* \brief Helper class for abstracting mouse input state checks.
	*/
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
		/**
		* \brief Used to determine if a mouse button is currently down.
		*
		* \return True if the mouse button is currently down, False otheriwse.
		*/
		bool isDown(int btn) const
		{
			return mouseState[btn] == KEY_IS_DOWN;
		};

		/**
		* \brief Used to determine if a mouse button is currently up.
		*
		* \return True if the mouse button is currently up, False otheriwse.
		*/
		bool isUp(int btn) const
		{
			return mouseState[btn] != KEY_IS_DOWN;
		};

		/**
		* \brief Used to determine if a mouse button was just pushed down.
		*
		* \return True if the mouse button just switched from being up to being down, False otheriwse.
		*/
		bool isPressed(int btn) const
		{
			return mouseState[btn] == KEY_IS_DOWN && previousMouseState[btn] != KEY_IS_DOWN;
		};

		/**
		* \brief Used to determine if a mouse button was just released.
		*
		* \return True if the mouse button just switched from being down to being up, False otheriwse.
		*/
		bool isReleased(int btn) const
		{
			return mouseState[btn] != KEY_IS_DOWN && previousMouseState[btn] == KEY_IS_DOWN;
		};
	};

	/**
	* \brief Helper class for abstracting window input state checks.
	*/
	class WindowInput
	{
	public:
		bool windowClose = false;
		uint32_t width = 0;
		uint32_t height = 0;
		uint32_t newWidth = 0;
		uint32_t newHeight = 0;
	public:
		/**
		* \brief Used to determine if the window was closed (x button pressed).
		*
		* \return True if the window was closed (x button pressed), False otheriwse.
		*/
		bool isClosed()
		{
			return windowClose;
		};
	};

	/**
	* \brief Common interface to be used by app logic for checking and loading hardware input state.
	*/
	class AbstractInputProcessor
	{
	public:
		KeyboardInput keyboard;
		MouseInput mouse;
		WindowInput window;
	public:
		/**
		* \brief Loads the current hardware input state.
		*/
		virtual void loadCurrentState() = 0;
	};
}
