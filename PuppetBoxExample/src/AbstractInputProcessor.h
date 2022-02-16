#pragma once

#include <iostream>
#include <functional>

#include <puppetbox/AbstractInputReader.h>
#include <puppetbox/Camera.h>

#include "Constants.h"
#include "EventDef.h"
#include "InputActions.h"
#include "UserInput.h"

class AbstractInputProcessor
{
public:
    AbstractInputProcessor(
            UserInput& userInput,
            const InputActions& inputActions,
            const PB::AbstractInputReader& inputReader)
            : userInput_(userInput), inputActions_(inputActions), inputReader_(inputReader)
    {

    };

    void processInput()
    {
        staticProcessInputs();

        if (userInput_.isReading())
        {
            if (
                    inputReader_.keyboard.isReleased(KEY_ENTER)
                    || inputReader_.keyboard.isReleased(KEY_KP_ENTER))
            {
                userInput_.stopReading();
            }
            else
            {
                // TODO: Still need to account for shift values
                bool shiftDown = inputReader_.keyboard.isDown(KEY_LEFT_SHIFT)
                                 || inputReader_.keyboard.isDown(KEY_RIGHT_SHIFT);

                // TODO: This can probably be better
                std::vector<std::uint8_t> releasedKeys = inputReader_.keyboard
                        .areReleased(Constants::Input::inputKeys, Constants::Input::inputKeyCount);

                for (auto k: releasedKeys)
                {
                    userInput_.append(PB::GetCharFromCode(k, shiftDown));
                }

                if (inputReader_.keyboard.isReleased(KEY_BACKSPACE))
                {
                    userInput_.removeBeforeCursor();
                }
            }
        }
        else
        {
            if (
                    inputReader_.keyboard.isReleased(KEY_ENTER)
                    || inputReader_.keyboard.isReleased(KEY_KP_ENTER))
            {
                userInput_.startReading();
            }
            else
            {
                if (inputActions_.isCommandReleased(InputActions::QUIT))
                {
                    PB::PublishEvent(Event::Topic::TERMINATE_TOPIC, nullptr);
                }

                if (
                        inputReader_.window.newWidth != 0
                        || inputReader_.window.newHeight != 0)
                {
                    std::cout << "Window Size: "
                              << std::to_string(inputReader_.window.newWidth)
                              << "x"
                              << std::to_string(inputReader_.window.newHeight)
                              << std::endl;
                }

                processInputs();
            }
        }
    };

    virtual void staticProcessInputs()
    {

    };

    virtual void processInputs() = 0;

protected:
    UserInput& userInput() const
    {
        return userInput_;
    };

    const InputActions& inputActions() const
    {
        return inputActions_;
    };

    const PB::AbstractInputReader& input() const
    {
        return inputReader_;
    };

private:
    UserInput& userInput_;
    const InputActions& inputActions_;
    const PB::AbstractInputReader& inputReader_;
};