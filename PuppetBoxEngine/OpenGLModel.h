#pragma once

#include <glad/glad.h>

#include "IModel.h"
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
		OpenGLModel(Mesh mesh, Material material) : mesh_(mesh), material_(material) {};

		void updateModelMatrix(mat3 transform)
		{
			mat4 model = mat4::eye();
			vec3 translate = transform[0];
			vec3 rotation = transform[1];
			vec3 scale = transform[2];

			model = GfxMath::Scale(model, scale);
			model = GfxMath::Translate(model, translate);
			model = GfxMath::Rotate(model, rotation);

			material_.shader.use();
			material_.shader.setMat4("model", model);
			material_.shader.unuse();
		}

		/**
		* \brief Renders the object with OpenGL specific invocations.
		*/
		void render() const
		{
			if (material_.requiresAlphaBlending)
			{
				glEnable(GL_BLEND);
			}
			material_.shader.use();
			material_.diffuseMap.use(GL_TEXTURE0);
			material_.shader.setInt("material.diffuseMap", 0);

			glBindVertexArray(mesh_.VAO);

			if (mesh_.EBO != 0)
			{
				//                           v-- number of indices to draw
				glDrawElements(GL_TRIANGLES, mesh_.drawCount, GL_UNSIGNED_INT, 0);
			}
			else
			{
				//                            v-- number of vertices to draw
				glDrawArrays(GL_TRIANGLES, 0, mesh_.drawCount);
			}

			// Unset VAO for next render
			glBindVertexArray(0);

			material_.diffuseMap.unuse(GL_TEXTURE0);
			material_.shader.unuse();
			glDisable(GL_BLEND);
		}
	private:
		Mesh mesh_;
		Material material_;
	};
}