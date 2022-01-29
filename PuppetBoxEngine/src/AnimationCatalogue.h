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
                std::uint8_t frameCount,
                std::unordered_map<std::uint8_t, std::vector<Keyframe>> keyframes
        );

        std::unordered_map<std::string, Keyframe>
        getFrames(std::uint8_t currentFrame, std::unordered_map<std::string, BoneMap> bones) const override;

        std::string getPath() const;

        std::uint8_t getFps() const;

        std::uint8_t getFrameCount() const;

    private:
        std::string animationPath_;
        std::uint8_t fps_;
        std::uint8_t frameCount_;
        // TODO: This should be only used to verify the animation's BoneMap matches the model's
        const std::unordered_map<std::string, BoneMap> boneMap_{};
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

        std::string getAnimationName() const override;

        void setMode(std::uint8_t mode) override;

        void update(float deltaTime, std::unordered_map<std::string, BoneMap> bones) override;

        bool finished() const override;

        void setCurrentFrame(std::uint32_t frame) override;

        std::unordered_map<std::string, mat4> getBoneTransformations() const override;

    private:
        IAnimation* animation_;
        bool playedOneFrame_ = false;
        bool animationFinished_ = false;
        std::uint8_t mode_ = 0;
        float sequenceTime_ = 0;
        float sequenceDuration_;
        std::unordered_map<std::string, mat4> boneTransformations_;
    };

    class AnimationCatalogue : public IAnimationCatalogue
    {
    public:
        explicit AnimationCatalogue(std::shared_ptr<AssetLibrary> assetLibrary);

        bool load(const std::string& assetPath) override;

        std::unique_ptr<IAnimator> get(const std::string& animationPath) const override;

    private:
        std::unordered_map<std::string, IAnimation*> animations_{};
        std::shared_ptr<AssetLibrary> assetLibrary_;
    };
}