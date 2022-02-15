#pragma once

#include <cstdint>
#include <vector>

#include "KeyCode.h"
#include "TypeDef.h"

#define MAX_KEY_CODES        120
#define MAX_MOUSE_CODES        8

namespace PB
{
    /**
    * \brief Helper class for abstracting keyboard input state checks.
    */
    class PUPPET_BOX_API KeyboardInput
    {
    public:
        std::uint8_t previousKeyState[KEY_LAST + 1]{};            //Stores previous state, indexed by key code
        std::uint8_t keyState[KEY_LAST + 1]{};                    //Stores state, indexed by key code
    public:
        /**
        * \brief Used to determine if a key is currently down.
        *
        * \return True if the key is currently down, False otheriwse.
        */
        bool isDown(std::uint16_t key) const;

        std::vector<std::uint8_t> areDown(std::uint8_t* btns, std::uint32_t btnCount) const;

        /**
        * \brief Used to determine if a key is currently up.
        *
        * \return True if the key is currently up, False otheriwse.
        */
        bool isUp(std::uint16_t key) const;

        std::vector<std::uint8_t> areUp(std::uint8_t* btns, std::uint32_t btnCount) const;

        /**
        * \brief Used to determine if a key was just pushed down.
        *
        * \return True if the key just switched from being up to being down, False otheriwse.
        */
        bool isPressed(std::uint16_t key) const;

        std::vector<std::uint8_t> arePressed(std::uint8_t* btns, std::uint32_t btnCount) const;

        /**
        * \brief Used to determine if a key was just released.
        *
        * \return True if the key just switched from being down to being up, False otheriwse.
        */
        bool isReleased(std::uint16_t key) const;

        std::vector<std::uint8_t> areReleased(std::uint8_t* btns, std::uint32_t btnCount) const;
    };

    /**
    * \brief Helper class for abstracting mouse input state checks.
    */
    class PUPPET_BOX_API MouseInput
    {
    public:
        std::uint8_t previousMouseState[MAX_MOUSE_CODES]{};
        std::uint8_t mouseState[MAX_MOUSE_CODES]{};
        std::uint32_t x = 0;
        std::uint32_t y = 0;
        std::int32_t deltaX = 0;
        std::int32_t deltaY = 0;
        // Mouse wheel vector, +/- 1
        std::int32_t wheelXDir = 0;
        std::int32_t wheelYDir = 0;
    public:
        /**
        * \brief Used to determine if a mouse button is currently down.
        *
        * \return True if the mouse button is currently down, False otheriwse.
        */
        bool isDown(std::uint8_t btn) const;

        /**
        * \brief Used to determine if a mouse button is currently up.
        *
        * \return True if the mouse button is currently up, False otheriwse.
        */
        bool isUp(std::uint8_t btn) const;

        /**
        * \brief Used to determine if a mouse button was just pushed down.
        *
        * \return True if the mouse button just switched from being up to being down, False otheriwse.
        */
        bool isPressed(std::uint8_t btn) const;

        /**
        * \brief Used to determine if a mouse button was just released.
        *
        * \return True if the mouse button just switched from being down to being up, False otheriwse.
        */
        bool isReleased(std::uint8_t btn) const;
    };

    /**
    * \brief Helper class for abstracting window input state checks.
    */
    class PUPPET_BOX_API WindowInput
    {
    public:
        bool windowClose = false;
        std::uint32_t width = 0;
        std::uint32_t height = 0;
        std::uint32_t newWidth = 0;
        std::uint32_t newHeight = 0;
    public:
        /**
        * \brief Used to determine if the window was closed (x button pressed).
        *
        * \return True if the window was closed (x button pressed), False otheriwse.
        */
        bool isClosed() const;
    };

    /**
    * \brief Common interface to be used by app logic for checking and loading hardware input state.
    */
    class PUPPET_BOX_API AbstractInputReader
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
