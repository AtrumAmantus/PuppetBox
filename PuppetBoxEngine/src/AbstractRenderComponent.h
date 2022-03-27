#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
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
     * Contains data for a single model of a parent object, which consists of a mesh and image map collection.
     *
     * The {\link #meshID} references the {\link IGfxApi} specific mesh data.
     */
    struct Model
    {
        std::uint32_t meshID;
        std::uint32_t shaderProgramID;
        std::vector<ImageMap> imageMaps;
    };

    /**
     * Contains the render data for a single complete object, which can be referenced with a {\link PB::UUID}.
     */
    struct RenderData
    {
        std::unordered_map<std::uint32_t, mat4> boneTransformations;
        std::vector<Model> model;
    };

    class AbstractRenderComponent
    {
    public:
        void init() const;

        void render() const;

    private:
        std::vector<RenderData> renderData_;
        std::unordered_map<UUID, std::uint32_t> renderDataMap_;

    private:
        virtual void render(
                const std::unordered_map<std::uint32_t, mat4>& boneTransforms,
                const Model& model) const = 0;
    };
}