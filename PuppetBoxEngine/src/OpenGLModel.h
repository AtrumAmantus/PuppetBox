#pragma once

#include <glad/glad.h>

#include <utility>

#include "IModel.h"
#include "puppetbox/DataStructures.h"
#include "Material.h"
#include "Mesh.h"

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
		* \param mesh		The OpenGL specific mesh data to use for rendering calls.
		* \param material	The OpenGL specific material data to use for rendering calls.
		*/
		OpenGLModel(Mesh mesh, Material material);

		/**
		* \brief Renders the object with OpenGL specific invocations.
		*/
		void render(mat3 transform) const override;
	private:
		Mesh mesh_;
		Material material_;
	};
}