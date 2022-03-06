#include "AnimationCatalogue.h"

#include <algorithm>
#include <functional>

#include <utility>

#include "GfxMath.h"

namespace PB
{
    namespace
    {
        static std::unordered_map<std::string, std::unordered_map<std::uint32_t, std::vector<TransformKeyframe>>> CACHED_KEYFRAMES{};

        inline Result<RawKeyframe> findExplicitKeyframe(
                const std::string& boneName,
                std::uint8_t frameIndexToCheck,
                std::unordered_map<std::uint8_t, std::vector<RawKeyframe>> keyframes
        )
        {
            Result<RawKeyframe> result{};

            if (keyframes.find(frameIndexToCheck) != keyframes.end())
            {
                auto& boneKeyframes = keyframes.at(frameIndexToCheck);

                auto iter = boneKeyframes.begin();

                while (!result.hasResult && iter < boneKeyframes.end())
                {
                    if (iter->boneName == boneName)
                    {
                        result.result = *iter;
                        result.hasResult = true;
                    }

                    ++iter;
                }
            }

            return result;
        }

        inline Result<float> findValueAtExplicitKeyframe(
                const std::string& boneName,
                std::uint8_t frameIndexToCheck,
                std::unordered_map<std::uint8_t, std::vector<RawKeyframe>> keyframes,
                std::function<Vec4&(RawKeyframe&)> pullVec4,
                std::function<Result<float>(Vec4&)> pullResult
        )
        {
            Result<float> result{};

            Result<RawKeyframe> keyframeResult = findExplicitKeyframe(boneName, frameIndexToCheck, keyframes);

            if (keyframeResult.hasResult)
            {
                result = pullResult(pullVec4(keyframeResult.result));
            }

            return result;
        }

        inline Result<RawKeyframe> findAdjacentKeyframe(
                std::int8_t direction,
                const std::string& boneName,
                std::uint8_t currentFrame,
                std::unordered_map<std::uint8_t, std::vector<RawKeyframe>> keyframes,
                std::vector<std::uint8_t> keyframeIndexes,
                std::function<Vec4&(RawKeyframe&)> pullVec4,
                std::function<Result<float>(Vec4&)> pullResult
        )
        {
            Result<RawKeyframe> result;

            std::vector<std::uint8_t> keyframesToSearch{};

            std::uint8_t startingPoint = 0;

            {
                if (direction < 0)
                {
                    auto itr = keyframeIndexes.begin();

                    // Find which index in keyframeIndexes we start looking with
                    while (itr < keyframeIndexes.end() && *itr < currentFrame)
                    {
                        ++startingPoint;
                        ++itr;
                    }

                    // If startingPoint is still 0, the currentframe is less than every keyframe, so start from the last one
                    if (startingPoint == 0)
                    {
                        startingPoint = keyframeIndexes.size() - 1;
                    }
                    else
                    {
                        // Adjust back one due to prior iteration logic "overshooting"
                        --startingPoint;
                    }
                }
                else
                {
                    startingPoint = keyframeIndexes.size();

                    auto itr = keyframeIndexes.end() - 1;

                    while (itr >= keyframeIndexes.begin() && *itr > currentFrame)
                    {
                        --startingPoint;
                        --itr;
                    }

                    if (startingPoint == keyframeIndexes.size())
                    {
                        startingPoint = 0;
                    }
                }
            }

            // Now, given the starting point, identify all keyframe indexes to search
            std::vector<std::uint8_t> indexesToSearch{};
            indexesToSearch.push_back(keyframeIndexes.at(startingPoint));

            for (
                    std::int32_t i = (((std::int32_t) startingPoint + direction) + keyframeIndexes.size())
                                     % keyframeIndexes.size();
                    i != startingPoint;
                    i = ((i + direction) + keyframeIndexes.size()) % keyframeIndexes.size())
            {
                indexesToSearch.push_back(keyframeIndexes.at(i));
            }

            {
                // Now we have the order in which we need to search, so start searching
                auto itr = indexesToSearch.begin();

                while (itr < indexesToSearch.end() && !result.hasResult)
                {
                    Result<RawKeyframe> keyframeResult = findExplicitKeyframe(boneName, *itr, keyframes);

                    // If it's the right keyframe, and has a value for the desired attribute, this is what we want.
                    if (keyframeResult.hasResult && pullResult(pullVec4(keyframeResult.result)).hasResult)
                    {
                        result = keyframeResult;
                    }

                    ++itr;
                }
            }

            return result;
        }

