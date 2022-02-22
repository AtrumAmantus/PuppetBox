#pragma once

#include <iostream>

#include "AbstractInputProcessor.h"
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
            event->action = [this](PB::Camera& camera){
                camera.zoom(static_cast<std::int8_t>(input()->mouse.wheelYDir));
            };

            PB::PublishEvent(Event::Topic::CAMERA_TOPIC, event);
        }

        PB::vec3 moveVec{};
        PB::vec3 cameraMoveVec{};

        if (inputActions().isCommandActive(InputActions::FORWARD) || inputActions().isCommandActive(InputActions::BACKWARD))
        {
            moveVec.y = (
                    inputActions().isCommandActive(InputActions::FORWARD)
                    + (-1 * inputActions().isCommandActive(InputActions::BACKWARD))
            );
        }

        if (inputActions().isCommandActive(InputActions::RIGHT) || inputActions().isCommandActive(InputActions::LEFT))
        {
            moveVec.x = (
                    inputActions().isCommandActive(InputActions::RIGHT)
                    + (-1 * inputActions().isCommandActive(InputActions::LEFT))
            );
        }

        if (inputActions().isCommandActive(InputActions::CAMERA_UP) || inputActions().isCommandActive(InputActions::CAMERA_DOWN))
        {
            cameraMoveVec.y = (
                    inputActions().isCommandActive(InputActions::CAMERA_UP)
                    + (-1 * inputActions().isCommandActive(InputActions::CAMERA_DOWN))
            );
        }

        if (inputActions().isCommandActive(InputActions::CAMERA_LEFT) || inputActions().isCommandActive(InputActions::CAMERA_RIGHT))
        {
            cameraMoveVec.x = (
                    inputActions().isCommandActive(InputActions::CAMERA_RIGHT)
                    + (-1 * inputActions().isCommandActive(InputActions::CAMERA_LEFT))
            );
        }

        auto cameraEvent = std::make_shared<CameraEvent>();
        cameraEvent->action = [cameraMoveVec](PB::Camera& camera) {
            camera.move(cameraMoveVec);
        };

        PB::PublishEvent(Event::Topic::CAMERA_TOPIC, cameraEvent);

        auto playerEvent = std::make_shared<PlayerEvent>();
        playerEvent->action = [moveVec](Entity& entity) {
            entity.moveVector = moveVec;
        };

        PB::PublishEvent(Event::Topic::PLAYER_TOPIC, playerEvent);
    };
};