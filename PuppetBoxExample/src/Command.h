#pragma once

namespace Command
{
    enum Command
    {
        NOTHING,
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        CAMERA_UP,
        CAMERA_DOWN,
        CAMERA_LEFT,
        CAMERA_RIGHT,
        QUIT,
    };
};

struct CommandState
{
    Command::Command command = Command::NOTHING;
    bool activated = false;
};