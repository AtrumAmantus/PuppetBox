#include "AnimationCatalogue.h"

#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <utility>

namespace PB
{
    namespace
    {
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
            std::unordered_map<std::string, BoneMap> boneMap,
            std::uint8_t fps,
            std::uint8_t length,
            std::unordered_map<std::uint8_t, std::vector<Keyframe>> keyframes
    ) : boneMap_(std::move(boneMap)), fps_(fps), length_(length), keyframes_(std::move(keyframes))
    {
        for (auto& i: keyframes_)
        {
            keyframeIndexes_.push_back(i.first);
        }

        std::sort(keyframeIndexes_.begin(), keyframeIndexes_.end());
    }

    Animator::Animator(IAnimation* animation) : animation_(animation),
                                                sequenceDuration_((float) animation->getLength() / animation->getFps())
    {

    }

    /**
     * Gets the transformation matrices for each bone on the current frame.
     *
     * <p>If the given frame has defined transformations, those are directly used</p>
     *
     * <p>If the given frame has no defined transformations, checks for previous/next frames with defined
     * transformations and does a tween</p>
     *
     * <p>If the prev/next frame are the same frame, the bone has a hard set value for the animation, and it uses
     * that</p>
     *
     * <p>If no transformations are defined anywhere for the bone, a default identity matrix is used.</p>
     *
     * TODO: This is not correctly tweening for transformations where the pos/scale/rot are on different frames.
     *
     * @param frame The frame to get the transformation matrix for on each bone.
     * @return An {@link std::unordered_map} of {@link Keyframe}s holding transformation matrices for every bone
     * for the given frame.
     */
    std::unordered_map<std::string, Keyframe>
    Animation::getFrames(std::uint8_t frame, std::unordered_map<std::string, BoneMap> bones) const
    {
        std::unordered_map<std::string, Keyframe> keyframeTransformationMatrixMap{};

        std::unique_ptr<Keyframe> prevKey{nullptr};
        std::unique_ptr<Keyframe> nextKey{nullptr};

        // Map frame values for each bone for the current frame
        for (const auto& bone: boneMap_)
        {
            bool mappedKey = false;
            prevKey.reset();
            nextKey.reset();

            Keyframe transformVectors{};

            Result<Keyframe> result{};

            // If it has a keyframe for this frame, just store its values
            if (
                    keyframes_.find(frame) != keyframes_.end()
                    && (result = findKeyframeForBone(bone.first, keyframes_.at(frame))).hasResult
                    )
            {
                transformVectors = result.result;
            }
            else
            {
                // If we didn't find a keyframe, we tween it (if possible)
                std::unique_ptr<Keyframe> firstKey{nullptr};
                std::unique_ptr<Keyframe> lastKey{nullptr};

                // Search every keyframe we have
                for (const auto& index: keyframeIndexes_)
                {
                    // Don't check the requested frame, we already know it's not there
                    if (index != frame)
                    {
                        // Try to find keyframe for given bone
                        Result<Keyframe> keyframe = findKeyframeForBone(bone.first, keyframes_.at(index));

                        // If this keyframe includes the current bone...
                        if (keyframe.hasResult)
                        {
                            if (index < frame)
                            {
                                // Save the firstKey with this bone in case this "wraps around"
                                if (!firstKey)
                                {
                                    firstKey = std::make_unique<Keyframe>(keyframe.result);
                                }

                                prevKey = std::make_unique<Keyframe>(keyframe.result);
                            }
                            else
                            {
                                // nextKey should be the first matching result after the target frame
                                if (!nextKey)
                                {
                                    nextKey = std::make_unique<Keyframe>(keyframe.result);
                                }

                                // Save the lastKey with this bone in case this "wraps around"
                                lastKey = std::make_unique<Keyframe>(keyframe.result);
                            }
                        }
                    }
                }

                if (!nextKey)
                {
                    // If we never found a nextKey, it probably wraps around to the front
                    nextKey = std::move(firstKey);
                }

                if (!prevKey)
                {
                    // If we never found a prevKey, it probably wrapped around from the end
                    prevKey = std::move(lastKey);
                }

                if (!prevKey || !nextKey)
                {
                    // If either are null, the bone is not defined in this animation, set default transformations
                    transformVectors.position = {0, 0, 0};
                    transformVectors.scale = {1, 1, 1};
                    transformVectors.rotation = {0, 0, 0};
                    transformVectors.unused = {0, 0, 0};
                }
                else
                {
                    if (*prevKey == *nextKey)
                    {
                        // If they are the same keyframe, then the bone isn't animated, just set
                        // its values to whatever that frame has them at.
                        transformVectors = *prevKey;
                    }
                    else
                    {
                        // Otherwise, complex tweening logic...
                        const uint8_t prevToEnd = length_ - prevKey->frameIndex;
                        const uint8_t prevToNext = ((nextKey->frameIndex - prevKey->frameIndex) + length_) % length_;
                        const uint8_t prevToCurr =
                                frame < prevKey->frameIndex ? prevToEnd + frame : frame - prevKey->frameIndex;
                        const float blendValue = prevToCurr / (float) prevToNext;

                        const vec4 position =
                                prevKey->position + ((prevKey->position - nextKey->position) * blendValue);
                        const vec4 scale = prevKey->scale + ((prevKey->scale - nextKey->scale) * blendValue);

                        glm::mat4 prevRot = glm::mat4{1.0f};
                        prevRot = rotVecToMat4(prevRot, prevKey->rotation);

                        glm::mat4 nextRot = glm::mat4{1.0f};
                        nextRot = rotVecToMat4(nextRot, prevKey->rotation);

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
                                                                 {frame, bone.first, mat4{&transformation[0][0]}})
                        );

                        mappedKey = true;
                    }
                }
            }

