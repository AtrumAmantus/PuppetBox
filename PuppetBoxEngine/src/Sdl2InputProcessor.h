#pragma once

#include <sdl2/SDL.h>

#include "puppetbox/AbstractInputProcessor.h"
#include "puppetbox/KeyCode.h"
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
            for (std::uint32_t i = 0; i < KEY_LAST + 1; ++i)
            {
                keyboard.previousKeyState[i] = keyboard.keyState[i];
            }

            for (std::uint32_t i = 0; i < BTN_LAST + 1; ++i)
            {
                mouse.previousMouseState[i] = mouse.mouseState[i];
            }

            mouse.deltaX = 0;
            mouse.deltaY = 0;
            mouse.wheelXDir = 0;
            mouse.wheelYDir = 0;
            window.newHeight = 0;
            window.newWidth = 0;

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
                    case SDL_MOUSEBUTTONDOWN:
                        mouse.mouseState[sdlEvent.button.button] = 1;
                        break;
                    case SDL_MOUSEBUTTONUP:
                        mouse.mouseState[sdlEvent.button.button] = 0;
                        break;
                    case SDL_MOUSEWHEEL:
                    {
                        std::int32_t invert = 1 - (sdlEvent.wheel.direction * 2);
                        mouse.wheelXDir = sdlEvent.wheel.x * invert;
                        mouse.wheelYDir = sdlEvent.wheel.y * invert;
                        break;
                    }
                    case SDL_MOUSEMOTION:
                        mouse.x = sdlEvent.motion.x;
                        mouse.y = sdlEvent.motion.y;
                        mouse.deltaX = sdlEvent.motion.xrel;
                        mouse.deltaY = sdlEvent.motion.yrel;
                        break;
                    case SDL_WINDOWEVENT:
                        handleWindowEvent(sdlEvent);
                        break;
                    default:
                        break;
                }
            }
        };
    private:
        void handleWindowEvent(SDL_Event sdlEvent)
        {
            switch (sdlEvent.window.event)
            {
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                    window.newWidth = sdlEvent.window.data1;
                    window.newHeight = sdlEvent.window.data2;
                    break;
                    // This is a redundant event that only appears if it's the result of external forces.
                    //case SDL_WINDOWEVENT_RESIZED:
                    //	break;
                default:
                    break;
            }
        }
    };
}