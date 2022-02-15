#pragma once

#include <string>
#include <unordered_map>

#include <puppetbox/AbstractInputReader.h>

class Controls
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
    Controls(PB::AbstractInputReader* inputReader) : inputReader_(inputReader) {};

    bool isCommandActive(Command commandName)
    {
        bool isStarted = false;

        if (bindings_.find(commandName) != bindings_.end())
        {
            auto vec = bindings_.at(commandName);

            for (std::size_t i = 0; !isStarted && i < vec.size(); ++i)
            {
                isStarted = inputReader_->keyboard.isDown(vec.at(i));
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
                isStarted = inputReader_->keyboard.isReleased(vec.at(i));
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

    float getZoomSpeed() const
    {
        return zoomSpeed_;
    };

    void setPanSpeed(float moveSpeed)
    {
        panSpeed_ = moveSpeed;
    };

    float getPanSpeed() const
    {
        return panSpeed_;
    };

private:
    PB::AbstractInputReader* inputReader_ = nullptr;
    std::unordered_map<Command, std::vector<std::int8_t>> bindings_{};
private:
    float zoomSpeed_ = 1.0f;
    float panSpeed_ = 1.0f;
};