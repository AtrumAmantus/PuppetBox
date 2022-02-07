#include "AnimationCatalogue.h"

#include <algorithm>
#include <functional>

#include <utility>

#include "GfxMath.h"

namespace PB
{
    namespace
    {
        std::unordered_map<std::string, std::unordered_map<std::uint32_t, std::vector<TransformKeyframe>>> CACHED_KEYFRAMES{};

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
                auto itr = keyframeIndexes.begin();

                // Find which index in keyframeIndexes we start looking with
                while (itr < keyframeIndexes.end() && *itr <= currentFrame)
                {
                    ++startingPoint;
                    ++itr;
                }
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
                std::unordered_map<std::string, BoneMap> bones,
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
            std::unordered_map<std::string, BoneMap> boneMap,
            std::uint8_t fps,
            std::uint8_t frameCount,
            std::unordered_map<std::uint8_t, std::vector<RawKeyframe>> keyframes
    ) : animationPath_(animationPath),
        boneMap_(std::move(boneMap)),
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

    /**
     * Gets the transformation matrices for each bone on the current currentFrame.
     *
     * <p>If the given currentFrame has defined transformations, those are directly used</p>
     *
     * <p>If the given currentFrame has no defined transformations, checks for previous/next frames with defined
     * transformations and does a tween</p>
     *
     * <p>If the prev/next currentFrame are the same currentFrame, the bone has a hard set value for the animation, and it uses
     * that</p>
     *
     * <p>If no transformations are defined anywhere for the bone, a default identity matrix is used.</p>
     *
     * \param currentFrame The frame to get the transformation matrix for on each bone.
     * \param bones        The {\link BoneMap} provided by the model, with accurate offsets.
     * \return An {\link std::unordered_map} of {\link Keyframe}s holding transformation matrices for every bone
     * for the given frame.
     */
    //TODO: bones vs boneMap_ is confusing, plus BoneMap structure is wacky, revisit
    std::unordered_map<std::string, TransformKeyframe>
    Animation::getFrames(std::uint8_t currentFrame, std::unordered_map<std::string, BoneMap> bones) const
    {
        //TODO: This is slow as fuck now, need to pre-load animations.
        std::unordered_map<std::string, TransformKeyframe> keyframeTransformationMatrixMap{};

        bool animationCached = CACHED_KEYFRAMES.find(getPath()) != CACHED_KEYFRAMES.end();

        if (animationCached &&
            CACHED_KEYFRAMES.at(getPath()).find(currentFrame) != CACHED_KEYFRAMES.at(getPath()).end())
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
            const auto totalFrameCount = getFrameCount();

            std::function<Vec4&(RawKeyframe&)> getVec4[] = {
                    [](RawKeyframe& k) -> Vec4& { return k.position; },
                    [](RawKeyframe& k) -> Vec4& { return k.scale; },
                    [](RawKeyframe& k) -> Vec4& { return k.rotation; }
            };
            std::function<void(Vec4&, Result<float>)> setResult[] = {
                    [](Vec4& v, Result<float> r) -> void { v.x = r; },
                    [](Vec4& v, Result<float> r) -> void { v.y = r; },
                    [](Vec4& v, Result<float> r) -> void { v.z = r; }
            };
            std::function<Result<float>(Vec4&)> getResult[] = {
                    [](Vec4& v) -> Result<float> { return v.x; },
                    [](Vec4& v) -> Result<float> { return v.y; },
                    [](Vec4& v) -> Result<float> { return v.z; }
            };
            std::function < float(RawKeyframe, RawKeyframe) > tweenTransform;

            std::function<float(float, float, float)> transformAlgo[] = {
                    [](float prev, float next, float blend) -> float { return prev + ((next - prev) * blend); },
                    [](float prev, float next, float blend) -> float { return prev + ((next - prev) * blend); },
                    [](float prev, float next, float blend) -> float {
                        float coef = next > prev ? -1 : 1;
                        float offset = abs(next - prev) > 180 ? 360 : 0;
                        float delta = ((next - prev) + (offset * coef)) * blend;
                        return fmod(prev + delta + 360, 360);
                    }
            };

            for (const auto& bone: bones)
            {
                RawKeyframe currentKey{};

                for (std::uint8_t v4 = 0; v4 < 3; ++v4)
                {
                    for (std::uint8_t r = 0; r < 3; ++r)
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
                                bones,
                                getVec4[v4],
                                getResult[r],
                                tweenTransform
                        );

                        setResult[r](getVec4[v4](currentKey), result);
                    }
                }

