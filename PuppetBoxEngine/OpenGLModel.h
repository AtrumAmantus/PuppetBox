#pragma once

#include <glad/glad.h>

#include "IModel.h"
#include "Material.h"
#include "Mesh.h"

namespace PB
{
	class OpenGLModel : public IModel
	{
	public:
		OpenGLModel(Mesh mesh, Material material) : mesh_(mesh), material_(material) {};
		void render() const
		{
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
		}
	private:
		Mesh mesh_;
		Material material_;
	};
}