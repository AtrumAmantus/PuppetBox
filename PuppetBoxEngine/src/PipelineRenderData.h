#pragma once

#include <memory>
#include <vector>

#include "puppetbox/DataStructures.h"
#include "puppetbox/RenderData.h"
#include "puppetbox/IValueReference.h"

namespace PB
{
/**
 * \brief Contains the render data for a single complete object, which can be referenced with a {\link PB::UUID}.
 */
    struct SingleRenderData
    {
        UUID uuid;
        mat4 transform;
        std::shared_ptr<IValueReference> transformReference;
        std::vector<mat4> boneTransformations;
        std::vector<Model> model;
    };
}