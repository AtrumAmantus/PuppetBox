#pragma once

#include <mutex>

#include "TypeDef.h"

namespace PB
{
    class PUPPET_BOX_API AbstractObjectComponent
    {
    public:
        AbstractObjectComponent() : mutex_(nullptr) {};

        AbstractObjectComponent(std::mutex* mutex) : mutex_(mutex) {};

        virtual void update(float deltaTime) {};

    protected:
        std::unique_lock<std::mutex> lock()
        {
            return std::unique_lock<std::mutex>(*mutex_);
        };

    private:
        std::mutex* mutex_ = nullptr;
    };
}