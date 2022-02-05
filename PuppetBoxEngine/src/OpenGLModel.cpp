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
        mat4 transformation;

        if (animator_ == nullptr)
        {
            Bone bone = bones_.at(boneName).bone;
            transformation = GfxMath::CreateTransformation(bone.rotation, bone.scale, bone.position);
        }
        else
        {
            transformation = animator_->getBoneTransformations().at(boneName);
        }

        return transformation;
    }

    void OpenGLModel::update(float deltaTime)
    {
        if (animator_ != nullptr)
        {
            animator_->update(deltaTime, bones_);

            if (animator_->finished())
            {
                animator_ = nullptr;
            }
        }
    }

    void OpenGLModel::render(mat4 transform) const
    {
        for (auto itr = renderedMeshes_.begin(); itr != renderedMeshes_.end(); ++itr)
        {
            Bone* bones;

            if (animator_ != nullptr)
            {
                //TODO: This won't work for all models, need support for meshes with multiple bones
                bones = new Bone[1];
                auto boneTransformations = animator_->getBoneTransformations();
                bones[0].translation = boneTransformations.at(itr->first);
            }
            else
            {
                // If there is no animation running, just use default "T-Pose" coordinates.
                Bone bone{};
                bone.translation = mat4::eye();
                bones = new Bone[]{bone};
                bones[0].translation = bones_.at(itr->first).bone.translation;
                // TODO: Scaling happens later, this is really messy, fix
                bones[0].translation[0][0] = 1;
                bones[0].translation[1][1] = 1;
                bones[0].translation[2][2] = 1;
            }

            itr->second->render(transform, bones, 1);
        }
    }
}