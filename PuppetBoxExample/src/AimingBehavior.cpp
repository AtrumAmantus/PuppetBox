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
    sceneObject()->clearBoneOverrides("right_shoulder");
    sceneObject()->clearBoneOverrides("right_elbow");
    sceneObject()->clearBoneOverrides("right_hand");
    sceneObject()->clearBoneOverrides("left_shoulder");
    sceneObject()->clearBoneOverrides("left_elbow");
    sceneObject()->clearBoneOverrides("left_hand");
}

void AimingBehavior::update(float deltaTime)
{
    timeSinceLastUpdate_ += deltaTime;

    // Limit update frequency to save frames
    //TODO: Should this be done at a higher level, per component group?
    if (timeSinceLastUpdate_ > timeBetweenUpdates_)
    {
        timeSinceLastUpdate_ = fmod(timeSinceLastUpdate_, timeBetweenUpdates_);

        //TODO: For non-player entities, the targetPosition_ is set externally by network traffic or AI
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

        sceneObject()->overrideBoneRotation("right_shoulder", jointRotations[0][0]);
        sceneObject()->overrideBoneRotation("right_elbow", jointRotations[0][1]);

        sceneObject()->overrideBoneRotation("left_shoulder", jointRotations[1][0]);
        sceneObject()->overrideBoneRotation("left_elbow", jointRotations[1][1]);
    }
}

const PB::vec3 AimingBehavior::getTargetPosition() const
{
    return targetPosition_;
}

void AimingBehavior::inits()
{
    // Get the resting position of all the bones
    // For the sake of the IK calculations, the origin joint (restingJointPositions_[*][0])
    // is considered to be at [0,0,0]

    restingJointPositions_[0][1] = sceneObject()->getBones().getBone("right_elbow").result->bone.position.vec3()
                                   + restingJointPositions_[0][0];
    restingJointPositions_[0][2] = sceneObject()->getBones().getBone("right_hand").result->bone.position.vec3()
                                   + restingJointPositions_[0][1];

    restingJointPositions_[1][1] = sceneObject()->getBones().getBone("left_elbow").result->bone.position.vec3()
                                   + restingJointPositions_[1][0];
    restingJointPositions_[1][2] = sceneObject()->getBones().getBone("left_hand").result->bone.position.vec3()
                                   + restingJointPositions_[1][1];
}