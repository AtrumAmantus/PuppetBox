#include "../Pipeline.h"

namespace PB
{
    std::string DefaultAnimator::getAnimationName() const
    {
        return "No animation loaded.";
    }

    void DefaultAnimator::update(float deltaTime, BoneMap& bones, std::unordered_map<std::uint32_t, mat4> overrides)
    {
        for (auto& entry : bones.getAllBones())
        {
            transformations_[entry.first] = entry.second.bone.defaultTransform;
        }
    }

    void DefaultAnimator::setCurrentFrame(std::uint32_t frame)
    {

    }

    const std::unordered_map<std::uint32_t, mat4>& DefaultAnimator::getBoneTransformations() const
    {
        return transformations_;
    }
}