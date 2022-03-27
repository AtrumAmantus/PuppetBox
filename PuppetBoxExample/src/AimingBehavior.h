#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

#include <puppetbox/AbstractBehavior.h>

#include "ScreenTranslator.h"


struct AimingModifier
{
    bool flipHozitonal = false;
    std::string animationName;
    std::unordered_map<std::uint32_t, float> frameInterpolations{};
};

class AimingBehavior : public PB::AbstractBehavior
{
public:
    AimingBehavior(const ScreenTranslator& screenTranslator, AimingModifier aimingModifier);

    ~AimingBehavior();

    void update(float deltaTime) override;

    const PB::vec3 getTargetPosition() const override;

protected:
    void inits() override;

private:
    std::unique_ptr<PB::IAnimator> animator_ = nullptr;
    PB::UUID eventUuid_{};
    PB::vec3 targetPosition_{};
    PB::vec2 direction_{};
    float timeSinceLastUpdate_ = 0;
    // Update 30 times a second
    const float timeBetweenUpdates_ = (1.0f / 30.0f);
    PB::mat3 restingJointPositions_[2]{};
    const ScreenTranslator& screenTranslator_;
    struct
    {
        std::uint32_t rightShoulder;
        std::uint32_t rightElbow;
        std::uint32_t rightHand;
        std::uint32_t leftShoulder;
        std::uint32_t leftElbow;
        std::uint32_t leftHand;
    } boneIds_;
};