#pragma once

#include <vector>

namespace PB
{
    struct BoneWeight
    {
        std::int32_t boneId;
        float weight;
    };

    /**
    * \brief OpenGL specific mesh object for storing mesh related rendering data to be used
    * in rendering calls.
    */
    struct Mesh
    {
        std::uint32_t vertexVBO = 0;
        std::uint32_t normalVBO = 0;
        std::uint32_t uvVBO = 0;
        std::uint32_t boneIndexVBO = 0;
        std::uint32_t boneWeightVBO = 0;
        std::uint32_t EBO = 0;
        std::uint32_t VAO = 0;
        std::int32_t drawCount = 0;
        std::uint32_t stride = 0;
    };
}