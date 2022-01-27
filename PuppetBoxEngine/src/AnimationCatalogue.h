#pragma once

#include "puppetbox/IAnimationCatalogue.h"
#include "AssetLibrary.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace PB
{
    class Animation : public IAnimation
    {
    public:
        Animation(
                std::string animationPath,
                std::unordered_map<std::string, BoneMap> boneMap,
                std::uint8_t fps,
                std::uint8_t length,
                std::unordered_map<std::uint8_t, std::vector<Keyframe>> keyframes
        );

        std::unordered_map<std::string, Keyframe>
        getFrames(std::uint8_t currentFrame, std::unordered_map<std::string, BoneMap> bones) const override;

        std::uint8_t getFps() const override;

        std::uint8_t getLength() const override;

    private:
        const std::string animationPath_;
        const std::unordered_map<std::string, BoneMap> boneMap_{};
        const std::uint8_t fps_;
        const std::uint8_t frameLength_;
        /**
         * Keyframes are stored as separate scale/rotation/transform vectors
         */
        const std::unordered_map<std::uint8_t, std::vector<Keyframe>> keyframes_{};
        std::vector<std::uint8_t> keyframeIndexes_{};
    };

    class Animator : public IAnimator
    {
    public:
        explicit Animator(IAnimation* animation);

        void update(float deltaTime, std::unordered_map<std::string, BoneMap> bones) override;

        void setCurrentFrame(std::uint32_t frame) override;

        std::unordered_map<std::string, mat4> getBoneTransformations() const override;

    private:
        IAnimation* animation_;
        float sequenceTime_ = 0;
        float sequenceDuration_;
        std::unordered_map<std::string, mat4> boneTransformations_;
    };

    class AnimationCatalogue : public IAnimationCatalogue
    {
    public:
        explicit AnimationCatalogue(std::shared_ptr<AssetLibrary> assetLibrary);

        bool load(const std::string& assetPath) override;

        std::unique_ptr<IAnimator> get(const std::string& animationName) const override;

    private:
        std::unordered_map<std::string, IAnimation*> animations_{};
        std::shared_ptr<AssetLibrary> assetLibrary_;
    };
}