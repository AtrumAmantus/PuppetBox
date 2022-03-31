#pragma once

namespace PB
{
    class IRenderComponent
    {
    public:
        virtual ~IRenderComponent() = default;

        virtual void tearDown() = 0;

        virtual void render() = 0;
    };
}