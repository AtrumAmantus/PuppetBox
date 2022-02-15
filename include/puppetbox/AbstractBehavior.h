#pragma once

#include "DataStructures.h"
#include "SceneObject.h"

namespace PB
{
    class SceneObject;

    class AbstractBehavior
    {
    public:
        AbstractBehavior(std::string behaviorName);

        /**
        * \brief Initializes the behavior configurations.
        *
        * \param sceneObject	The SceneObject the behavior is attached to.
        */
        virtual void init(SceneObject* sceneObject) = 0;

        /**
        * \brief Runs the update logic for the behavior.
        *
        * \param sceneObject	The SceneObject the behavior is attached to.
        * \param deltaTime		The amount of time (in Milliseconds) that have elapsed since the last update.
        */
        virtual void update(SceneObject* sceneObject, float deltaTime) = 0;

        /**
         * \brief Returns the target position of the currently running behavior.  This is
         * a behavior specific value with behavior specific meaning.
         *
         * \return A {\link vec3} of the current behavior's target position.
         */
        virtual vec3 getTargetPosition() const = 0;

    protected:
        /**
         * \brief Triggers the {\link SceneObject#behaviorEvent(std::string, std::string)} method on the
         * given object.
         *
         * <p>This method is invoked by derived implementations to take advantage of
         * the base classes "friendship" with the {\link SceneObject}.</p>
         *
         * @param sceneObject   The {\link SceneObject} to trigger the behavior event on.
         * @param behaviorEvent The type of behavior event to trigger.
         */
        void triggerBehaviorEvent(SceneObject* sceneObject, std::uint32_t behaviorEvent);

    private:
        std::string behaviorName_;
    };
}