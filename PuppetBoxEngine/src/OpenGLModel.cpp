#include "OpenGLModel.h"

#include <utility>

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

    void OpenGLModel::playAnimation(std::unique_ptr<IAnimator> animator)
    {
        animator_ = std::move(animator);
    }

    void OpenGLModel::update(float deltaTime)
    {
        if (animator_ != nullptr)
        {
            animator_->update(deltaTime, bones_);
        }
    }

	void OpenGLModel::render(mat3 transform) const
	{
        for(auto itr = renderedMeshes_.begin(); itr != renderedMeshes_.end(); ++itr)
        {
            Bone* bones = nullptr;

            if (animator_ != nullptr)
            {
                //TODO: This won't work for all models, need support for meshes with multiple bones
                bones = new Bone[1];
                auto boneTransformations = animator_->getBoneTransformations();
                bones[0].translation = boneTransformations.at(itr->first);
            }
            else
            {
                Bone bone{};
                bone.translation = mat4::eye();
                bones = new Bone[] {bone};
            }

            itr->second->render(transform, bones, 1);
        }
	}

    Bone* OpenGLModel::buildBoneArray(const std::string& boneName, const std::vector<BoneMap>& boneTransforms) const
    {
        std::string currentBone = boneName;

        std::int32_t lastIndex = bones_.at(boneName).depth + 1;

        Bone* bones = new Bone[lastIndex--];

        while (lastIndex >= 0)
        {
            bones[lastIndex--] = bones_.at(currentBone).bone;
            currentBone = bones_.at(currentBone).parent;
        }

        return bones;
    }
}