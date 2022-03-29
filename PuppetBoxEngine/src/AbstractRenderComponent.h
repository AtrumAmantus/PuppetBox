#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "puppetbox/DataStructures.h"
#include "puppetbox/IRenderComponent.h"

#include "RenderData.h"

namespace PB
{
    class AbstractRenderComponent : public IRenderComponent
    {
    public:
        void init();

        void render() const override;

    private:
        std::vector<RenderData> renderData_;
        std::unordered_map<UUID, std::uint32_t> renderDataMap_;

    private:
        virtual void render(
                const std::vector<mat4>& boneTransforms,
                const Model& model) const = 0;
    };
}