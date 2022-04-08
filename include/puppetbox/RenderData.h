#pragma once

#include <cstdint>
#include <vector>

#include "DataStructures.h"

namespace PB
{
    enum ImageMapType
    {
        DIFFUSE,
        EMISSION,
        NORMAL,
    };

    /**
     * \brief Contains data for a single image map, which can be a diffuseMap, emissionMap, etc.
     *
     * The {\link #imageMapID} references the {\link IGfxApi} specific image data.
     */
    struct ImageMap
    {
        std::uint32_t imageMapID = 0;
        ImageMapType mapType = DIFFUSE;
    };

    /**
     * \brief Contains data for a single model of a parent object, which consists of a mesh and image map collection.
     *
     * The {\link #meshID} references the {\link IGfxApi} specific mesh data.
     */
    struct Model
    {
        std::uint32_t meshID = 0;
        mat4 meshTransform{};
        std::uint32_t shaderProgramID = 0;
        std::vector<ImageMap> imageMaps{};
    };
}