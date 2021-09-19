#pragma once

#include "Material.h"
#include "Mesh.h"

namespace PB
{
    /**
     * Wrapper for {@link Mesh} and {@link Material} objects for a particular renderable mesh.
     */
    class RenderedMesh
    {
    public:
        /**
        * \brief Creates a wrapper for the render data.
        *
        * \param mesh		The OpenGL specific mesh data to use for rendering calls.
        * \param material	The OpenGL specific material data to use for rendering calls.
        */
        RenderedMesh(Mesh mesh, Material material);

        /**
        * \brief Renders the object with OpenGL specific invocations.
        */
        void render(mat3 transform) const;
    private:
        Mesh mesh_;
        Material material_;
    };
}