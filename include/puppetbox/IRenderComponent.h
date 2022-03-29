#pragma once

namespace PB
{
    class IRenderComponent
    {
    public:
        virtual ~IRenderComponent() = default;

        virtual void render() const = 0;
    };
}