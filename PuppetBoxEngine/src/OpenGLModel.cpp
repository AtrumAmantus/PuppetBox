#include "OpenGLModel.h"

#include <utility>

#include "GfxMath.h"

namespace PB
{
    OpenGLModel::OpenGLModel(
            BoneMap& bones,
            std::unordered_map<std::string, RenderedMesh*> renderedMeshes,
            IAnimationCatalogue* animationCatalogue) :
            bones_(std::move(bones)),
            animationCatalogue_(animationCatalogue),
            renderedMeshes_(std::move(renderedMeshes))
    {

    }

    void OpenGLModel::playAnimation(const std::string& animationPath, std::uint32_t startFrame)
    {
        animator_ = std::move(animationCatalogue_->get(animationPath));

        if (animator_ != nullptr)
        {
            animator_->setCurrentFrame(startFrame);
        }
    }

    void OpenGLModel::stopAnimation()
    {
        animator_ = nullptr;
    }

    void OpenGLModel::stopAnimation(const std::string& animationPath)
    {
        if (animator_ && animator_->getAnimationName() == animationPath)
        {
            animator_ = nullptr;
        }
    }

    bool OpenGLModel::isAnimating() const
    {
        return animator_ != nullptr;
    }

    mat4 OpenGLModel::getAbsolutePositionForBone(const std::string& boneName) const
    {
        return boneTransformations_.at(boneName);
    }

    void OpenGLModel::update(float deltaTime)
    {
        boneTransformations_.clear();

        if (animator_ != nullptr)
        {
            // Update bone transformation matrices for current frame
            animator_->update(deltaTime, bones_, boneTransformationOverrides_);

            // Get bone transformation matrices
            auto& keyValues = animator_->getBoneTransformations();

            boneTransformations_.insert(keyValues.begin(), keyValues.end());
        }
        else
        {
            //TODO: not supporting mix animations atm

            // Update T-Pose local bone transforms
            for (auto& itr : bones_.getAllBones())
            {
                auto override = boneTransformationOverrides_.find(itr.first);

                if (override != boneTransformationOverrides_.end())
                {
                    itr.second.bone.transform = override->second;
                }
                else
                {
                    itr.second.bone.transform = GfxMath::CreateTransformation(
                            itr.second.bone.rotation.vec3(),
                            itr.second.bone.scale.vec3(),
                            itr.second.bone.position.vec3()
                    );
                }
            }

            // Calculate T-Pose final bone transforms
            for (auto& itr : bones_.getAllBones())
            {
                mat4 transform = itr.second.bone.transform;

                std::string parentName = itr.second.parent;

                while (!parentName.empty())
                {
                    transform = bones_.getBone(parentName).result->bone.transform * transform;
                    parentName = bones_.getBone(parentName).result->parent;
                }

                boneTransformations_.insert(
                        std::pair<std::string, mat4>(itr.first, transform)
                );
            }
        }
    }

    void OpenGLModel::render(mat4 transform) const
    {
        for (auto itr = renderedMeshes_.begin(); itr != renderedMeshes_.end(); ++itr)
        {
            Bone* bones = new Bone[1];
            bones[0].transform = boneTransformations_.at(itr->first);

            itr->second->render(transform, bones, 1);

            delete[] bones;
        }
    }

    void OpenGLModel::overrideBoneRotation(const std::string& boneName, vec3 rotation)
    {
        //TODO: Need to translate over the scaling values from the animation frame
        boneTransformationOverrides_[boneName] = GfxMath::CreateTransformation(
                rotation,
                {1, 1, 1},
                bones_.getBone(boneName).result->bone.position.vec3());
    }

    void OpenGLModel::clearBoneOverrides(const std::string& boneName)
    {
        boneTransformationOverrides_.erase(boneName);
    }

    BoneMap OpenGLModel::getBones() const
    {
        return bones_;
    }
}