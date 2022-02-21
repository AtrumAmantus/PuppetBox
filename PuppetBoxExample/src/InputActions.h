#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <puppetbox/AbstractInputReader.h>

class InputActions
{
public:
    enum Command {
        CAMERA_UP,
        CAMERA_DOWN,
        CAMERA_LEFT,
        CAMERA_RIGHT,
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        QUIT,
    };
public:
    InputActions() = default;

    InputActions(std::shared_ptr<PB::AbstractInputReader>& inputReader) : inputReader_(inputReader) {};

    bool isCommandActive(Command commandName) const
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

    bool isCommandReleased(Command commandName) const
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

    void registerCommand(Command commandName, std::int8_t key)
    {
        if (bindings_.find(commandName) != bindings_.end())
        {
            bindings_.at(commandName).push_back(key);
        }
        else
        {
            bindings_.insert(
                    std::pair<Command, std::vector<std::int8_t>>{commandName, std::vector<std::int8_t>{key}}
            );
        }
    };

private:
    std::shared_ptr<PB::AbstractInputReader> inputReader_{nullptr};
    std::unordered_map<Command, std::vector<std::int8_t>> bindings_{};
};