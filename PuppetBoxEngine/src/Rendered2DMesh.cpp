#include "Rendered2DMesh.h"

#include "GfxMath.h"

namespace PB
{
    Rendered2DMesh::Rendered2DMesh(Mesh mesh, Material material) : mesh_(mesh), material_(material)
    {

    }

    void Rendered2DMesh::render(mat4 transform, Bone* bones, std::uint32_t boneCount) const
    {
        if (material_.requiresAlphaBlending)
        {
            glEnable(GL_BLEND);
        }

        material_.shader.use();
        material_.diffuseMap.use(0);
        material_.shader.setInt("material.diffuseMap", 0);

        vec4 diffuseUvAdjust{
                static_cast<float>(material_.diffuseData.width) / static_cast<float>(material_.diffuseMap.width),
                static_cast<float>(material_.diffuseData.height) / static_cast<float>(material_.diffuseMap.height),
                static_cast<float>(material_.diffuseData.xOffset) / static_cast<float>(material_.diffuseMap.width),
                static_cast<float>(material_.diffuseData.yOffset) / static_cast<float>(material_.diffuseMap.height)
        };

        material_.shader.setVec4("diffuseUvAdjust", diffuseUvAdjust);
        material_.shader.setMat4("boneTransform", bones[0].transform);
        material_.shader.setMat4("meshTransform", mesh_.transform);
        material_.shader.setMat4("model", transform);

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

        material_.diffuseMap.unuse(0);
        material_.shader.unuse();
        glDisable(GL_BLEND);
    }
}