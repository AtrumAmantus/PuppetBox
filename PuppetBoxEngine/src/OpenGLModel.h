#pragma once

#include <glad/glad.h>

#include <string>
#include <unordered_map>
#include <utility>

#include "puppetbox/DataStructures.h"

#include "IModel.h"
#include "RenderedMesh.h"

namespace PB
{
	/**
	* \brief OpenGL specific implementation of an IModel object for storing rendering specific data.
	*/
	class OpenGLModel : public IModel
	{
	public:
		/**
		* \brief Creates an OpenGL implementation specific object used for storing rendering specific data.
		* 
		* \param renderedMeshes Vector of {@link RenderedMesh} objects to use for model rendering.
		*/
		OpenGLModel(std::unordered_map<std::string, BoneMap> bones, std::unordered_map<std::string, RenderedMesh*> renderedMeshes);

		/**
		* \brief Renders the object with OpenGL specific invocations.
		*/
		void render(mat3 transform) const override;
	private:
        std::unordered_map<std::string, BoneMap> bones_{};
		std::unordered_map<std::string, RenderedMesh*> renderedMeshes_{};
    private:
        Bone* buildBoneArray(std::string boneName) const;
	};
}