        inline Result<float> getValueForKeyframe(
                const std::string& boneName,
                std::uint8_t currentFrame,
                std::unordered_map<std::uint8_t, std::vector<RawKeyframe>> keyframes,
                std::vector<std::uint8_t> keyframeIndexes,
                std::function<Vec4&(RawKeyframe&)> pullVec4,
                std::function<Result<float>(Vec4&)> pullResult,
                std::function<float(RawKeyframe, RawKeyframe)> tweenTransform
        )
        {
            Result<float> result = findValueAtExplicitKeyframe(boneName, currentFrame, keyframes, pullVec4, pullResult);

            if (!result.hasResult)
            {
                Result<RawKeyframe> prevKeyframe = findAdjacentKeyframe(
                        -1,
                        boneName,
                        currentFrame,
                        keyframes,
                        keyframeIndexes,
                        pullVec4,
                        pullResult
                );

                if (prevKeyframe.hasResult)
                {
                    Result<RawKeyframe> nextKeyframe = findAdjacentKeyframe(
                            1,
                            boneName,
                            currentFrame,
                            keyframes,
                            keyframeIndexes,
                            pullVec4,
                            pullResult
                    );

                    if (prevKeyframe.result.frameIndex == nextKeyframe.result.frameIndex)
                    {
                        // It's the same keyframe, just use its value
                        result = pullResult(pullVec4(prevKeyframe.result));
                    }
                    else
                    {
                        result.result = tweenTransform(prevKeyframe.result, nextKeyframe.result);
                        result.hasResult = true;
                    }
                }
            }

            return result;
        }
    }

    Animation::Animation(
            std::string animationPath,
            std::uint8_t fps,
            std::uint8_t frameCount,
            std::unordered_map<std::uint8_t, std::vector<RawKeyframe>> keyframes
    ) : animationPath_(animationPath),
        fps_(fps),
        frameCount_(frameCount),
        keyframes_(std::move(keyframes))
    {
        for (auto& i: keyframes_)
        {
            keyframeIndexes_.push_back(i.first);
        }

        std::sort(keyframeIndexes_.begin(), keyframeIndexes_.end());
    }

    bool Animation::preLoadFrames(BoneMap& boneMap)
    {
        LOGGER_INFO("Preloading animation '" + getPath() + "'...");

        for (std::uint32_t i = 0; i < frameCount_; ++i)
        {
            getFrames(i, boneMap);
        }

        LOGGER_INFO("Animation '" + getPath() + "' has been loaded.");

        return true;
    }

    std::unordered_map<std::string, TransformKeyframe> Animation::getFrames(
            std::uint8_t currentFrame,
            BoneMap& bones) const
    {
        const std::uint32_t MAX_VECTORS = 2;
        const std::uint32_t MAX_AXES = 3;

        std::unordered_map<std::string, TransformKeyframe> keyframeTransformationMatrixMap{};

        auto animationItr = CACHED_KEYFRAMES.find(getPath());

        bool animationCached = animationItr != CACHED_KEYFRAMES.end();

        // First check if we already loaded this keyframe
        if (animationCached && animationItr->second.find(currentFrame) != animationItr->second.end())
        {
            // We already computed this before, awesome, use the cached values.
            for (auto& k: CACHED_KEYFRAMES.at(getPath()).at(currentFrame))
            {
                keyframeTransformationMatrixMap.insert(
                        std::pair<std::string, TransformKeyframe>{k.boneName, k}
                );
            }
        }
        else
        {
            // Otherwise, generate a keyframe for each property and store it in the cache
            const auto totalFrameCount = getFrameCount();

            std::function<Vec4&(RawKeyframe&)> getVec4[MAX_VECTORS] = {
                    [](RawKeyframe& k) -> Vec4& { return k.scale; },
                    [](RawKeyframe& k) -> Vec4& { return k.rotation; }
            };
            std::function<void(Vec4&, Result<float>)> setResult[MAX_AXES] = {
                    [](Vec4& v, Result<float> r) -> void { v.x = r; },
                    [](Vec4& v, Result<float> r) -> void { v.y = r; },
                    [](Vec4& v, Result<float> r) -> void { v.z = r; }
            };
            std::function<Result<float>(Vec4&)> getResult[MAX_AXES] = {
                    [](Vec4& v) -> Result<float> { return v.x; },
                    [](Vec4& v) -> Result<float> { return v.y; },
                    [](Vec4& v) -> Result<float> { return v.z; }
            };
            std::function < float(RawKeyframe, RawKeyframe) > tweenTransform;

            std::function<float(float, float, float)> transformAlgo[MAX_VECTORS] = {
                    [](float prev, float next, float blend) -> float { return prev + ((next - prev) * blend); },
                    [](float prev, float next, float blend) -> float {
                        float coef = next > prev ? -1 : 1;
                        float offset = abs(next - prev) > 180 ? 360 : 0;
                        float delta = ((next - prev) + (offset * coef)) * blend;
                        return fmod(prev + delta + 360, 360);
                    }
            };

            for (const auto& bone: bones.getAllBones())
            {
                RawKeyframe currentKey{};

                // For each vector
                for (std::uint8_t v4 = 0; v4 < MAX_VECTORS; ++v4)
                {
                    // For each axis
                    for (std::uint8_t r = 0; r < MAX_AXES; ++r)
                    {
                        tweenTransform = [totalFrameCount, currentFrame, getResult, transformAlgo, getVec4, v4, r](
                                RawKeyframe prev, RawKeyframe next) -> float {
                            const uint8_t prevToEnd = totalFrameCount - prev.frameIndex;
                            const uint8_t prevToNext =
                                    ((next.frameIndex - prev.frameIndex) + totalFrameCount) % totalFrameCount;
                            const uint8_t prevToCurr =
                                    currentFrame < prev.frameIndex ? prevToEnd + currentFrame : currentFrame -
                                                                                                prev.frameIndex;
                            const float blendValue = prevToCurr / (float) prevToNext;

                            float prevValue = getResult[r](getVec4[v4](prev)).result;
                            float nextValue = getResult[r](getVec4[v4](next)).result;

                            return transformAlgo[v4](prevValue, nextValue, blendValue);
                        };

                        Result<float> result = getValueForKeyframe(
                                bone.first,
                                currentFrame,
                                keyframes_,
                                keyframeIndexes_,
                                getVec4[v4],
                                getResult[r],
                                tweenTransform
                        );

                        setResult[r](getVec4[v4](currentKey), result);
                    }
                }

                vec3 position{};

                // TODO: Scaling comes later, this is a bit messy
                vec3 scale = {
                        currentKey.scale.x.orElse(1),
                        currentKey.scale.y.orElse(1),
                        currentKey.scale.z.orElse(1)
                };

                vec3 rotation = {
                        currentKey.rotation.x.orElse(0),
                        currentKey.rotation.y.orElse(0),
                        currentKey.rotation.z.orElse(0)
                };

                keyframeTransformationMatrixMap.insert(
                        std::pair<std::string, TransformKeyframe>(
                                bone.first,
                                {currentFrame, bone.first, {position, rotation, scale}}
                        )
                );
            }

            // If this animation has not been cached at all yet, add it
            if (!animationCached)
            {
                CACHED_KEYFRAMES.insert(
                        std::pair<std::string, std::unordered_map<std::uint32_t, std::vector<TransformKeyframe>>>(
                                getPath(),
                                std::unordered_map<std::uint32_t, std::vector<TransformKeyframe>>{}
                        )
                );
            }

            auto& animationMap = CACHED_KEYFRAMES.at(getPath());

            // Add a vector for the current frame index
            animationMap.insert(
                    std::pair<std::uint32_t, std::vector<TransformKeyframe>>(
                            currentFrame,
                            std::vector<TransformKeyframe>{}
                    )
            );

            auto& animationFrameMap = animationMap.at(currentFrame);

            // Add the keyframe information for each bone for the current frame index
            for (auto k: keyframeTransformationMatrixMap)
            {
                animationFrameMap.push_back(k.second);
            }
        }

        // Completed map of keyframes for each bone that has one.
        return keyframeTransformationMatrixMap;
    }

