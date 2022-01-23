#pragma once

namespace PB
{
    /**
    * \brief OpenGL specific mesh object for storing mesh related rendering data to be used
    * in rendering calls.
    */
    struct Mesh
    {
        std::uint32_t VBO = 0;
        std::uint32_t EBO = 0;
        std::uint32_t VAO = 0;
        std::int32_t drawCount = 0;
        vec3 scale{1, 1, 1};
        std::uint32_t stride = 0;
    };
}