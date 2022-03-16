#include "AnimationCatalogue.h"

#include <algorithm>
#include <functional>

#include <utility>

#include "GfxMath.h"

namespace PB
{
    namespace
    {
        static std::unordered_map<std::string, std::unordered_map<std::uint32_t, std::unordered_map<std::uint32_t, TransformKeyframe>>> CACHED_KEYFRAMES{};
        static std::unordered_map<std::string, std::unordered_map<std::uint32_t, std::unordered_map<std::uint32_t, mat4>>> CACHED_TRANSFORMATIONS{};

        inline void cacheTransformationMatrix(
                const std::string& animationPath,
                const std::uint32_t currentFrame,
                std::uint32_t boneId,
                mat4 transformationMatrix)
        {
            if (CACHED_TRANSFORMATIONS.find(animationPath) == CACHED_TRANSFORMATIONS.end())
            {
                CACHED_TRANSFORMATIONS.insert(
                        std::pair<std::string, std::unordered_map<std::uint32_t, std::unordered_map<std::uint32_t, mat4>>>{
                                animationPath, {}}
                );
            }

            auto& frameIndexMap = CACHED_TRANSFORMATIONS.at(animationPath);

            if (frameIndexMap.find(currentFrame) == frameIndexMap.end())
            {
                frameIndexMap.insert(
                        std::pair<std::uint32_t, std::unordered_map<std::uint32_t, mat4>>{currentFrame, {}}
                );
            }

            auto& boneMap = frameIndexMap.at(currentFrame);

            boneMap.insert(
                    std::pair<std::uint32_t, mat4>{boneId, transformationMatrix}
            );
        }

        inline Result<mat4*> findCachedTransformationMatrix(
                const std::string& animationPath,
                const std::uint32_t currentFrame,
                std::uint32_t boneId)
        {
            auto animationItr = CACHED_TRANSFORMATIONS.find(animationPath);

            if (animationItr != CACHED_TRANSFORMATIONS.end())
            {
                auto frameItr = animationItr->second.find(currentFrame);

                if (frameItr != animationItr->second.end())
                {
                    auto boneItr = frameItr->second.find(boneId);

                    if (boneItr != frameItr->second.end())
                    {
                        return {&boneItr->second, true};
                    }
                }
            }

            return {nullptr, false};
        }

        inline Result<TransformKeyframe*> findCachedTransformKeyframe(
                const std::string& animationPath,
                const std::uint32_t currentFrame,
                std::uint32_t boneId)
        {
            auto animationItr = CACHED_KEYFRAMES.find(animationPath);

            if (animationItr != CACHED_KEYFRAMES.end())
            {
                auto frameItr = animationItr->second.find(currentFrame);

                if (frameItr != animationItr->second.end())
                {
                    auto boneItr = frameItr->second.find(boneId);

                    if (boneItr != frameItr->second.end())
                    {
                        return {&boneItr->second, true};
                    }
                }
            }

            return {nullptr, false};
        }

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
            for (auto& entry: boneMap.getAllBones())
            {
                getKeyFrameForBone(i, entry.first, entry.second.name);
            }
        }

        LOGGER_INFO("Animation '" + getPath() + "' has been loaded.");

