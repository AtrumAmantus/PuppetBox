#ifdef _USE_SDL2

#pragma once

#include <sdl2/SDL.h>

#include "AbstractInputProcessor.h"
#include "KeyCode.h"
#include "Logger.h"

namespace PB
{
	/**
	* \brief SDL2 specific InputProcessor implementation for reading hardware input.
	*/
	class Sdl2InputProcessor : public AbstractInputProcessor
	{
	public:
		/**
		* \brief Loads the current hardware input state, storing the previous state for doing later state checks.
		*/
		void loadCurrentState() override
		{
			for (uint32_t i = 0; i < KEY_LAST + 1; ++i)
			{
				keyboard.previousKeyState[i] = keyboard.keyState[i];
			}

			mouse.deltaX = 0;
			mouse.deltaY = 0;

			SDL_Event sdlEvent;

			while (SDL_PollEvent(&sdlEvent))
			{
				switch (sdlEvent.type)
				{
				case SDL_QUIT:
					window.windowClose = true;
					break;
				case SDL_KEYDOWN:
					keyboard.keyState[sdlEvent.key.keysym.scancode] = 1;
					break;
				case SDL_KEYUP:
					keyboard.keyState[sdlEvent.key.keysym.scancode] = 0;
					break;
				case SDL_MOUSEMOTION:
					mouse.x = sdlEvent.motion.x;
					mouse.y = sdlEvent.motion.y;
					mouse.deltaX = sdlEvent.motion.xrel;
					mouse.deltaY = sdlEvent.motion.yrel;
					break;
				default:
					break;
				}
			}

			if (keyboard.isPressed(KEY_ESCAPE))
			{
				window.windowClose = true;
			}
		};
	};
}

#endif //_USE_SDL2