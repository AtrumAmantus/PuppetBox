#pragma once

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "puppetbox/DataStructures.h"
#include "puppetbox/IRenderComponent.h"
#include "puppetbox/RenderData.h"

#include "Pipeline.h"

namespace PB
{
    class AbstractRenderComponent : public IRenderComponent
    {
    public:
        ~AbstractRenderComponent() noexcept override;

        void init() override;

        void render() const override;

        void addDataVector(std::string referenceName, std::shared_ptr<void> reference) override;

        void setEntityMap(std::unordered_map<UUID, std::uint32_t>* entityMap) override;

        void sync(const std::function<void()>& process) override;

        void lock() override;

        void unlock() override;

    private:
        std::vector<UUID> subscriptions_{};
        std::unordered_map<UUID, std::uint32_t>* entityMap_ = nullptr;
        std::shared_ptr<std::vector<SingleRenderData>> singleRenders_;
        std::shared_ptr<std::vector<InstanceRenderData>> instanceRenders_;
        std::mutex mutex_;
        bool isLocked_ = false;

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