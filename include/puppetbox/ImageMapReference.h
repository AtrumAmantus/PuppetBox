#pragma once

#include <cstdint>

struct ImageMapReference
{
    std::uint32_t imageMapId = 0;
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    bool requiresAlphaBlending = false;
};