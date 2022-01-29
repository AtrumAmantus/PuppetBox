#pragma once

#include "puppetbox/TypeDef.h"
#include "DataStructures.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace PB
{
    struct Keyframe
    {
        std::uint8_t frameIndex;
        std::string boneName;
        union
        {
            mat4 translation{};
            struct
            {
                vec4 rotation;
                vec4 scale;
                vec4 unused;
                vec4 position;
            };
        };

        bool operator==(const Keyframe& rhs) const
        {
            // If the frame index and bone name are the same, it should be the same
            // keyframe, or something really wrong happened.
            return this->frameIndex == rhs.frameIndex
                   && this->boneName == rhs.boneName;
        }
    };

    class PUPPET_BOX_API IAnimation
    {
    public:
        /**
         * \brief Gets keyframes for the given bones at the given frame index.
         *
         * <p>Every bone will have a keyframe for the given index.</p>
         *
         * \param currentFrame The frame index to get keyframes for.
         * \param bones        The bones to get keyframes for.
         * \return A map of the keyframes for the given bones, keyed per bone.
         */
        virtual std::unordered_map<std::string, Keyframe>
        getFrames(std::uint8_t currentFrame, std::unordered_map<std::string, BoneMap> bones) const = 0;

        /**
         * \brief Returns the path associated with the animation.
         *
         * \return The path associated with the animation.
         */
        virtual std::string getPath() const = 0;

        /**
         * \brief Gets the defined FPS of the animation.
         *
         * <p>This can be used to dictate the "speed" the animation plays at.</p>
         *
         * \return The defined FPS of the animation.
         */
        virtual std::uint8_t getFps() const = 0;

        /**
         * \brief Gets the total number of frames in the animation.
         *
         * \return The total number of frames in the animation.
         */
        virtual std::uint8_t getFrameCount() const = 0;
    };

    class PUPPET_BOX_API IAnimator
    {
    public:
        /**
         * \brief Gets the path name for the animation attached to this animator.
         *
         * \return Path name for the attached animation.
         */
        virtual std::string getAnimationName() const = 0;

        /**
         * \brief Sets the play mode for the animation.
         *
         * <ul>
         * <li>0 -> Endless</li>
         * <li>1 -> Single loop</li>
         * </ul>
         *
         * \param mode The desired mode for the animation.
         */
        virtual void setMode(std::uint8_t mode) = 0;

        /**
         * \brief Update the state of the current animator.
         *
         * \param deltaTime The time since the last update cycle.
         * \param bones     The bones for the target skeleton to apply the animation to.
         */
        virtual void update(float deltaTime, std::unordered_map<std::string, BoneMap> bones) = 0;

        /**
         * \brief Sets the current frame for the animation.
         *
         * \param frame The frame number to set the animation to.
         */
        virtual void setCurrentFrame(std::uint32_t frame) = 0;

        /**
         * \brief Indicates if the animation has finished playing.
         *
         * <p>An animation is finished if it's MODE is set to 1 (singe loop), and
         * it has already completed one cycle back to the first frame again.
         *
         * \return True if the animation has completed one cycle, False otherwise.
         */
        virtual bool finished() const = 0;

        /**
         * \brief Gets the previously calculated bone transformation matrices
         * for the attached animation.
         *
         * \return A map of the bone transformation matrices, keyed on each bone name.
         */
        virtual std::unordered_map<std::string, mat4> getBoneTransformations() const = 0;
    };

    class PUPPET_BOX_API IAnimationCatalogue
    {
    public:
        //TODO: The relationship between loading animation sets and getting a single
        // animation is a messy one.
        /**
         * \brief Loads the animations defined at the given path reference.
         *
         * \param assetPath The path reference to the desired animations to load.
         * \return True if the animations were loaded successfully, False otherwise.
         */
        virtual bool load(const std::string& assetPath) = 0;

        /**
         * \brief Get the animation associated to the given path reference.
         *
         * \param animationPath The path name associated to the desired animation to get.
         * \return The animation that was associated with the given path reference.
         */
        virtual std::unique_ptr<IAnimator> get(const std::string& animationPath) const = 0;
    };
}
