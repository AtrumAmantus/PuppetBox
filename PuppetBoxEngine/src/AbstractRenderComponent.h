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

namespace PB
{
    class AbstractRenderComponent : public IRenderComponent
    {
    public:
        void init() override;

        void tearDown() override;

        void render() override;

    private:
        std::vector<RenderData> renderData_;
        std::unordered_map<UUID, std::uint32_t> renderDataMap_;
        std::vector<UUID> subscriptions_;
        std::mutex mutex_;

    private:
        virtual void render(
                const std::vector<mat4>& boneTransforms,
                const Model& model,
                const mat4 transform) const = 0;
    };
}