#pragma once

#include <glad/glad.h>

#include <utility>

#include "puppetbox/IModel.h"
#include "GfxMath.h"
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
		OpenGLModel(Mesh mesh, Material material) : mesh_(mesh), material_(std::move(material)) {};

		/**
		* \brief Renders the object with OpenGL specific invocations.
		*/
		void render(mat3 transform) const override
		{
			if (material_.requiresAlphaBlending)
			{
				glEnable(GL_BLEND);
			}
			material_.shader.use();
			material_.diffuseMap.use(GL_TEXTURE0);
			material_.shader.setInt("material.diffuseMap", 0);

			vec4 diffuseUvAdjust{
				static_cast<float>(material_.diffuseData.width) / static_cast<float>(material_.diffuseMap.width),
				static_cast<float>(material_.diffuseData.height) / static_cast<float>(material_.diffuseMap.height),
				static_cast<float>(material_.diffuseData.xOffset) / static_cast<float>(material_.diffuseMap.width),
			    static_cast<float>(material_.diffuseData.yOffset) / static_cast<float>(material_.diffuseMap.height)
			};

			material_.shader.setVec4("diffuseUvAdjust", diffuseUvAdjust);

            updateModelMatrix(transform);

			glBindVertexArray(mesh_.VAO);

			if (mesh_.EBO != 0)
			{
				//                           v-- number of indices to draw
				glDrawElements(GL_TRIANGLES, mesh_.drawCount, GL_UNSIGNED_INT, 0); // NOLINT(modernize-use-nullptr)
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
	private:
        void updateModelMatrix(mat3 transform) const
        {
            mat4 model = mat4::eye();

            model = GfxMath::Translate(model, transform[0]);
            model = GfxMath::Scale(model, transform[2]);
            model = GfxMath::Rotate(model, transform[1]);

            material_.shader.setMat4("model", model);
        }
	};
}