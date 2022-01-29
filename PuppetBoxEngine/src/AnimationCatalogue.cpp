#include "AnimationCatalogue.h"

#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <utility>

#include "GfxMath.h"

namespace PB
{
    namespace
    {
        std::unordered_map<std::string, std::unordered_map<std::uint32_t, std::vector<Keyframe>>> CACHED_KEYFRAMES{};

        inline Result<Keyframe> findKeyframeForBone(const std::string& boneName, const std::vector<Keyframe>& keyframes)
        {
            for (const auto& keyframe: keyframes)
            {
                if (keyframe.boneName == boneName)
                {
                    return {keyframe, true};
                }
            }

            return {Keyframe{}, false};
        }

        inline glm::mat4 rotVecToMat4(glm::mat4 initialMatrix, vec4 rotVec)
        {
            initialMatrix = glm::rotate(initialMatrix, rotVec.x, glm::vec3(1.0f, 0.0f, 0.0f));
            initialMatrix = glm::rotate(initialMatrix, rotVec.y, glm::vec3(0.0f, 1.0f, 0.0f));
            initialMatrix = glm::rotate(initialMatrix, rotVec.z, glm::vec3(0.0f, 0.0f, 1.0f));

            return initialMatrix;
        }
    }

    Animation::Animation(
            std::string animationPath,
            std::unordered_map<std::string, BoneMap> boneMap,
            std::uint8_t fps,
            std::uint8_t frameCount,
            std::unordered_map<std::uint8_t, std::vector<Keyframe>> keyframes
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
     * TODO: This is not correctly tweening for transformations where the pos/scale/rot are on different frames.
     *
     * \param currentFrame The frame to get the transformation matrix for on each bone.
     * \param bones        The {\link BoneMap} provided by the model, with accurate offsets.
     * \return An {\link std::unordered_map} of {\link Keyframe}s holding transformation matrices for every bone
     * for the given frame.
     */
    //TODO: bones vs boneMap_ is confusing, plus BoneMap structure is wacky, revisit
    std::unordered_map<std::string, Keyframe>
    Animation::getFrames(std::uint8_t currentFrame, std::unordered_map<std::string, BoneMap> bones) const
    {
        std::unordered_map<std::string, Keyframe> keyframeTransformationMatrixMap{};

        bool animationCached = CACHED_KEYFRAMES.find(getPath()) != CACHED_KEYFRAMES.end();

        if (animationCached &&
            CACHED_KEYFRAMES.at(getPath()).find(currentFrame) != CACHED_KEYFRAMES.at(getPath()).end())
        {
            for (auto& k: CACHED_KEYFRAMES.at(getPath()).at(currentFrame))
            {
                keyframeTransformationMatrixMap.insert(
                        std::pair<std::string, Keyframe>{k.boneName, k}
                );
            }
        }
        else
        {
            Keyframe prevKey{};
            Keyframe nextKey{};

            // Map frame values for each bone for the current frame
            for (const auto& bone: bones)
            {
                bool mappedKey = false;
                prevKey.boneName = "";
                nextKey.boneName = "";

                Keyframe transformVectors{};

                Result<Keyframe> result{};

                // If it has a keyframe for this frame, just store its values
                if (
                        keyframes_.find(currentFrame) != keyframes_.end()
                        && (result = findKeyframeForBone(bone.first, keyframes_.at(currentFrame))).hasResult
                        )
                {
                    transformVectors = result.result;
                }
                else
                {
                    // If we didn't find a keyframe, we tween it (if possible)
                    Keyframe firstKey{};
                    Keyframe lastKey{};

                    // Search every keyframe we have
                    for (const auto& index: keyframeIndexes_)
                    {
                        // Don't check the requested frame, we already know it's not there
                        if (index != currentFrame)
                        {
                            // Try to find keyframe for given bone
                            Result<Keyframe> keyframe = findKeyframeForBone(bone.first, keyframes_.at(index));

                            // If this keyframe includes the current bone...
                            if (keyframe.hasResult)
                            {
                                if (index < currentFrame)
                                {
                                    // Save the firstKey with this bone in case this "wraps around"
                                    if (firstKey.boneName.empty())
                                    {
                                        firstKey = keyframe.result;
                                    }

                                    prevKey = keyframe.result;
                                }
                                else
                                {
                                    // nextKey should be the first matching result after the target frame
                                    if (nextKey.boneName.empty())
                                    {
                                        nextKey = keyframe.result;
                                    }

                                    // Save the lastKey with this bone in case this "wraps around"
                                    lastKey = keyframe.result;
                                }
                            }
                        }
                    }

                    if (prevKey.boneName.empty() && nextKey.boneName.empty())
                    {
                        // If both are null, the bone is not defined in this animation, set default transformations
                        transformVectors.position = bones.at(bone.first).bone.offset;
                        transformVectors.scale = {1, 1, 1};
                        transformVectors.rotation = {0, 0, 0};
                        transformVectors.unused = {0, 0, 0};
                    }
                    else
                    {
                        if (nextKey.boneName.empty())
                        {
                            // If we never found a nextKey, it probably wraps around to the front
                            nextKey = std::move(firstKey);
                        }

                        if (prevKey.boneName.empty())
                        {
                            // If we never found a prevKey, it probably wrapped around from the end
                            prevKey = std::move(lastKey);
                        }

                        if (prevKey.frameIndex == nextKey.frameIndex)
                        {
                            // If they are the same keyframe, then the bone isn't animated, just set
                            // its values to whatever that frame has them at.
                            transformVectors = prevKey;
                        }
                        else
                        {
                            // Otherwise, complex tweening logic...
                            const uint8_t prevToEnd = getFrameCount() - prevKey.frameIndex;
                            const uint8_t prevToNext =
                                    ((nextKey.frameIndex - prevKey.frameIndex) + getFrameCount()) % getFrameCount();
                            const uint8_t prevToCurr =
                                    currentFrame < prevKey.frameIndex ? prevToEnd + currentFrame : currentFrame -
                                                                                                   prevKey.frameIndex;
                            const float blendValue = prevToCurr / (float) prevToNext;

                            const vec4 position =
                                    prevKey.position + ((nextKey.position - prevKey.position) * blendValue);
                            const vec4 scale = prevKey.scale + ((prevKey.scale - nextKey.scale) * blendValue);

                            glm::mat4 prevRot = glm::mat4{1.0f};
                            prevRot = rotVecToMat4(prevRot, prevKey.rotation);

                            glm::mat4 nextRot = glm::mat4{1.0f};
                            nextRot = rotVecToMat4(nextRot, prevKey.rotation);

                            glm::quat fromQuat = glm::quat_cast(prevRot);
                            glm::quat toQuat = glm::quat_cast(nextRot);

                            glm::quat quatRot = glm::slerp(fromQuat, toQuat, blendValue);

                            glm::mat4 rot = glm::mat4_cast(quatRot);
                            glm::mat4 pos = glm::translate(glm::mat4(1.0), {position.x, position.y, position.z});
                            glm::mat4 sca = glm::scale(glm::mat4(1.0), {scale.x, scale.y, scale.z});

                            glm::mat4 transformation = pos * rot * sca;

                            // Store the tween values for this frame.
                            keyframeTransformationMatrixMap.insert(
                                    std::pair<std::string, Keyframe>(bone.first,
                                                                     {currentFrame, bone.first,
                                                                      mat4{&transformation[0][0]}})
                            );

                            mappedKey = true;
                        }
                    }
                }

                if (!mappedKey)
                {
                    mat4 transformation = GfxMath::CreateTransformation(
                            transformVectors.rotation,
                            transformVectors.scale,
                            transformVectors.position
                    );

                    keyframeTransformationMatrixMap.insert(
                            std::pair<std::string, Keyframe>(
                                    bone.first,
                                    {currentFrame, bone.first, transformation}
                            )
                    );
                }
            }

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
                        // Add offset of parent bone
                        mat4 m = mat4::eye();
                        m[3] = bone.bone.offset;
                        m[3][3] = 1;
                        entry.second.translation *= m;
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
                        std::pair<std::string, std::unordered_map<std::uint32_t, std::vector<Keyframe>>>{getPath(),
                                                                                                         std::unordered_map<std::uint32_t, std::vector<Keyframe>>{}}
                );
            }

            CACHED_KEYFRAMES.at(getPath()).insert(
                    std::pair<std::uint32_t, std::vector<Keyframe>>{currentFrame, std::vector<Keyframe>{}}
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

    void Animator::setMode(std::uint8_t mode)
    {
        mode_ = mode;
    }

    void Animator::update(float deltaTime, std::unordered_map<std::string, BoneMap> bones)
    {
        sequenceTime_ += deltaTime;
        sequenceTime_ = fmod(sequenceTime_, sequenceDuration_);

        std::uint8_t currentFrame = (sequenceTime_ / sequenceDuration_) * animation_->getFrameCount();

        if (playedOneFrame_ && currentFrame == 0)
        {
            animationFinished_ = true;
        }
        else
        {
            playedOneFrame_ = playedOneFrame_ || currentFrame > 1;

            auto currentKeyframes = animation_->getFrames(currentFrame, bones);

            boneTransformations_.clear();

            for (auto& keyFrame: currentKeyframes)
            {
                mat4 matrix = mat4::eye();

                BoneMap currentBone = {"", keyFrame.first};

                do
                {
                    currentBone = bones.at(currentBone.parent);
                    matrix *= currentKeyframes.at(currentBone.name).translation;
                } while (!currentBone.parent.empty());

                boneTransformations_.insert(
                        std::pair<std::string, mat4>{keyFrame.first, matrix}
                );
            }
        }
    }

    bool Animator::finished() const
    {
        return animationFinished_;
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
