#pragma once

#include "DataStructures.h"
#include "SceneObject.h"
#include "TypeDef.h"

namespace PB
{
    class SceneObject;

    class PUPPET_BOX_API AbstractBehavior
    {
    public:
        AbstractBehavior(const std::string& behaviorName);

        virtual ~AbstractBehavior();

        /**
        * \brief Initializes the behavior configurations, called automatically.
        *
        * \param sceneObject	The SceneObject the behavior is attached to.
        */
        void init(SceneObject* sceneObject);

        /**
        * \brief Runs the update logic for the behavior.
        *
        * \param sceneObject	The SceneObject the behavior is attached to.
        * \param deltaTime		The amount of time (in seconds) that have elapsed since the last update.
        */
        virtual void update(float deltaTime) = 0;

        /**
         * \brief Returns the target position of the currently running behavior.  This is
         * a behavior specific value with behavior specific meaning.
         *
         * \return A {\link vec3} of the current behavior's target position.
         */
        virtual vec3 getTargetPosition() const = 0;

    protected:
        /**
         * \brief Can be defined by the implementing class to define extra initialization logic. This is
         * called internally.
         */
        virtual void inits();

        /**
         * \brief Returns the stored {\link PB::SceneObject} reference of the {\link PB::SceneObject} this
         * behavior is attached to.
         *
         * \return The {\link PB::SceneObject} reference of the object this behavior is attached to.
         */
        SceneObject* sceneObject();

        /**
         * \brief Triggers the {\link SceneObject#behaviorEvent(std::string, std::string)} method on the
         * given object.
         *
         * <p>This method is invoked by derived implementations to take advantage of
         * the base classes "friendship" with the {\link SceneObject}.</p>
         *
         * \param sceneObject   The {\link SceneObject} to trigger the behavior event on.
         * \param behaviorEvent The type of behavior event to trigger.
         */
        void triggerBehaviorEvent(std::uint32_t behaviorEvent);

    private:
        std::string behaviorName_;
        SceneObject* sceneObject_;
    };
}