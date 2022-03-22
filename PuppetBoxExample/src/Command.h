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
        EQUIP_SLOT_1,
        EQUIP_SLOT_2,
        EQUIP_SLOT_3,
        EQUIP_SLOT_4,
        EQUIP_SLOT_5,
        QUIT,
    };
};

struct CommandState
{
    Command::Command command = Command::NOTHING;
    bool activated = false;
};