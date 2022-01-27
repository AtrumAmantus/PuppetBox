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
        virtual std::unordered_map<std::string, Keyframe>
        getFrames(std::uint8_t currentFrame, std::unordered_map<std::string, BoneMap> bones) const = 0;

        virtual std::uint8_t getFps() const = 0;

        virtual std::uint8_t getLength() const = 0;
    };

    class PUPPET_BOX_API IAnimator
    {
    public:
        virtual void update(float deltaTime, std::unordered_map<std::string, BoneMap> bones) = 0;

        virtual void setCurrentFrame(std::uint32_t frame) = 0;

        virtual std::unordered_map<std::string, mat4> getBoneTransformations() const = 0;
    };

    class PUPPET_BOX_API IAnimationCatalogue
    {
    public:
        virtual bool load(const std::string& assetPath) = 0;

        virtual std::unique_ptr<IAnimator> get(const std::string& animationName) const = 0;
    };
}
