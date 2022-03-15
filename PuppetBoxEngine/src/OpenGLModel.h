#pragma once

#include <glad/glad.h>

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "puppetbox/DataStructures.h"
#include "puppetbox/IAnimationCatalogue.h"
#include "puppetbox/IModel.h"

#include "RenderedMesh.h"

namespace PB
{
    /**
    * \brief OpenGL specific implementation of an IModel object for storing rendering specific data.
    */
    class OpenGLModel : public IModel
    {
    public:

        /**
        * \brief Creates an OpenGL implementation specific object used for storing rendering specific data.
        *
        * \param bones              Skeletal data associated with this model.
        * \param renderedMeshes     Vector of {@link RenderedMesh} objects to use for model rendering.
        * \param animationCatalogue The {\link PB::IAnimationCatalogue} to use with this model for loading animations.
        */
        OpenGLModel(
                BoneMap& bones,
                std::unordered_map<std::string, RenderedMesh*> renderedMeshes,
                IAnimationCatalogue* animationCatalogue);

        /**
         * \brief Sets the animation of the Model.
         *
         * \param animationPath Path to the animation to start playing
         * \param startFrame    The frame that the animation will start playing from.
         */
        void playAnimation(const std::string& animationPath, std::uint32_t startFrame) override;

        /**
         * \brief Removes any currently attached {\link Animator}s from the object.
         */
        void stopAnimation() override;

        /**
         * \brief Removes any attached animators with the given animation path reference.
         *
         * \param animationPath The path reference to remove animators by.
         */
        void stopAnimation(const std::string& animationPath) override;

        /**
         * \brief Checks if the {\link IModel} currently has an attached animation.
         *
         * \return True if there is currently an attached animation, False otherwise.
         */
        bool isAnimating() const override;

        /**
         * \brief Gets the transformation matrix to position something exactly where
         * the bone associated with the given boneName is.
         *
         * \param boneName The name associated to the desired bone.
         * \return A transformation matrix for positioning on the same spot as the bone.
         */
        mat4 getAbsolutePositionForBone(const std::string& boneName) const override;

        /**
         * \brief Updates the object's state prior to the render() call.
         *
         * @param deltaTime The time passed since the last update().
         */
        void update(float deltaTime) override;

        /**
        * \brief Renders the object with OpenGL specific invocations.
        */
        void render(mat4 transform) const override;

        /**
         * \brief Rotates the specified bone to the given rotation values, negating animation logic.
         *
         * \param boneName The name of the bone to rotate.
         * \param rotation The specific values to set the bone's rotations to, in radians.
         */
        void overrideBoneRotation(const std::string& boneName, vec3 rotation) override;

        /**
         * \brief Clears any override transformations for the specified bone.
         *
         * \param boneName  The bone to clear overrides for.
         */
        void clearBoneOverrides(const std::string& boneName) override;

        /**
         * \brief Fetches the skeletal data associated with this model.
         *
         * \return The skeletal data associated with this model.
         */
        const BoneMap& getBones() const override;

    private:
        BoneMap bones_{};
        std::unordered_map<std::string, mat4> boneTransformations_{};
        std::unordered_map<std::string, mat4> boneTransformationOverrides_{};
        std::unique_ptr<IAnimator> animator_{nullptr};
        std::unordered_map<std::string, RenderedMesh*> renderedMeshes_{};
        IAnimationCatalogue* animationCatalogue_ = nullptr;
    };
}