    std::string Animation::getPath() const
    {
        return animationPath_;
    }

    std::uint8_t Animation::getFps() const
    {
        return fps_;
    }

    std::uint8_t Animation::getFrameCount() const
    {
        return frameCount_;
    }

    Animator::Animator(IAnimation* animation)
            : animation_(animation), sequenceDuration_((float) animation->getFrameCount() / animation->getFps())
    {

    }

    std::string Animator::getAnimationName() const
    {
        return animation_->getPath();
    }

    void Animator::update(float deltaTime, BoneMap& bones)
    {
        //TODO: Animations never stop, need to create an animation event.
        sequenceTime_ += deltaTime;
        sequenceTime_ = fmod(sequenceTime_, sequenceDuration_);

        std::uint8_t currentFrame = (sequenceTime_ / sequenceDuration_) * animation_->getFrameCount();

        auto currentKeyframes = animation_->getFrames(currentFrame, bones);

        boneTransformations_.clear();

        for (auto& keyFrame: currentKeyframes)
        {
            boneTransformations_.insert(
                    std::pair<std::string, Transform>{keyFrame.first, keyFrame.second.transform}
            );
        }
    }

    void Animator::setCurrentFrame(std::uint32_t frame)
    {
        frame %= animation_->getFrameCount();
        sequenceTime_ = (static_cast<float>(frame) / animation_->getFps());
        sequenceTime_ = fmod(sequenceTime_, sequenceDuration_);
    }

    std::unordered_map<std::string, Transform> Animator::getBoneTransformations() const
    {
        return boneTransformations_;
    }

    AnimationCatalogue::AnimationCatalogue(std::shared_ptr<AssetLibrary> assetLibrary)
            : assetLibrary_(std::move(assetLibrary))
    {

    }

    bool AnimationCatalogue::load(const std::string& assetPath)
    {
        return assetLibrary_->loadAnimationSetAsset(assetPath, animations_);
    }

    bool AnimationCatalogue::preloadAnimation(BoneMap& boneMap, const std::string& animationPath)
    {
        auto itr = animations_.find(animationPath);

        if (itr != animations_.end())
        {
            itr->second->preLoadFrames(boneMap);
        }

        return itr != animations_.end();
    }

    std::unique_ptr<IAnimator> AnimationCatalogue::get(const std::string& animationPath) const
    {
        std::unique_ptr<IAnimator> animator;

        auto itr = animations_.find(animationPath);

        if (itr != animations_.end())
        {
            animator = std::make_unique<Animator>(itr->second);
        }
        else
        {
            animator = nullptr;
        }

        return animator;
    }
}
