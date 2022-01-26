#pragma once

#include <string>
#include <unordered_map>

#include <puppetbox/AbstractInputProcessor.h>

class Controls
{
public:
    enum Command {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        QUIT,
    };
public:
    Controls(PB::AbstractInputProcessor* input) : input_(input) {};

    bool isCommandActive(Command commandName)
    {
        bool isStarted = false;

        if (bindings_.find(commandName) != bindings_.end())
        {
            auto vec = bindings_.at(commandName);

            for (std::size_t i = 0; !isStarted && i < vec.size(); ++i)
            {
                isStarted = input_->keyboard.isDown(vec.at(i));
            }
        }

        return isStarted;
    };

    bool isCommandReleased(Command commandName)
    {
        bool isStarted = false;

        if (bindings_.find(commandName) != bindings_.end())
        {
            auto vec = bindings_.at(commandName);

            for (std::size_t i = 0; !isStarted && i < vec.size(); ++i)
            {
                isStarted = input_->keyboard.isReleased(vec.at(i));
            }
        }

        return isStarted;
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

    void setZoomSpeed(float zoomSpeed)
    {
        zoomSpeed_ = zoomSpeed;
    };

    float getZoomSpeed()
    {
        return zoomSpeed_;
    };

    void setMoveSpeed(float moveSpeed)
    {
        moveSpeed_ = moveSpeed;
    };

    float getMoveSpeed()
    {
        return moveSpeed_;
    };

private:
    PB::AbstractInputProcessor* input_;
    std::unordered_map<Command, std::vector<std::int8_t>> bindings_;
private:
    float zoomSpeed_ = 1.0f;
    float moveSpeed_ = 1.0f;
};