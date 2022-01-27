#pragma once

#include <glad/glad.h>

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "puppetbox/DataStructures.h"
#include "puppetbox/IAnimationCatalogue.h"
#include "puppetbox/IModel.h"

#include "RenderedMesh.h"

namespace PB
{
    /**
    * \brief OpenGL specific implementation of an IModel object for storing rendering specific data.
    */
    class OpenGLModel : public IModel
    {
    public:
        OpenGLModel(std::unordered_map<std::string, BoneMap> bones,
                    std::unordered_map<std::string, RenderedMesh*> renderedMeshes);

        /**
        * \brief Creates an OpenGL implementation specific object used for storing rendering specific data.
        *
        * \param renderedMeshes Vector of {@link RenderedMesh} objects to use for model rendering.
        */
        OpenGLModel(std::unordered_map<std::string, BoneMap> bones, std::unique_ptr<IAnimator> animator,
                    std::unordered_map<std::string, RenderedMesh*> renderedMeshes);

        void playAnimation(std::unique_ptr<IAnimator> animator, std::uint32_t startFrame) override;

        /**
         * \brief Updates the object's state prior to the render() call.
         *
         * @param deltaTime The time passed since the last update().
         */
        void update(float deltaTime) override;

        /**
        * \brief Renders the object with OpenGL specific invocations.
        */
        void render(mat3 transform) const override;

    private:
        std::unordered_map<std::string, BoneMap> bones_{};
        std::unique_ptr<IAnimator> animator_{nullptr};
        std::unordered_map<std::string, RenderedMesh*> renderedMeshes_{};
    private:
        Bone* buildBoneArray(const std::string& boneName, const std::vector<BoneMap>& boneTransforms) const;
    };
}