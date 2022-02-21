#pragma once

#include "DataStructures.h"

namespace PB
{
    class RenderWindow
    {
    public:
        const std::uint32_t* width = nullptr;
        const std::uint32_t* height = nullptr;
        const std::uint32_t* depth = nullptr;
    };
}