#pragma once

#include <cstdint>
#include <queue>
#include <string>
#include <unordered_map>

#include <puppetbox/SceneObject.h>
#include <puppetbox/WanderBehavior.h>

#include "Command.h"
#include "Constants.h"
#include "MathUtils.h"

//TODO: Need to refactor to composition rather then add all these static properties
class Entity : public PB::SceneObject
{
public:
    PB::UUID equippedItem = PB::UUID::nullUUID();
    std::string name;
    std::uint8_t health = 50;
    std::unordered_map<std::uint32_t, PB::UUID> inventory{};
    //TODO: Hacky?
    bool isPlayerControlled = false;
public:
    Entity()
    {
        commands_.insert(std::pair<Command::Command, bool>{Command::FORWARD, false});
        commands_.insert(std::pair<Command::Command, bool>{Command::BACKWARD, false});
        commands_.insert(std::pair<Command::Command, bool>{Command::LEFT, false});
        commands_.insert(std::pair<Command::Command, bool>{Command::RIGHT, false});
    };

    void setCommandState(CommandState commandState)
    {
        nextCommands_.push(commandState);
    };

    void setPosition(PB::vec3 position)
    {
        positionUpdate_ = position;
        updatePosition_ = true;
    };

protected:
    void updates(float deltaTime) override
    {
        if (updatePosition_)
        {
            updatePosition_ = false;
            position = positionUpdate_;
        }

        while (!nextCommands_.empty())
        {
            auto commandState = nextCommands_.front();
            nextCommands_.pop();
            auto entry = commands_.find(commandState.command);
            entry->second = commandState.activated;
        }

        moveVector.x = commands_.at(Command::RIGHT) - commands_.at(Command::LEFT);
        moveVector.y = commands_.at(Command::FORWARD) - commands_.at(Command::BACKWARD);

        if (moveVector.y > 0)
        {
            rotation.y = PI;
        }
        else
        {
            rotation.y = 0;
        }

        if (moveVector.x != 0 || moveVector.y != 0)
        {
            if (!isMoving_)
            {
                isMoving_ = true;
                this->playAnimation(Constants::Animation::kWalk, 0);
            }
        }
        else
        {
            if (isMoving_)
            {
                isMoving_ = false;
                this->stopAnimation(Constants::Animation::kWalk);
            }
        }
    };

    void behaviorEvent(std::string behaviorName, std::uint32_t behaviorEvent) override
    {
        if (behaviorName == "wander")
        {
            if (behaviorEvent == PB::WanderBehavior::Events::START)
            {
                this->playAnimation(Constants::Animation::kWalk, 0);
            }
            else if (behaviorEvent == PB::WanderBehavior::Events::STOP)
            {
                this->stopAnimation(Constants::Animation::kWalk);

                if (rand() % 3 == 1)
                {
                    this->playAnimation(Constants::Animation::kIdle0, 0);
                }
            }
        }
    };

private:
    bool isMoving_ = false;
    PB::vec3 positionUpdate_{};
    bool updatePosition_ = false;
    std::unordered_map<Command::Command, bool> commands_{};
    std::queue<CommandState> nextCommands_{};
};
