#pragma once

#include <cstdint>
#include <string>

namespace Constants
{
    namespace Font
    {
        extern std::string kMochiyPop;
    }

    namespace Animation
    {
        namespace Pack
        {
           extern std::string kBasicHuman;
        }

        extern std::string kWalk;
        extern std::string kIdle0;
    }

    namespace Input
    {
        extern std::uint32_t inputKeyCount;

        extern std::uint8_t inputKeys[63];
    }

    namespace Behavior
    {
        enum Behavior {
            NONE,
            AIM,
            WANDER
        };
    }
}