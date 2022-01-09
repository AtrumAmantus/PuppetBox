#pragma once

#include "Material.h"
#include "Mesh.h"
#include "RenderedMesh.h"

namespace PB
{
    /**
     * Wrapper for {@link Mesh} and {@link Material} objects for a particular rendered mesh.
     */
    class Rendered2DMesh : public RenderedMesh
    {
    public:
        /**
        * \brief Creates a wrapper for the render data.
        *
        * \param mesh		The OpenGL specific mesh data to use for rendering calls.
        * \param material	The OpenGL specific material data to use for rendering calls.
        */
        Rendered2DMesh(Mesh mesh, Material material);

        /**
        * \brief Renders the object with OpenGL specific invocations.
        */
        void render(mat3 transform, Bone* bones, std::uint32_t boneCount) const;

    private:
        Mesh mesh_;
        Material material_;
    };
}