            //
            if (!mappedKey)
            {
                glm::mat4 rot = rotVecToMat4(glm::mat4(1.0), transformVectors.rotation);
                glm::mat4 pos = glm::translate(glm::mat4(1.0),
                                               {transformVectors.position.x, transformVectors.position.y,
                                                transformVectors.position.z});
                glm::mat4 sca = glm::scale(glm::mat4(1.0), {transformVectors.scale.x, transformVectors.scale.y,
                                                            transformVectors.scale.z});

                glm::mat4 transformation = pos * rot * sca;

                keyframeTransformationMatrixMap.insert(
                        std::pair<std::string, Keyframe>(bone.first,
                                                         {frame, bone.first, mat4{&transformation[0][0]}})
                );
            }
        }

        // Transformations are defined as relative to original position, so now
        // we add in the bone offsets to complete the transformation values.
        for (auto& entry: keyframeTransformationMatrixMap)
        {
            auto bone = boneMap_.at(entry.first);

            while (!bone.parent.empty())
            {
                // For each bone that is not the root (having no defined parent),
                // add the bone's offset.
                entry.second.position += bone.bone.offset;
                bone = boneMap_.at(bone.parent);
            }
        }

        // Completed map of keyframes for each bone that has one.
        return keyframeTransformationMatrixMap;
    }

    std::uint8_t Animation::getFps() const
    {
        return fps_;
    }

    std::uint8_t Animation::getLength() const
    {
        return length_;
    }

    void Animator::update(float deltaTime, std::unordered_map<std::string, BoneMap> bones)
    {
        sequenceTime_ += deltaTime;
        sequenceTime_ = fmod(sequenceTime_, sequenceDuration_);

        std::uint8_t currentFrame = (sequenceTime_ / sequenceDuration_) * animation_->getLength();

//        currentKeyframes_.clear();
        currentKeyframes_ = animation_->getFrames(currentFrame, bones);

        boneTransformations_.clear();

        for (auto& keyFrame: currentKeyframes_)
        {
            mat4 matrix = mat4::eye();

            BoneMap currentBone = {"", keyFrame.first};

            do
            {
                currentBone = bones.at(currentBone.parent);
                matrix *= currentKeyframes_.at(currentBone.name).translation;
                matrix[3] += currentBone.bone.offset;
            } while (!currentBone.parent.empty());

            //TODO: Getting closer, just some slight decapitation issues....
            boneTransformations_.insert(
                    std::pair<std::string, mat4>{keyFrame.first, matrix}
            );
        }
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

    std::unique_ptr<IAnimator> AnimationCatalogue::get(const std::string& animationName) const
    {
        if (animations_.find(animationName) != animations_.end())
        {
            return std::make_unique<Animator>(animations_.at(animationName));
        }

        return nullptr;
    }
}
