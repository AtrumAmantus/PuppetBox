#pragma once

#include <mutex>

#include "TypeDef.h"

namespace PB
{
    class PUPPET_BOX_API AbstractObjectComponent
    {
    public:
        AbstractObjectComponent();

        AbstractObjectComponent(std::mutex* mutex);

        void init();

        virtual void tearDown();

        virtual void update(float deltaTime);

    protected:
        virtual void inits();

        std::unique_lock<std::mutex> lock();

    private:
        bool isInitialized_ = false;
        std::mutex* mutex_ = nullptr;
    };
}