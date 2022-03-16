#include <PuppetBox.h>

#include "AimingBehavior.h"
#include "EventDef.h"
#include "MathUtils.h"

AimingBehavior::AimingBehavior(const ScreenTranslator& screenTranslator)
        : PB::AbstractBehavior("aiming_behavior"), screenTranslator_(screenTranslator)
{

}

AimingBehavior::~AimingBehavior()
{
    if (eventUuid_ != PB::UUID::nullUUID())
    {
        PB::Unsubscribe(eventUuid_);
    }

    // Clear all the overrides
    sceneObject()->clearBoneOverrides(boneIds_.rightShoulder);
    sceneObject()->clearBoneOverrides(boneIds_.rightElbow);
    sceneObject()->clearBoneOverrides(boneIds_.leftShoulder);
    sceneObject()->clearBoneOverrides(boneIds_.leftElbow);

    auto event = std::make_shared<PlayerClearBoneOverrideEvent>();
    event->boneId = boneIds_.rightShoulder;
    PB::PublishEvent(PBEX_EVENT_PLAYER_BONE_CLEAR_OVERRIDE, event);

    event = std::make_shared<PlayerClearBoneOverrideEvent>();
    event->boneId = boneIds_.rightElbow;
    PB::PublishEvent(PBEX_EVENT_PLAYER_BONE_CLEAR_OVERRIDE, event);

    event = std::make_shared<PlayerClearBoneOverrideEvent>();
    event->boneId = boneIds_.leftShoulder;
    PB::PublishEvent(PBEX_EVENT_PLAYER_BONE_CLEAR_OVERRIDE, event);

    event = std::make_shared<PlayerClearBoneOverrideEvent>();
    event->boneId = boneIds_.leftElbow;
    PB::PublishEvent(PBEX_EVENT_PLAYER_BONE_CLEAR_OVERRIDE, event);
}

void AimingBehavior::update(float deltaTime)
{
    timeSinceLastUpdate_ += deltaTime;

    // Limit update frequency to save frames
    //TODO: Should this be done at a higher level, per component group?
    if (timeSinceLastUpdate_ > timeBetweenUpdates_)
    {
        timeSinceLastUpdate_ = fmod(timeSinceLastUpdate_, timeBetweenUpdates_);

        //TODO: Still need to figure out how to apply this to other entities...
        // maybe this tied to the client player only?
        if (((Entity*) sceneObject())->isPlayerControlled)
        {
            targetPosition_ = screenTranslator_.cursor.worldCoords;
        }

        //TODO: The origin can't be the model root
        direction_ = MathUtils::Normalize({
                                                  targetPosition_.x - sceneObject()->position.x,
                                                  targetPosition_.y - sceneObject()->position.y
                                          });

        // This is the amount the hands will "reach out"
        //TODO: This needs to be modified by whatever is being held
        PB::vec2 directionOffset = direction_ * 15;

        //TODO: These calculations are wrong, need to picture a "line" between the shoulders
        PB::vec2 ikPosition[2];
        ikPosition[0] = {
                restingJointPositions_[0][0].x + directionOffset.x,
                restingJointPositions_[0][0].y + directionOffset.y
        };
        ikPosition[1] = {
                restingJointPositions_[1][0].x + directionOffset.x,
                restingJointPositions_[1][0].y + directionOffset.y
        };

        PB::mat3 jointRotations[2];
        jointRotations[0] = MathUtils::InverseKinematics3DF(restingJointPositions_[0], ikPosition[0]);
        jointRotations[1] = MathUtils::InverseKinematics3DF(restingJointPositions_[1], ikPosition[1]);

        //TODO: Flip the angles around if origin goes past 180 degrees

        //TODO: Flip the angles if the root rotation is 180 on y-axis

        sceneObject()->overrideBoneRotation(boneIds_.rightShoulder, jointRotations[0][0]);
        sceneObject()->overrideBoneRotation(boneIds_.rightElbow, jointRotations[0][1]);

        sceneObject()->overrideBoneRotation(boneIds_.leftShoulder, jointRotations[1][0]);
        sceneObject()->overrideBoneRotation(boneIds_.leftElbow, jointRotations[1][1]);

        if (((Entity*) sceneObject())->isPlayerControlled)
        {
            auto event = std::make_shared<PlayerBoneOverrideEvent>();
            event->boneId = boneIds_.rightShoulder;
            event->override = jointRotations[0][0];
            PB::PublishEvent(PBEX_EVENT_PLAYER_BONE_OVERRIDE, event);

            event = std::make_shared<PlayerBoneOverrideEvent>();
            event->boneId = boneIds_.rightElbow;
            event->override = jointRotations[0][1];
            PB::PublishEvent(PBEX_EVENT_PLAYER_BONE_OVERRIDE, event);

            event = std::make_shared<PlayerBoneOverrideEvent>();
            event->boneId = boneIds_.leftShoulder;
            event->override = jointRotations[1][0];
            PB::PublishEvent(PBEX_EVENT_PLAYER_BONE_OVERRIDE, event);

            event = std::make_shared<PlayerBoneOverrideEvent>();
            event->boneId = boneIds_.leftElbow;
            event->override = jointRotations[1][1];
            PB::PublishEvent(PBEX_EVENT_PLAYER_BONE_OVERRIDE, event);
        }
    }
}

const PB::vec3 AimingBehavior::getTargetPosition() const
{
    return targetPosition_;
}

void AimingBehavior::inits()
{
    boneIds_.leftShoulder = sceneObject()->getBoneId("left_shoulder");
    boneIds_.leftElbow = sceneObject()->getBoneId("left_elbow");
    boneIds_.leftHand = sceneObject()->getBoneId("left_hand");
    boneIds_.rightShoulder = sceneObject()->getBoneId("right_shoulder");
    boneIds_.rightElbow = sceneObject()->getBoneId("right_elbow");
    boneIds_.rightHand = sceneObject()->getBoneId("right_hand");

    // Get the resting position of all the bones
    // For the sake of the IK calculations, the origin joint (restingJointPositions_[*][0])
    // is considered to be at [0,0,0]

    restingJointPositions_[0][1] = sceneObject()->getBones().getBone(boneIds_.rightElbow).result->bone.position.vec3()
                                   + restingJointPositions_[0][0];
    restingJointPositions_[0][2] = sceneObject()->getBones().getBone(boneIds_.rightHand).result->bone.position.vec3()
                                   + restingJointPositions_[0][1];

    restingJointPositions_[1][1] = sceneObject()->getBones().getBone(boneIds_.leftElbow).result->bone.position.vec3()
                                   + restingJointPositions_[1][0];
    restingJointPositions_[1][2] = sceneObject()->getBones().getBone(boneIds_.leftHand).result->bone.position.vec3()
                                   + restingJointPositions_[1][1];
}