        return true;
    }

    TransformKeyframe& Animation::getKeyFrameForBone(
            std::uint8_t currentFrame,
            std::uint32_t boneId,
            const std::string& boneName) const
    {
        const std::uint32_t MAX_VECTORS = 2;
        const std::uint32_t MAX_AXES = 3;

        Result<TransformKeyframe*> cachedFrame = findCachedTransformKeyframe(getPath(), currentFrame, boneId);

        if (cachedFrame.hasResult)
        {
            return *cachedFrame.result;
        }
        else
        {
            // Otherwise, generate a keyframe for each property and store it in the cache
            const std::uint8_t totalFrameCount = getFrameCount();

            const std::function<Vec4&(RawKeyframe&)> getVec4[MAX_VECTORS] = {
                    [](RawKeyframe& k) -> Vec4& { return k.scale; },
                    [](RawKeyframe& k) -> Vec4& { return k.rotation; }
            };

            const std::function<void(Vec4&, Result<float>)> setResult[MAX_AXES] = {
                    [](Vec4& v, Result<float> r) -> void { v.x = r; },
                    [](Vec4& v, Result<float> r) -> void { v.y = r; },
                    [](Vec4& v, Result<float> r) -> void { v.z = r; }
            };

            const std::function<Result<float>(Vec4&)> getResult[MAX_AXES] = {
                    [](Vec4& v) -> Result<float> { return v.x; },
                    [](Vec4& v) -> Result<float> { return v.y; },
                    [](Vec4& v) -> Result<float> { return v.z; }
            };

            const std::function<float(float, float, float)> transformAlgo[MAX_VECTORS] = {
                    [](float prev, float next, float blend) -> float { return prev + ((next - prev) * blend); },
                    [](float prev, float next, float blend) -> float {
                        float coef = next > prev ? -1 : 1;
                        float offset = abs(next - prev) > PI ? TWO_PI : 0;
                        float delta = ((next - prev) + (offset * coef)) * blend;
                        return fmod(prev + delta + TWO_PI, TWO_PI);
                    }
            };

            std::function<float(RawKeyframe, RawKeyframe)> tweenTransform;

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
                                currentFrame < prev.frameIndex ?
                                prevToEnd + currentFrame :
                                currentFrame - prev.frameIndex;
                        const float blendValue = prevToCurr / (float) prevToNext;

                        float prevValue = getResult[r](getVec4[v4](prev)).result;
                        float nextValue = getResult[r](getVec4[v4](next)).result;

                        return transformAlgo[v4](prevValue, nextValue, blendValue);
                    };

                    Result<float> result = getValueForKeyframe(
                            boneName,
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

            TransformKeyframe transformKeyframe = {currentFrame, boneName, boneId, {position, rotation, scale}};

            // If this animation has not been cached at all yet, add it
            if (CACHED_KEYFRAMES.find(getPath()) == CACHED_KEYFRAMES.end())
            {
                CACHED_KEYFRAMES.insert(
                        std::pair<std::string, std::unordered_map<std::uint32_t, std::unordered_map<std::uint32_t, TransformKeyframe>>>(
                                getPath(),
                                {}
                        )
                );
            }

            auto& frameIndexMap = CACHED_KEYFRAMES.at(getPath());

            // If this frame hasn't been cached yet, add it
            frameIndexMap.insert(
                    std::pair<std::uint32_t, std::unordered_map<std::uint32_t, TransformKeyframe>>(
                            currentFrame,
                            {}
                    )
            );

            auto& boneTransformMap = frameIndexMap.at(currentFrame);

            // Finally, add the bone's TransformKeyframe
            boneTransformMap.insert(
                    std::pair<std::uint32_t, TransformKeyframe>{boneId, transformKeyframe}
            );

            return boneTransformMap.at(boneId);
        }
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

    void Animator::update(float deltaTime, BoneMap& bones, std::unordered_map<std::uint32_t, mat4> overrides)
    {
        //TODO: Animations never stop, need to create an animation event.
        sequenceTime_ += deltaTime;
        sequenceTime_ = fmod(sequenceTime_, sequenceDuration_);

        std::uint8_t currentFrame = (sequenceTime_ / sequenceDuration_) * animation_->getFrameCount();

        // If this is still the same frame, nothing to update
        if (currentFrame == lastFrameIndex_) return;

        lastFrameIndex_ = currentFrame;

        std::unordered_map<std::uint32_t, mat4> boneTransformations{};

        for (auto& entry: bones.getAllBones())
        {
            auto override = overrides.find(entry.first);

            // If there is an override for this bone, don't calculate animation transform
            if (override != overrides.end())
            {
                boneTransformations.insert(
                        std::pair<std::uint32_t, mat4>{entry.first, override->second}
                );
            }
            else
            {
                // Check to see if this transformation has been calculated before
                Result<mat4*> transformationMatrix = findCachedTransformationMatrix(
                        animation_->getPath(),
                        currentFrame,
                        entry.first);

                if (transformationMatrix.hasResult)
                {
                    boneTransformations.insert(
                            std::pair<std::uint32_t, mat4>{entry.first, *transformationMatrix.result}
                    );
                }
                else
                {
                    // Get the bone's Transform keyframe
                    auto& boneKeyframe = animation_->getKeyFrameForBone(currentFrame, entry.first, entry.second.name);

                    //TODO: Need to validate the animation skeleton matches the model skeleton
                    const BoneNode* boneNode = bones.getBone(entry.first).result;

                    mat4 matrix = GfxMath::CreateTransformation(
                            boneKeyframe.transform.rotation,
                            boneKeyframe.transform.scale,
                            boneNode->bone.position.vec3());

                    // Create a transformation matrix for the bone
                    boneTransformations.insert(
                            std::pair<std::uint32_t, mat4>{entry.first, matrix}
                    );

                    cacheTransformationMatrix(animation_->getPath(), currentFrame, entry.first, matrix);
                }
            }
        }

        boneTransformations_.clear();

        // Iteratively compound transform matrix with parent bone
        for (auto& transform: boneTransformations)
        {
            boneTransformations_.insert(
                    std::pair<std::uint32_t, mat4>{transform.first, transform.second}
            );

            mat4& matrix = boneTransformations_.at(transform.first);

            const BoneNode* bone = bones.getBone(transform.first).result;

            auto parent = bones.getBone(bone->parentId);

            while (parent.hasResult)
            {
                matrix = boneTransformations.at(parent.result->id) * matrix;

                parent = bones.getBone(parent.result->parentId);
            }
        }
    }

    void Animator::setCurrentFrame(std::uint32_t frame)
    {
        frame %= animation_->getFrameCount();
        sequenceTime_ = (static_cast<float>(frame) / animation_->getFps());
        sequenceTime_ = fmod(sequenceTime_, sequenceDuration_);
    }

    const std::unordered_map<std::uint32_t, mat4>& Animator::getBoneTransformations() const
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
