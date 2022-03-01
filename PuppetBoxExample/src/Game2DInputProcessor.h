#pragma once

#include <iostream>

#include "AbstractInputProcessor.h"
#include "Command.h"
#include "EventDef.h"

class Game2DInputProcessor : public AbstractInputProcessor
{
public:
    Game2DInputProcessor(
            UserInput& userInput,
            const InputActions& inputActions,
            const std::shared_ptr<PB::AbstractInputReader>& inputReader)
            : AbstractInputProcessor(userInput, inputActions, inputReader)
    {

    };

    void staticProcessInputs() override
    {
        if (input()->mouse.isReleased(BTN_LEFT))
        {
            std::cout << "Clicked at: " << input()->mouse.x << ", "
                      << input()->mouse.y << std::endl;
        }
    }

    void processInputs() override
    {
        if (input()->mouse.wheelYDir != 0)
        {
            auto event = std::make_shared<CameraEvent>();
            event->action = [this](PB::Camera& camera) {
                camera.zoom(static_cast<std::int8_t>(input()->mouse.wheelYDir));
            };

            PB::PublishEvent(Event::Topic::CAMERA_TOPIC, event);
        }

        PB::vec3 moveVec{};
        PB::vec3 cameraMoveVec{};

        std::uint32_t commandIndex = 0;
        CommandState commandStates[4]{};

        bool isActive;

        if ((isActive = (inputActions().isCommandActivated(Command::FORWARD))) ||
            inputActions().isCommandReleased(Command::FORWARD))
        {
            commandStates[commandIndex++] = {Command::FORWARD, isActive};
        }

        if ((isActive = (inputActions().isCommandActivated(Command::BACKWARD))) ||
            inputActions().isCommandReleased(Command::BACKWARD))
        {
            commandStates[commandIndex++] = {Command::BACKWARD, isActive};
        }

        if ((isActive = (inputActions().isCommandActivated(Command::LEFT))) ||
            inputActions().isCommandReleased(Command::LEFT))
        {
            commandStates[commandIndex++] = {Command::LEFT, isActive};
        }

        if ((isActive = (inputActions().isCommandActivated(Command::RIGHT))) ||
            inputActions().isCommandReleased(Command::RIGHT))
        {
            commandStates[commandIndex++] = {Command::RIGHT, isActive};
        }

        if (inputActions().isCommandActive(Command::CAMERA_UP) || inputActions().isCommandActive(Command::CAMERA_DOWN))
        {
            cameraMoveVec.y = (
                    inputActions().isCommandActive(Command::CAMERA_UP)
                    + (-1 * inputActions().isCommandActive(Command::CAMERA_DOWN))
            );
        }

        if (inputActions().isCommandActive(Command::CAMERA_LEFT) ||
            inputActions().isCommandActive(Command::CAMERA_RIGHT))
        {
            cameraMoveVec.x = (
                    inputActions().isCommandActive(Command::CAMERA_RIGHT)
                    + (-1 * inputActions().isCommandActive(Command::CAMERA_LEFT))
            );
        }

        auto cameraEvent = std::make_shared<CameraEvent>();
        cameraEvent->action = [cameraMoveVec](PB::Camera& camera) {
            camera.move(cameraMoveVec);
        };

        PB::PublishEvent(Event::Topic::CAMERA_TOPIC, cameraEvent);

        if (commandIndex > 0)
        {
            auto playerEvent = std::shared_ptr<PlayerEvent>(
                    (PlayerEvent*) malloc(sizeof(PlayerEvent) + (commandIndex * sizeof(CommandState))));
            playerEvent->commandCount = commandIndex;

            for (std::uint32_t i = 0; i < playerEvent->commandCount; ++i)
            {
                playerEvent->commandStates[i] = commandStates[i];
            }

            PB::PublishEvent(Event::Topic::PLAYER_TOPIC, playerEvent);
        }
    };
};