#pragma once

#include <vector>

#include "puppetbox/TypeDef.h"
#include "AbstractInputProcessor.h"

namespace PB
{
    /**
    * \brief Base class to create derived SceneHandlers for guiding events within a scene.
    */
    class PUPPET_BOX_API AbstractSceneHandler
    {
    public:
        AbstractSceneHandler() = default;

        explicit AbstractSceneHandler(AbstractInputProcessor* inputProcessor) : inputProcessor_(inputProcessor) {};

        /**
        * \brief Runs only once, when the scene first loads
        */
        virtual void setUp() {};

        /**
        * \brief Runs once per frame, after input processing, but before rendering.
        *
        * \param deltaTime The time in milliseconds since the last frame was run.
        */
        virtual void update(const float deltaTime) {};

        /**
        * \brief Runs once per frame, after updates
        */
        virtual void render() {};
    protected:
        AbstractInputProcessor* input()
        {
            return inputProcessor_;
        };
    private:
        AbstractInputProcessor* inputProcessor_ = nullptr;
    };
}