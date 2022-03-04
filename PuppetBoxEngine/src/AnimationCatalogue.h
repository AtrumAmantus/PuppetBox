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
                std::uint8_t fps,
                std::uint8_t frameCount,
                std::unordered_map<std::uint8_t, std::vector<RawKeyframe>> keyframes
        );

        bool preLoadFrames(BoneMap& boneMap) override;

        std::unordered_map<std::string, TransformKeyframe> getFrames(
                std::uint8_t currentFrame,
                BoneMap& bones) const override;

        std::string getPath() const;

        std::uint8_t getFps() const;

        std::uint8_t getFrameCount() const;

    private:
        std::string animationPath_;
        std::uint8_t fps_;
        std::uint8_t frameCount_;
        //TODO: Revisit the RawKeyframe logic
        /** Keyframes are stored as separate scale/rotation/transform vectors */
        const std::unordered_map<std::uint8_t, std::vector<RawKeyframe>> keyframes_{};
        std::vector<std::uint8_t> keyframeIndexes_{};
    };

    class Animator : public IAnimator
    {
    public:
        explicit Animator(IAnimation* animation);

        std::string getAnimationName() const override;

        void update(float deltaTime, BoneMap& bones) override;

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

        bool preloadAnimation(BoneMap& boneMap, const std::string& animationPath) override;

        std::unique_ptr<IAnimator> get(const std::string& animationPath) const override;

    private:
        std::unordered_map<std::string, IAnimation*> animations_{};
        std::shared_ptr<AssetLibrary> assetLibrary_;
    };
}