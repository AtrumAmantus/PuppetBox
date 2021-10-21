#pragma once

#include "puppetbox/DataStructures.h"

namespace PB
{
    /**
     * Wrapper for {@link Mesh} and {@link Material} objects for a particular rendered mesh.
     */
    class RenderedMesh
    {
    public:
        /**
        * \brief Renders the object with OpenGL specific invocations.
        */
        virtual void render(mat3 transform, Bone* bones, std::uint32_t boneCount) const = 0;
    };
}