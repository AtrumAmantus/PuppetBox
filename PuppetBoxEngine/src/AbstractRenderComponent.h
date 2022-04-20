#pragma once

#include <cstdint>
#include <map>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "puppetbox/DataStructures.h"
#include "puppetbox/IRenderComponent.h"
#include "puppetbox/RenderData.h"

#include "PipelineRenderData.h"

namespace PB
{
    class AbstractRenderComponent : public IRenderComponent
    {
    public:
        void init() override;

        void tearDown() override;

        void render() override;

    private:
        std::vector<SingleRenderData> singleRenderData_{};
        std::unordered_map<UUID, std::uint32_t> singleRenderDataMap_{};
        std::vector<InstanceRenderData> instanceRenderData_{};
        std::unordered_map<UUID, std::uint32_t> instanceRenderDataMap_{};
        std::vector<UUID> subscriptions_;
        std::mutex mutex_;

    private:
        virtual void render(
                const std::vector<mat4>& boneTransforms,
                const Model& model,
                const mat4 transform) const = 0;

        virtual void renderInstance(
                const Model& model,
                const mat4 transform,
                const std::vector<mat4>& instanceTransforms,
                const std::vector<mat4>& instanceData) const = 0;
    };
}