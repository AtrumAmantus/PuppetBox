#pragma once

namespace PB
{
    class IRenderComponent
    {
    public:
        virtual ~IRenderComponent() = default;

        virtual void init() = 0;

        virtual void tearDown() = 0;

        virtual void render() = 0;
    };
}