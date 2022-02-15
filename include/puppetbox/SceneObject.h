#pragma once

#include <cstdint>

#include <memory>

#include "AbstractBehavior.h"
#include "Constants.h"
#include "DataStructures.h"
#include "IAnimationCatalogue.h"
#include "IModel.h"
#include "TypeDef.h"

namespace PB
{
    /**
     * \brief The basic container for renderable objects in a scene.  Derived classes should be used to extend with additional needed attributes
     * such as name, health, etc.
     *
     * This class serves as a basic container to inject asset references into for later GFX API calls during rendering.
     */
    class PUPPET_BOX_API SceneObject
    {
        friend class AbstractBehavior;

    public:

        /**
        * \brief World relative position of object.
        */
        vec3 position{0.0f, 0.0f, 0.0f};

        /**
        * \brief World relative rotation of object.
        */
        vec3 rotation{0.0f, 0.0f, 0.0f};

        /**
        * \brief World relative scale of object.
        */
        vec3 scale{1.0f, 1.0f, 1.0f};

        /**
        * \brief The current move forces being applied to the object in units per second.
        */
        vec3 velocity{0.0f, 0.0f, 0.0f};

        /**
         * \brief The unit's intended vector of movement.
         */
        vec3 moveVector{0.0f, 0.0f, 0.0f};

        /**
        * \brief The max speed of the object.
        */
        float speed = 64.0f;

        bool isUpdated = false;
    public:
        /**
        * \brief Default constructor that is typically called by a derived class to create
        * initial instance values.
        */
        SceneObject();

        /**
        * \brief Typically called by PuppetBox internal logic to apply assets to an already
        * instantiated derived object.
        *
        * \param baseScale	The scale values used as a baseline for the object, SceneObject#scale values multiply this.
        * \param model		Pointer to the IModel assets applied to this object.
        */
        SceneObject(std::string id, vec3 baseScale, IModel* model);

        /**
         * \brief Attaches the object to another object, causing it to use it's transformation
         * matrices instead, creating the effect of it occupying the space space.
         *
         * \param sceneObject The object to attach to.
         * \param attachPoint The bone to attach to on the target object.
         */
        void attachTo(SceneObject* sceneObject, const std::string& attachPoint);

        /**
         * \brief Denotes if this {\link SceneObject} is attached to another one.
         *
         * \return True if this object is attached to another, False otherwise.
         */
        bool isAttached() const;

        /**
         * \brief Returns a constant reference to the {\link SceneObject} that this object is attached to.
         *
         * \return The {\link SceneObject} tha this object is attached to.
         */
        const SceneObject& getAttached() const;

        /**
         * \brief Gets the transformation matrix to position something exactly where
         * the bone associated with the given boneName is.
         *
         * \param boneName The name associated to the desired bone.
         * \return A transformation matrix for positioning on the same spot as the bone.
         */
        mat4 getAbsolutePositionForBone(const std::string& boneName) const;

        /**
        * \brief Calls updates() and updates model matrices.
        *
        * \param deltaTime	The time passed (in Milliseconds) since the last update.
        */
        void update(float deltaTime);

        /**
        * \brief Executes GFX API specific render calls on injected assets.
        */
        void render();

        /**
         * \brief Rotates the specified bone to the given rotation values.
         *
         * \param boneName The name of the bone to rotate.
         * \param rotation The specific values to set the bone's rotations to.
         */
        void rotateBone(const std::string& boneName, vec3 rotation);

        /**
        * \brief Set a predefined behavior on the object. AbstractBehavior#init() is called
        * immediately after being added to the SceneObject.
        *
        * \param behavior	The predefined behavior to set.
        */
        void setBehavior(AI::Behavior behavior);

        /**
        * \brief Set a custom derived behavior on the object. AbstractBehavior#init() is never
        * called, and must be invoked manually.
        *
        * \param behavior	The custom derived behavior to set.
        */
        void setBehavior(std::unique_ptr<AbstractBehavior> behavior);

        /**
         * \brief Returns a reference to the current {\link AbstractBehavior} of the {\link SceneObject}.
         *
         * \return Reference to the current {\link AbstractBehavior}.
         */
        const AbstractBehavior* getBehavior() const;

        /**
         * \brief Sets the {\link IAnimationCatalogue} that will be used to load animations.
         *
         * <p>Called automatically by the {\link SceneObject} creation calls.</p>
         *
         * \param animationCatalogue The {\link IAnimationCatalogue} to use for loading/playing animations.
         */
        void setAnimationCatalogue(IAnimationCatalogue* animationCatalogue);

        /**
         * \brief Applies the given animation to the {\link IModel} associated with this
         * {\link SceneObject} beginning start frame, looping endlessly as long as it is
         * attached.
         *
         * \param animationPath The path associated with the animation to play.
         * \param startFrame    The frame of the animation to start playing at.
         */
        void playAnimation(const std::string& animationPath, std::uint32_t startFrame);

        /**
         * \brief Removes any currently attached {\link Animator}s from the object.
         */
        void stopAnimation();

        /**
         * \brief Removes any attached animation with the given path reference.
         *
         * \param animationPath The path associated with the desired animation to stop.
         */
        void stopAnimation(const std::string& animationPath);

        /**
         * Checks if the {\link SceneObject} has an attached animation.
         *
         * @return True if there is currently an attached animation, False otherwise.
         */
        bool isAnimating() const;

        /**
        * \brief Returns the actual scale of the object, which is the product of the
        * public scale vector and the internal baseScale vector.
        *
        * \return The actual scale of the object.
        */
        vec3 actualScale() const;

        /**
         * \brief Get the UUID associated with this {\link SceneObject}.
         *
         * \return The UUID associated with this {\link SceneObject}.
         */
        std::string getId() const;

        bool operator==(const SceneObject& rhs) const;

        bool operator!=(const SceneObject& rhs) const;

    protected:
        /**
        * \brief Update method for consumer defined update logic, called by update().
        *
        * \param deltaTime	The time passed (in Milliseconds) since the last update.
        */
        virtual void updates(float deltaTime) {};

        /**
         * \brief Triggered by behaviors with behavior specified event types.
         *
         * <p>Implementing classes can define event based logic to alter object state.</p>
         *
         * \param behaviorName  The name of the behavior that triggered the event.
         * \param behaviorEvent The event the behavior sent out.
         */
        virtual void behaviorEvent(std::string behaviorName, std::uint32_t behaviorEvent) {};

    private:
        std::string id_;
        vec3 baseScale_{1.0f, 1.0f, 1.0f};
        mat4 transform_{};
        IAnimationCatalogue* animationCatalogue_ = nullptr;
        IModel* model_ = nullptr;
        std::unique_ptr<AbstractBehavior> behavior_{nullptr};
        SceneObject* attachedTo_ = nullptr;
        std::string attachPoint_ = "";
    };
}