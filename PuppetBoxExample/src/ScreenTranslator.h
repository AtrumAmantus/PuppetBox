#pragma once

#include <puppetbox/DataStructures.h>

struct Cursor
{
    PB::uivec2 renderCoords{};
    PB::vec3 worldCoords{};
    PB::vec3 worldVector{};
};

struct ScreenTranslator
{
    Cursor cursor{};
};