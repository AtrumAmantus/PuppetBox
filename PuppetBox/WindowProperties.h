#ifdef _USE_GLFW

#pragma once

#include <stdint.h>

struct Mouse
{
	uint32_t x = 0;
	uint32_t y = 0;
	uint32_t lastX = 0;
	uint32_t lastY = 0;
	uint32_t deltaX = 0;
	uint32_t deltaY = 0;
	float sensitivity = 1.0f;
	uint32_t scrollDirection = 0;
};

struct WindowProperties
{
	bool isOrthoActive = false;
	bool showConsole = false;
	uint32_t width = 0;
	uint32_t height = 0;
	uint32_t newWidth = 0;
	uint32_t newHeight = 0;
	Mouse mouse;
	void resizeWindow(int w, int h)
	{
		width = w;
		height = h;
		newWidth = w;
		newHeight = h;
		mouse.x = w / 2;
		mouse.y = h / 2;
		mouse.lastX = mouse.x;
		mouse.lastY = mouse.y;
		resetMouseInputs();
	};
	void resetInputs()
	{
		newWidth = 0;
		newHeight = 0;
		resetMouseInputs();
	}
	void resetMouseInputs()
	{
		mouse.deltaX = 0;
		mouse.deltaY = 0;
		mouse.scrollDirection = 0;
	};
	void updateMouse(uint32_t x, uint32_t y)
	{
		// Set new last position
		mouse.lastX = mouse.x;
		mouse.lastY = mouse.y;
		// Set new current position
		mouse.x = x;
		mouse.y = y;
		// Set offset (for in app calculations)
		mouse.deltaX = (x - mouse.lastX);
		mouse.deltaY = (y - mouse.lastY);
	};
};

#endif //_USEGLFW
