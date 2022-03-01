#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <puppetbox/AbstractInputReader.h>

#include "Command.h"

class InputActions
{
public:
    InputActions() = default;

    InputActions(std::shared_ptr<PB::AbstractInputReader>& inputReader) : inputReader_(inputReader) {};

    bool isCommandActivated(Command::Command commandName) const
    {
        bool isActivated = false;

        if (bindings_.find(commandName) != bindings_.end())
        {
            auto vec = bindings_.at(commandName);

            for (std::size_t i = 0; !isActivated && i < vec.size(); ++i)
            {
                isActivated = inputReader_->keyboard.isPressed(vec.at(i));
            }
        }

        return isActivated;
    };

    bool isCommandActive(Command::Command commandName) const
    {
        bool isActive = false;

        if (bindings_.find(commandName) != bindings_.end())
        {
            auto vec = bindings_.at(commandName);

            for (std::size_t i = 0; !isActive && i < vec.size(); ++i)
            {
                isActive = inputReader_->keyboard.isDown(vec.at(i));
            }
        }

        return isActive;
    };

    bool isCommandReleased(Command::Command commandName) const
    {
        bool isReleased = false;

        if (bindings_.find(commandName) != bindings_.end())
        {
            auto vec = bindings_.at(commandName);

            for (std::size_t i = 0; !isReleased && i < vec.size(); ++i)
            {
                isReleased = inputReader_->keyboard.isReleased(vec.at(i));
            }
        }

        return isReleased;
    };

    void registerCommand(Command::Command commandName, std::int8_t key)
    {
        if (bindings_.find(commandName) != bindings_.end())
        {
            bindings_.at(commandName).push_back(key);
        }
        else
        {
            bindings_.insert(
                    std::pair<Command::Command, std::vector<std::int8_t>>{commandName, std::vector<std::int8_t>{key}}
            );
        }
    };

private:
    std::shared_ptr<PB::AbstractInputReader> inputReader_{nullptr};
    std::unordered_map<Command::Command, std::vector<std::int8_t>> bindings_{};
};