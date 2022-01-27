#pragma once

#include <cstdint>

#include <memory>

#include "Constants.h"
#include "DataStructures.h"
#include "IAnimationCatalogue.h"
#include "IBehavior.h"
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
        * \brief The current move forces being applied to the object.
        */
        vec3 velocity{0.0f, 0.0f, 0.0f};

        /**
        * \brief The max speed of the object.
        */
        float speed = 64.0f;
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
        * \brief Set a predefined behavior on the object. IBehavior#init() is called
        * immediately after being added to the SceneObject.
        *
        * \param behavior	The predefined behavior to set.
        */
        void setBehavior(AI::Behavior behavior);

        /**
        * \brief Set a custom derived behavior on the object. IBehavior#init() is never
        * called, and must be invoked manually.
        *
        * \param behavior	The custom derived behavior to set.
        */
        void setBehavior(std::unique_ptr<IBehavior> behavior);

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
         * \param animationName The name of the animation to play.
         * \param startFrame    The frame of the animation to start playing at.
         */
        void playAnimation(const std::string& animationName, std::uint32_t startFrame);

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
    private:
        std::string id_;
        vec3 baseScale_{1.0f, 1.0f, 1.0f};
        IAnimationCatalogue* animationCatalogue_ = nullptr;
        IModel* model_ = nullptr;
        std::unique_ptr<IBehavior> behavior_{nullptr};
    };
}