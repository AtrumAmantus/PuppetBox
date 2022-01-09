#pragma once

#include "puppetbox/DataStructures.h"

#include <vector>

namespace PB
{
    /**
    * \brief Interface for defining GFX API specific data to be used in rendering calls.
    */
    class IModel
    {
    public:
        /**
         * \brief Sets the animation of the Model to the one held by the given animator.
         *
         * @param animator {@link Animator} holding the desired @{link Animation}.
         */
        virtual void playAnimation(std::unique_ptr<IAnimator> animator) = 0;

        /**
         * \brief Method used by application logic to update the state of the model prior to the render()
         * call.
         *
         * @param deltaTime The time passed since the last update()
         */
        virtual void update(float deltaTime) = 0;

        /**
        * \brief Method used by application logic to render an object using the GFX API invocations of
        * the derived implementation.
        */
        virtual void render(mat3 transform) const = 0;
    };
}