                Bone tModelValues = bones.at(bone.first).bone;

                // TODO: Position changes on bones is awful, this needs to be removed
                vec3 position = {
                        currentKey.position.x.orElse(tModelValues.position.x),
                        currentKey.position.y.orElse(tModelValues.position.y),
                        currentKey.position.z.orElse(tModelValues.position.z)
                };

                // TODO: Scaling comes later, this is a bit messy
                vec3 scale = {
                        currentKey.scale.x.orElse(1.0f),
                        currentKey.scale.y.orElse(1.0f),
                        currentKey.scale.z.orElse(1.0f)
                };

                vec3 rotation = {
                        currentKey.rotation.x.orElse(tModelValues.rotation.x),
                        currentKey.rotation.y.orElse(tModelValues.rotation.y),
                        currentKey.rotation.z.orElse(tModelValues.rotation.z)
                };

                mat4 transformation = GfxMath::CreateTransformation(rotation, scale, position);

                keyframeTransformationMatrixMap.insert(
                        std::pair<std::string, TransformKeyframe>(
                                bone.first,
                                {currentFrame, bone.first, transformation}
                        )
                );
            }

            auto originalKeyframeTransformationMatrixMap = keyframeTransformationMatrixMap;

            // Transformations are defined as relative to original position, so now
            // we add in the bone offsets to complete the transformation values.
            for (auto& entry: keyframeTransformationMatrixMap)
            {
                auto bone = bones.at(entry.first);

                while (!bone.name.empty())
                {
                    // Current bone's offsets were already applied, skip
                    if (bone.name != entry.second.boneName)
                    {
                        entry.second.transform *= originalKeyframeTransformationMatrixMap.at(bone.name).transform;
                    }

                    if (bones.find(bone.parent) != bones.end())
                    {
                        bone = bones.at(bone.parent);
                    }
                    else
                    {
                        bone.name = "";
                    }
                }
            }

            if (!animationCached)
            {
                CACHED_KEYFRAMES.insert(
                        std::pair<std::string, std::unordered_map<std::uint32_t, std::vector<TransformKeyframe>>>(
                                getPath(),
                                std::unordered_map<std::uint32_t, std::vector<TransformKeyframe>>{}
                        )
                );
            }

            CACHED_KEYFRAMES.at(getPath()).insert(
                    std::pair<std::uint32_t, std::vector<TransformKeyframe>>(
                            currentFrame,
                            std::vector<TransformKeyframe>{}
                    )
            );

            for (auto k: keyframeTransformationMatrixMap)
            {
                CACHED_KEYFRAMES.at(getPath()).at(currentFrame).push_back(k.second);
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

    void Animator::update(float deltaTime, std::unordered_map<std::string, BoneMap> bones)
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
                    std::pair<std::string, mat4>{keyFrame.first, keyFrame.second.transform}
            );
        }
    }

    void Animator::setCurrentFrame(std::uint32_t frame)
    {
        frame %= animation_->getFrameCount();
        sequenceTime_ = (static_cast<float>(frame) / animation_->getFps());
        sequenceTime_ = fmod(sequenceTime_, sequenceDuration_);
    }

    std::unordered_map<std::string, mat4> Animator::getBoneTransformations() const
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

    std::unique_ptr<IAnimator> AnimationCatalogue::get(const std::string& animationPath) const
    {
        if (animations_.find(animationPath) != animations_.end())
        {
            return std::make_unique<Animator>(animations_.at(animationPath));
        }

        return nullptr;
    }
}
