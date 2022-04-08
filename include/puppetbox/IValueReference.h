#pragma once

#include "DataStructures.h"
#include "TypeDef.h"

namespace PB
{
    class PUPPET_BOX_API IValueReference
    {
    public:
        virtual ~IValueReference() {};

        virtual mat4 getMat4() const = 0;
    };

    typedef std::function<void(std::shared_ptr<IValueReference>)> pb_ValueReferenceCallback;
}