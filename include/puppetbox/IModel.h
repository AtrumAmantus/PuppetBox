#pragma once

#include <vector>

#include "DataStructures.h"
#include "IAnimationCatalogue.h"

namespace PB
{
    /**
    * \brief Interface for defining GFX API specific data to be used in rendering calls.
    */
    class IModel
    {
    public:
        /**
         * \brief Sets the animation of the Model.
         *
         * \param animationPath Path to the animation to start playing
         * \param startFrame    The frame that the animation will start playing from.
         */
        virtual void playAnimation(const std::string& animationPath, std::uint32_t startFrame) = 0;

        /**
         * \brief Removes any currently attached {\link Animator}s from the object.
         */
        virtual void stopAnimation() = 0;

        /**
         * \brief Removes any attached animators that reference the given animation path.
         *
         * \param animationPath The path reference that dictates which animators will be removed.
         */
        virtual void stopAnimation(const std::string& animationPath) = 0;

        /**
         * \brief Checks if the {\link IModel} currently has an attached animation.
         *
         * \return True if there is currently an attached animation, False otherwise.
         */
        virtual bool isAnimating() const = 0;

        /**
         * \brief Gets the transformation matrix to position something exactly where
         * the bone associated with the given boneName is.
         *
         * \param boneId The ID associated to the desired bone.
         *
         * \return A transformation matrix for positioning on the same spot as the bone.
         */
        virtual mat4 getAbsolutePositionForBone(std::uint32_t boneId) const = 0;

        /**
         * \brief Method used by application logic to update the state of the model prior to the render()
         * call.
         *
         * \param deltaTime The time passed since the last update()
         */
        virtual void update(float deltaTime) = 0;

        /**
        * \brief Method used by application logic to render an object using the GFX API invocations of
        * the derived implementation.
        */
        virtual void render(mat4 transform) const = 0;

        /**
         * \brief Rotates the specified bone to the given rotation values, negating animation logic.
         *
         * \param boneId    The ID of the bone to rotate.
         * \param rotation  The specific values to set the bone's rotations to, in radians.
         */
        virtual void overrideBoneRotation(std::uint32_t boneId, vec3 rotation) = 0;

        /**
         * \brief Clears any override transformations for the specified bone.
         *
         * \param boneId The ID of the bone to clear overrides for.
         */
        virtual void clearBoneOverrides(std::uint32_t boneId) = 0;

        /**
         * \brief Gets the ID associated with the given bone name.
         *
         * \param boneName The name of the bone to get the associated ID for.
         *
         * \return The ID associated with the given bone name, or 0 if the bone doesn't exist.
         */
        virtual const std::uint32_t getBoneId(const std::string& boneName) const = 0;

        /**
         * \brief Grabs the skeletal data associated with this {\link PB::IModel}.
         *
         * \return The skeletal data associated with this {\link PB::IModel}.
         */
        virtual const BoneMap& getBones() const = 0;
    };
}