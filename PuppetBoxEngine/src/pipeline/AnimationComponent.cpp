#include "puppetbox/IValueReference.h"

#include "../GfxMath.h"
#include "../MessageBroker.h"
#include "../Pipeline.h"
#include "../PipelineEvents.h"

namespace PB
{
    class AbsoluteBonePositionReference : public IValueReference
    {
    public:
        AbsoluteBonePositionReference(
                const UUID hostUUID,
                const std::uint32_t boneId,
                const std::vector<EntityAnimator>& animators,
                const std::vector<SingleRenderData>& singleRenders,
                const std::unordered_map<UUID, std::uint32_t>& entityMap) :
                hostUUID_(hostUUID),
                boneId_(boneId),
                animators_(animators),
                singleRenders_(singleRenders),
                entityMap_(entityMap)
        {

        };

        mat4 getMat4() const override
        {
            auto INDEX = entityMap_.at(hostUUID_);
            auto boneMat = animators_.at(INDEX).boneTransformations.at(boneId_);
            return singleRenders_.at(INDEX).transformReference->getMat4() * boneMat;
        };

    private:
        const UUID hostUUID_;
        const std::uint32_t boneId_;
        const std::vector<EntityAnimator>& animators_;
        const std::vector<SingleRenderData>& singleRenders_;
        const std::unordered_map<UUID, std::uint32_t>& entityMap_;
    };

    AnimationComponent::~AnimationComponent() noexcept
    {
        for (const auto& subscription : subscriptions_)
        {
            MessageBroker::instance().unsubscribe(subscription);
        }
    }

    void AnimationComponent::init()
    {
        auto uuid = MessageBroker::instance().subscribe(
                PB_EVENT_PIPELINE_SET_BONE_MAP_TOPIC,
                [this](std::shared_ptr<void> data) {
                    auto event = std::static_pointer_cast<PipelineSetBoneMapEvent>(data);

                    sync([this, event]() {
                        animators_->at(entityMap().at(event->uuid)).boneMap = event->boneMap;
                    });
                });

        subscriptions_.push_back(uuid);

        uuid = MessageBroker::instance().subscribe(
                PB_EVENT_PIPELINE_ATTACH_OBJECT_TO_TOPIC,
                [this](std::shared_ptr<void> data) {
                    auto event = std::static_pointer_cast<PipelineAttachObjectTo>(data);

                    sync([this, event]() {
                        singleRenders_->at(entityMap().at(event->parasiteUUID)).transformReference =
                            std::make_shared<AbsoluteBonePositionReference>(
                                event->hostUUID,
                                event->attachPointId,
                                *animators_,
                                *singleRenders_,
                                entityMap()
                            );
                    });
                });

        subscriptions_.push_back(uuid);

        uuid = MessageBroker::instance().subscribe(
                PB_EVENT_PIPELINE_ADD_ANIMATOR_TOPIC,
                [this](std::shared_ptr<void> data) {
                    auto event = std::static_pointer_cast<PipelineAddAnimatorEvent>(data);

                    animators_->at(entityMap().at(event->uuid)).animator = std::move(event->animator);
                });

        subscriptions_.push_back(uuid);
    }

    void AnimationComponent::update(float deltaTime)
    {
        for (std::uint32_t i = 0; i < animators_->size(); ++i)
        {
            auto& entityAnimator = animators_->at(i);
            entityAnimator.animator->update(deltaTime, entityAnimator.boneMap, {});
            entityAnimator.boneTransformations = entityAnimator.animator->getBoneTransformations();

            singleRenders_->at(i).boneTransformations.clear();

            for (const auto boneId : entityAnimator.boneMap.getBoneOrder())
            {
                singleRenders_->at(i).boneTransformations.push_back(entityAnimator.boneTransformations[boneId]);
            }
        }
    }

    void AnimationComponent::addVectorReference(std::string referenceName, std::shared_ptr<void> reference)
    {
        if (referenceName == "pb_single_render")
        {
            singleRenders_ = std::move(std::static_pointer_cast<std::vector<SingleRenderData>>(reference));
        }
        else if (referenceName == "pb_animator")
        {
            animators_ = std::move(std::static_pointer_cast<std::vector<EntityAnimator>>(reference));
        }
    }
}