#pragma once

#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "puppetbox/AbstractObjectComponent.h"
#include "puppetbox/DataStructures.h"
#include "puppetbox/IAnimationCatalogue.h"
#include "puppetbox/IPipeline.h"
#include "puppetbox/IRenderComponent.h"
#include "puppetbox/IValueReference.h"
#include "puppetbox/RenderData.h"
#include "puppetbox/TypeDef.h"

namespace PB
{
    class DefaultAnimator : public IAnimator
    {
    public:
        std::string getAnimationName() const override;

        void update(float deltaTime, BoneMap& bones, std::unordered_map<std::uint32_t, mat4> overrides) override;

        void setCurrentFrame(std::uint32_t frame) override;

        const std::unordered_map<std::uint32_t, mat4>& getBoneTransformations() const override;

    private:
        std::unordered_map<std::uint32_t, mat4> transformations_{};

    };

    struct EntityAnimator
    {
        UUID uuid;
        BoneMap boneMap;
        std::unordered_map<std::uint32_t, mat4> boneTransformations{};
        std::unique_ptr<IAnimator> animator = std::make_unique<DefaultAnimator>();
    };

    /**
     * \brief Contains the render data for a single complete object, which can be referenced with a {\link PB::UUID}.
     */
    struct SingleRenderData
    {
        UUID uuid{};
        mat4 transform = mat4::eye();
        std::shared_ptr<IValueReference> transformReference{};
        std::vector<mat4> boneTransformations{};
        std::vector<Model> models{};
    };

    struct InstanceRenderData
    {
        UUID uuid;
        mat4 transform;
        std::shared_ptr<IValueReference> transformReference;
        Model model;
        std::unordered_map<UUID, std::uint32_t> instanceMap{};
        std::vector<mat4> instanceTransforms{};
        std::vector<mat4> instanceData{};
    };

    class DefaultTransformMatrixReference : public IValueReference
    {
    public:
        DefaultTransformMatrixReference(
                const UUID uuid,
                const std::unordered_map<UUID, std::uint32_t>& uuidMap,
                const std::vector<SingleRenderData>& singleRenderVector
        ) : uuid_(uuid), uuidMap_(uuidMap), singleRenderVector_(singleRenderVector)
        {

        }

        mat4 getMat4() const override
        {
            return singleRenderVector_.at(uuidMap_.at(uuid_)).transform;
        }

    private:
        const UUID uuid_;
        const std::unordered_map<UUID, std::uint32_t>& uuidMap_;
        const std::vector<SingleRenderData>& singleRenderVector_;
    };

    class ActionComponent : public AbstractObjectComponent
    {
    public:
        void update(float deltaTime) override;
    };

    class AIComponent : public AbstractObjectComponent
    {
    public:
        void update(float deltaTime) override;
    };

    class AnimationComponent : public AbstractObjectComponent
    {
    public:
        ~AnimationComponent() noexcept;

        void init() override;

        void update(float deltaTime) override;

        void addVectorReference(std::string referenceName, std::shared_ptr<void> reference) override;

    private:
        std::vector<UUID> subscriptions_{};
        std::shared_ptr<std::vector<EntityAnimator>> animators_;
        std::shared_ptr<std::vector<SingleRenderData>> singleRenders_;
    };

    class PhysicsComponent : public AbstractObjectComponent
    {
    public:
        void update(float deltaTime) override;
    };

    struct ObjectPositionUpdate {
        UUID uuid;
        vec3 position;
    };

    struct ObjectTransformReferenceUpdate {
        UUID hostUUID;
        UUID parasiteUUID;
        std::unique_ptr<IValueReference> reference;
    };

    class PositionComponent : public AbstractObjectComponent
    {
    public:
        ~PositionComponent() noexcept;

        void init() override;

        void update(float deltaTime) override;

        void addVectorReference(std::string referenceName, std::shared_ptr<void> reference) override;

    private:
        std::vector<UUID> subscriptions_{};
        PB::Concurrent::NonBlocking::Queue<ObjectPositionUpdate> queuedPositionUpdates_{};
        std::shared_ptr<std::vector<Transform>> transforms_;
        std::shared_ptr<std::vector<SingleRenderData>> singleRenders_;

    };

    class TransformPipelineData : public AbstractPipelineData
    {
    public:
        void addData(const UUID uuid) override;

        void removeDataAt(std::uint32_t index) override;

        const std::string getReference() const override;

        std::shared_ptr<void> getDataVector() const override;

    private:
        std::shared_ptr<std::vector<Transform>> transformVector_ = std::make_shared<std::vector<Transform>>();

    };

    class AnimatorPipelineData : public AbstractPipelineData
    {
    public:
        void addData(const UUID uuid) override;

        void removeDataAt(std::uint32_t index) override;

        const std::string getReference() const override;

        std::shared_ptr<void> getDataVector() const override;

    private:
        std::shared_ptr<std::vector<EntityAnimator>> animatorVector_ = std::make_shared<std::vector<EntityAnimator>>();

    };

    class SingleRenderPipelineData : public AbstractPipelineData
    {
    public:
        void addData(const UUID uuid) override;

        void removeDataAt(std::uint32_t index) override;

        const std::string getReference() const override;

        std::shared_ptr<void> getDataVector() const override;

    private:
        std::shared_ptr<std::vector<SingleRenderData>> singleRenderVector_ =
                std::make_shared<std::vector<SingleRenderData>>();
    };

    class Pipeline : public IPipeline
    {
    public:
        ~Pipeline() noexcept override;

        void init() override;

        void tearDown() override;

        void update(float deltaTime) override;

        void render() override;

        void lockPipeline() override;

        void unlockPipeline() override;

        void addComponent(
                std::unique_ptr<AbstractObjectComponent> component,
                std::vector<std::string> vectorReferences) override;

        void setRenderComponent(
                std::unique_ptr<IRenderComponent> renderComponent,
                const std::vector<std::string>& vectorReferences) override;

        void addPipelineData(std::unique_ptr<AbstractPipelineData> pipelineData) override;

    private:
        std::vector<UUID> subscriptions_{};
        std::unordered_map<std::string, std::shared_ptr<void>> dataVectorMap_{};
        std::unordered_map<std::string, std::unique_ptr<AbstractPipelineData>> pipelineDataMap_{};
        std::unordered_map<UUID, std::uint32_t> entityMap_{};
        std::uint32_t entityCount_ = 0;
        std::vector<std::unique_ptr<AbstractObjectComponent>> objectComponents_{};
        std::unique_ptr<IRenderComponent> renderComponent_{nullptr};
        std::mutex mutex_;
        bool componentsLocked_ = false;
        bool isLocked_ = false;
    };
}