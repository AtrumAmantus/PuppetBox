#pragma once

#include "DataStructures.h"

namespace PB
{
    class IRenderWindow
    {
    public:
        virtual vec2 getDimensions() const = 0;
    };
}