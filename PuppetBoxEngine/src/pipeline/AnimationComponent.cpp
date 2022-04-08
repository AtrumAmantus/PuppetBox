#include "puppetbox/IValueReference.h"

#include "../GfxMath.h"
#include "../PipelineComponents.h"

namespace PB
{
    class BoneTransformMatrixReference : public IValueReference
    {
    public:
        BoneTransformMatrixReference(
                const UUID uuid,
                const std::uint32_t boneId,
                const std::unordered_map<UUID, std::uint32_t>& animatorMap,
                const std::vector<EntityAnimator>& animators
        ) : uuid_(uuid), boneId_(boneId), animatorMap_(animatorMap), animators_(animators)
        {

        }

        mat4 getMat4() const override
        {
            mat4 m = mat4::eye();

            auto itr = animatorMap_.find(uuid_);

            if (itr != animatorMap_.end())
            {
                auto& animator = animators_[itr->second];

                m = animator.boneTransformations.at(boneId_);
            }

            return m;
        }

    private:
        const UUID uuid_;
        const std::uint32_t boneId_;
        const std::unordered_map<UUID, std::uint32_t>& animatorMap_;
        const std::vector<EntityAnimator>& animators_;

    };

    class DefaultAnimator : public IAnimator
    {
    public:
        std::string getAnimationName() const override
        {
            return "No animation loaded.";
        }

        void update(float deltaTime, BoneMap& bones, std::unordered_map<std::uint32_t, mat4> overrides) override
        {
            for (auto& entry : bones.getAllBones())
            {
                transformations_[entry.first] = entry.second.bone.defaultTransform;
            }
        }

        void setCurrentFrame(std::uint32_t frame) override
        {

        }

        const std::unordered_map<std::uint32_t, mat4>& getBoneTransformations() const override
        {
            return transformations_;
        }
    private:
        std::unordered_map<std::uint32_t, mat4> transformations_{};

    };

    void AnimationComponent::inits()
    {
        // Set up listener for pipeline updates to add an entity by its UUID
        auto uuid = MessageBroker::instance().subscribe(PB_EVENT_PIPELINE_ADD_ENTITY_TOPIC, [this](std::shared_ptr<void> data){
            std::unique_lock<std::mutex> mlock = createLock();

            auto event = std::static_pointer_cast<PipelineAddEntityEvent>(data);
            animatorMap_[event->uuid] = animators_.size();
            BoneMap boneMap{};
            vec3 defaultScale = {1.0f, 1.0f, 1.0f};
            Bone bone{{}, defaultScale, {}};
            bone.defaultTransform = GfxMath::CreateTransformation({}, defaultScale, {});
            boneMap.addBone("root", "", bone);
            std::uint32_t boneId = boneMap.getBoneId("root");
            animators_.push_back(
                    EntityAnimator{
                        event->uuid,
                        boneMap,
                        {{boneId, mat4::eye()}},
                        {boneId},
                        std::make_unique<DefaultAnimator>()});
        });

        subscriptions_.push_back(uuid);

        // Set up listener for pipeline updates to animators for a given object by its UUID
        uuid = MessageBroker::instance().subscribe(PB_EVENT_PIPELINE_ADD_ANIMATOR_TOPIC, [this](std::shared_ptr<void> data){
            std::unique_lock<std::mutex> mlock = createLock();

            auto event = std::static_pointer_cast<PipelineAddAnimatorEvent>(data);
            animators_[animatorMap_[event->uuid]].animator = std::move(event->animator);
        });

        subscriptions_.push_back(uuid);

        // Set up listener for pipeline updates to boneMap for a given object by its UUID
        uuid = MessageBroker::instance().subscribe(PB_EVENT_PIPELINE_SET_BONE_MAP_TOPIC, [this](std::shared_ptr<void> data){
            std::unique_lock<std::mutex> mlock = createLock();

            auto event = std::static_pointer_cast<PipelineSetBoneMapEvent>(data);

            // Sort the bones in a predictable way for shader referencing
            std::vector<BoneNode> sortedBones{};
            for (const auto& entry : event->boneMap.getAllBones())
            {
                sortedBones.push_back(entry.second);
            }
            std::sort(
                    sortedBones.begin(),
                    sortedBones.end(),
                    [](const BoneNode& a, const BoneNode& b) {
                        return a.depth < b.depth || (a.depth == b.depth && a.id < b.id);
                    });

            EntityAnimator& animator = animators_[animatorMap_[event->uuid]];

            animator.boneOrder.clear();

            for (const auto& entry : sortedBones)
            {
                animator.boneOrder.push_back(entry.id);
            }

            animator.boneMap = std::move(event->boneMap);
        });

        subscriptions_.push_back(uuid);

        // Set up listener for pipeline requests for bone transform references
        uuid = MessageBroker::instance().subscribe(PB_EVENT_PIPELINE_GET_BONE_TRANSFORM_TOPIC, [this](std::shared_ptr<void> data){
            std::unique_lock<std::mutex> mlock = createLock();

            auto event = std::static_pointer_cast<PipelineGetBoneTransformReferenceEvent>(data);
            auto reference = std::make_shared<BoneTransformMatrixReference>(
                    event->uuid,
                    event->boneId,
                    animatorMap_,
                    animators_);

            event->callback(std::move(reference));
        });

        subscriptions_.push_back(uuid);
    }

    void AnimationComponent::tearDown()
    {
        for (auto subscription : subscriptions_)
        {
            MessageBroker::instance().unsubscribe(subscription);
        }
    }

    void AnimationComponent::updates(float deltaTime)
    {
        for (auto& entityAnimator : animators_)
        {
            entityAnimator.animator->update(deltaTime, entityAnimator.boneMap, {});

            auto event = std::make_shared<PipelineBoneTransformEvent>();
            event->uuid = entityAnimator.uuid;

            entityAnimator.boneTransformations = entityAnimator.animator->getBoneTransformations();

            for (const auto boneId : entityAnimator.boneOrder)
            {
                event->transforms.push_back(entityAnimator.boneTransformations[boneId]);
            }

            MessageBroker::instance().publish(PB_EVENT_PIPELINE_BONE_TRANSFORM_TOPIC, event);
        }
    }
}