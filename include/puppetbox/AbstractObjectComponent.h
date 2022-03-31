#pragma once

#include <mutex>

#include "TypeDef.h"

namespace PB
{
    class PUPPET_BOX_API AbstractObjectComponent
    {
    public:

    public:
        void init();

        virtual void tearDown();

        void update(float deltaTime);

    protected:
        virtual void inits();

        virtual void updates(float deltaTime);

        std::unique_lock<std::mutex> createLock();

    private:
        bool isInitialized_ = false;
        std::mutex mutex_;
    };
}