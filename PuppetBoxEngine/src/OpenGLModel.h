#pragma once

#include <glad/glad.h>

#include <utility>
#include <vector>

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
		OpenGLModel(std::vector<RenderedMesh> renderedMeshes);

		/**
		* \brief Renders the object with OpenGL specific invocations.
		*/
		void render(mat3 transform) const override;
	private:
		std::vector<RenderedMesh> renderedMeshes_;
	};
}