#include "OpenGLModel.h"

#include <utility>

#include "GfxMath.h"

namespace PB
{
    OpenGLModel::OpenGLModel(
            std::unordered_map<std::string, BoneMap> bones,
            std::unordered_map<std::string, RenderedMesh*> renderedMeshes
    ) : bones_(std::move(bones)), animator_(nullptr), renderedMeshes_(std::move(renderedMeshes))
    {

    }

    OpenGLModel::OpenGLModel(
            std::unordered_map<std::string, BoneMap> bones,
            std::unique_ptr<IAnimator> animator,
            std::unordered_map<std::string, RenderedMesh*> renderedMeshes
    ) : bones_(std::move(bones)), animator_(std::move(animator)), renderedMeshes_(std::move(renderedMeshes))
    {

    }

    void OpenGLModel::playAnimation(std::unique_ptr<IAnimator> animator, std::uint32_t startFrame)
    {
        animator_ = std::move(animator);

        if (animator_ != nullptr)
        {
            animator_->setCurrentFrame(startFrame);
        }
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
        if (animator_ != nullptr)
        {
            animator_->update(deltaTime, bones_);

            boneTransformations_ = animator_->getBoneTransformations();
        }
        else
        {
            //TODO: not supporting mix animations atm

            // Update T-Pose local bone transforms
            for (auto itr = bones_.begin(); itr != bones_.end(); ++itr)
            {
                itr->second.bone.transform = GfxMath::CreateTransformation(
                        itr->second.bone.rotation,
                        itr->second.bone.scale,
                        itr->second.bone.position
                );
            }

            boneTransformations_.clear();

            // Calculate T-Pose final bone transforms
            for (auto itr = bones_.begin(); itr != bones_.end(); ++itr)
            {
                mat4 transform = itr->second.bone.transform;

                std::string parentName = itr->second.parent;

                while (!parentName.empty())
                {
                    transform = bones_.at(parentName).bone.transform * transform;
                    parentName = bones_.at(parentName).parent;
                }

                boneTransformations_.insert(
                        std::pair<std::string, mat4>(itr->first, transform)
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
}