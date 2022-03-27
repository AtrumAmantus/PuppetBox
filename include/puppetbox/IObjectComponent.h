#pragma once

#include "TypeDef.h"

namespace PB
{
    class PUPPET_BOX_API IObjectComponent
    {
    public:
        virtual void update(float deltaTime) = 0;
    };
}