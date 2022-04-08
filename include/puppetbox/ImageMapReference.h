#pragma once

#include <cstdint>

#include "TypeDef.h"

struct PUPPET_BOX_API ImageMapReference
{
    std::uint32_t imageMapId = 0;
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    bool requiresAlphaBlending = false;
};