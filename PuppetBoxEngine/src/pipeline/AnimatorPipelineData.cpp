#include "../Pipeline.h"

namespace PB
{
    void AnimatorPipelineData::addData(const UUID uuid)
    {
        BoneMap defaultBoneMap{};
        defaultBoneMap.addBone("root", "", Bone{});

        animatorVector_->push_back(EntityAnimator{
            uuid,
            std::move(defaultBoneMap)
        });
    }

    void AnimatorPipelineData::removeDataAt(std::uint32_t index)
    {
        animatorVector_->erase(animatorVector_->begin() + index);
    }

    const std::string AnimatorPipelineData::getReference() const
    {
        return "pb_animator";
    }

    std::shared_ptr<void> AnimatorPipelineData::getDataVector() const
    {
        return animatorVector_;
    }
}