#pragma once

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
        void triggerBehaviorEvent(SceneObject* sceneObject, std::string behaviorEvent);

    private:
        std::string behaviorName_;
    };
}