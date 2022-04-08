#pragma once

#include "puppetbox/IValueReference.h"
#include "puppetbox/RenderData.h"

namespace PB
{
/**
 * \brief Contains the render data for a single complete object, which can be referenced with a {\link PB::UUID}.
 */
    struct SingleRenderData
    {
        UUID uuid;
        mat4 transform;
        std::vector<mat4> boneTransformations;
        std::vector<Model> model;
    };

    struct InstanceDataPiece
    {
        std::uint32_t id;
        UUID attachToUUID;
        std::uint32_t attachToBoneId;
        Transform transform;
        mat4 transformMatrix;
    };

    struct InstanceRenderData
    {
        UUID uuid;
        UUID attachToUUID;
        std::unique_ptr<IValueReference> attachReference;
        Transform transform;
        mat4 transformMatrix;
        Model model;
        std::vector<mat4> instanceTransforms{};
        std::vector<std::uint32_t> instanceMapSample{};
        std::vector<vec2> instanceUVPositions{};
        std::vector<InstanceDataPiece> instanceData{};  //TODO: Sort out this mess...
        std::unordered_map<UUID, std::uint32_t> instanceDataMap{};
        std::unordered_map<UUID, std::vector<std::uint32_t>> attachedToHostUUID{};
    };
}