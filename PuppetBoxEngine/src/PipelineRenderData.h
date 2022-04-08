#pragma once

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
}