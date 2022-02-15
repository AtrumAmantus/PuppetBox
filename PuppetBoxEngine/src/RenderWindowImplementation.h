#pragma once

#include "puppetbox/IRenderWindow.h"
#include "IGfxApi.h"

namespace PB
{
    class RenderWindowImplementation : public IRenderWindow
    {
    public:
        RenderWindowImplementation(IGfxApi* gfxApi) : gfxApi_(gfxApi) {};

        vec2 getDimensions() const override
        {
            return vec2(gfxApi_->getRenderWidth(), gfxApi_->getRenderHeight());
        }

    private:
        IGfxApi* gfxApi_ = nullptr;
    };
}