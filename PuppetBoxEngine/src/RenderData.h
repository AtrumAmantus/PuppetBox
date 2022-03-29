#pragma once

#include <cstdint>
#include <vector>

#include "puppetbox/DataStructures.h"

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
        std::uint32_t imageMapID;
        ImageMapType mapType = DIFFUSE;
    };

    /**
     * \brief Contains data for a single model of a parent object, which consists of a mesh and image map collection.
     *
     * The {\link #meshID} references the {\link IGfxApi} specific mesh data.
     */
    struct Model
    {
        std::uint32_t meshID;
        mat4 meshTransform;
        std::uint32_t shaderProgramID;
        std::vector<ImageMap> imageMaps;
    };

    /**
     * \brief Contains the render data for a single complete object, which can be referenced with a {\link PB::UUID}.
     */
    struct RenderData
    {
        UUID uuid;
        std::vector<mat4> boneTransformations;
        std::vector<Model> model;
    };
}