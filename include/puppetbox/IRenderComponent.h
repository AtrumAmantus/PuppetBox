#pragma once

namespace PB
{
    class IRenderComponent
    {
    public:
        virtual void render() const = 0;
    };
}