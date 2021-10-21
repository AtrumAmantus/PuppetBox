#include "OpenGLModel.h"

namespace PB
{
	OpenGLModel::OpenGLModel(
            std::unordered_map<std::string, BoneMap> bones,
            std::unordered_map<std::string, RenderedMesh*> renderedMeshes
            ) : bones_(bones), renderedMeshes_(renderedMeshes)
	{

	}

	void OpenGLModel::render(mat3 transform) const
	{
        for(auto itr = renderedMeshes_.begin(); itr != renderedMeshes_.end(); ++itr) {
            itr->second->render(transform, buildBoneArray(itr->first), bones_.at(itr->first).depth + 1);
        }
	}

    Bone* OpenGLModel::buildBoneArray(std::string boneName) const
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