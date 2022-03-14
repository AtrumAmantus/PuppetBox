#include "puppetbox/AbstractInputReader.h"

#define KEY_IS_DOWN    1

namespace PB
{

    const bool KeyboardInput::isDown(std::uint16_t key) const
    {
        return keyState[key] == KEY_IS_DOWN;
    }

    std::vector<std::uint8_t> KeyboardInput::areDown(std::uint8_t* btns, std::uint32_t btnCount) const
    {
        std::vector<std::uint8_t> downKeys{};

        for (std::uint32_t i = 0; i < btnCount; ++i)
        {
            if (isDown(btns[i]))
            {
                downKeys.push_back(btns[i]);
            }
        }

        return downKeys;
    }

    const bool KeyboardInput::isUp(std::uint16_t key) const
    {
        return keyState[key] != KEY_IS_DOWN;
    }

    std::vector<std::uint8_t> KeyboardInput::areUp(std::uint8_t* btns, std::uint32_t btnCount) const
    {
        std::vector<std::uint8_t> upKeys{};

        for (std::uint32_t i = 0; i < btnCount; ++i)
        {
            if (isPressed(btns[i]))
            {
                upKeys.push_back(btns[i]);
            }
        }

        return upKeys;
    }

    const bool KeyboardInput::isPressed(std::uint16_t key) const
    {
        return keyState[key] == KEY_IS_DOWN && previousKeyState[key] != KEY_IS_DOWN;
    }

    std::vector<std::uint8_t> KeyboardInput::arePressed(std::uint8_t* btns, std::uint32_t btnCount) const
    {
        std::vector<std::uint8_t> pressedKeys{};

        for (std::uint32_t i = 0; i < btnCount; ++i)
        {
            if (isPressed(btns[i]))
            {
                pressedKeys.push_back(btns[i]);
            }
        }

        return pressedKeys;
    }

    const bool KeyboardInput::isReleased(std::uint16_t key) const
    {
        return keyState[key] != KEY_IS_DOWN && previousKeyState[key] == KEY_IS_DOWN;
    }

    std::vector<std::uint8_t> KeyboardInput::areReleased(std::uint8_t* btns, std::uint32_t btnCount) const
    {
        std::vector<std::uint8_t> releasedKeys{};

        for (std::uint32_t i = 0; i < btnCount; ++i)
        {
            if (isReleased(btns[i]))
            {
                releasedKeys.push_back(btns[i]);
            }
        }

        return releasedKeys;
    }

    const bool MouseInput::isDown(std::uint8_t btn) const
    {
        return mouseState[btn] == KEY_IS_DOWN;
    }

    const bool MouseInput::isUp(std::uint8_t btn) const
    {
        return mouseState[btn] != KEY_IS_DOWN;
    }

    const bool MouseInput::isPressed(std::uint8_t btn) const
    {
        return mouseState[btn] == KEY_IS_DOWN && previousMouseState[btn] != KEY_IS_DOWN;
    }

    const bool MouseInput::isReleased(std::uint8_t btn) const
    {
        return mouseState[btn] != KEY_IS_DOWN && previousMouseState[btn] == KEY_IS_DOWN;
    }


    const bool WindowInput::isClosed() const
    {
        return windowClose;
    }
}