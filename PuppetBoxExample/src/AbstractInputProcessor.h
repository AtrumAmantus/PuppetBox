#pragma once

#include <cstdint>
#include <iostream>
#include <functional>
#include <memory>
#include <vector>

#include <PuppetBox.h>
#include <puppetbox/AbstractInputReader.h>
#include <puppetbox/Camera.h>

#include "Command.h"
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
            const std::shared_ptr<PB::AbstractInputReader>& inputReader)
            : userInput_(userInput), inputActions_(inputActions), inputReader_(inputReader)
    {

    };

    /**
     * \brief Processes user input, invoking game events of various forms.  Invokes static processing
     * and ascii input prior to executing {\link AbstractInputProcessor} implementation specific processing.
     */
    void processInput()
    {
        staticProcessInputs();

        if (userInput_.isReading())
        {
            if (
                    inputReader_->keyboard.isReleased(KEY_ENTER)
                    || inputReader_->keyboard.isReleased(KEY_KP_ENTER))
            {
                userInput_.stopReading();
            }
            else
            {
                // TODO: Still need to account for shift values
                bool shiftDown = inputReader_->keyboard.isDown(KEY_LEFT_SHIFT)
                                 || inputReader_->keyboard.isDown(KEY_RIGHT_SHIFT);

                // TODO: This can probably be better
                std::vector<std::uint8_t> releasedKeys = inputReader_->keyboard
                        .areReleased(Constants::Input::inputKeys, Constants::Input::inputKeyCount);

                for (auto k: releasedKeys)
                {
                    userInput_.append(PB::GetCharFromCode(k, shiftDown));
                }

                if (inputReader_->keyboard.isReleased(KEY_BACKSPACE))
                {
                    userInput_.removeBeforeCursor();
                }
            }
        }
        else
        {
            if (
                    inputReader_->keyboard.isReleased(KEY_ENTER)
                    || inputReader_->keyboard.isReleased(KEY_KP_ENTER))
            {
                userInput_.startReading();
            }
            else
            {
                if (inputActions_.isCommandReleased(Command::QUIT))
                {
                    PB::PublishEvent(Event::Topic::TERMINATE_TOPIC, nullptr);
                }

                if (
                        inputReader_->window.newWidth != 0
                        || inputReader_->window.newHeight != 0)
                {
                    std::cout << "Window Size: "
                              << std::to_string(inputReader_->window.newWidth)
                              << "x"
                              << std::to_string(inputReader_->window.newHeight)
                              << std::endl;
                }

                processInputs();
            }
        }
    };

    /**
     * \brief Implementation defined input processing that should always occur.
     */
    virtual void staticProcessInputs()
    {

    };

    /**
     * \brief Defined by specific implementing class to do additional unique processing.
     */
    virtual void processInputs() = 0;

protected:
    /**
     * \brief Provides access to the {\link UserInput} object.
     *
     * \return The {\link UserInput} object instance.
     */
    UserInput& userInput() const
    {
        return userInput_;
    };

    /**
     * \brief Provides access to the {\link InputActions} object.
     *
     * \return The {\link InputActions} object instance.
     */
    const InputActions& inputActions() const
    {
        return inputActions_;
    };

    /**
     * \brief Provides access to the {\link PB::AbstractInputReader} object.
     *
     * \return The {\link PB::AbstractInputReader} object instance.
     */
    const std::shared_ptr<PB::AbstractInputReader>& input() const
    {
        return inputReader_;
    };

private:
    UserInput& userInput_;
    const InputActions& inputActions_;
    const std::shared_ptr<PB::AbstractInputReader> inputReader_;
};