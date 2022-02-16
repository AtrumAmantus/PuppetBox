#pragma once

#include <iostream>

#include "AbstractInputProcessor.h"

class Game3DInputProcessor : public AbstractInputProcessor
{
public:
    Game3DInputProcessor(
            UserInput& userInput,
            const InputActions& inputActions,
            const PB::AbstractInputReader& inputReader)
            : AbstractInputProcessor(userInput, inputActions, inputReader)
    {

    };

    void processInputs() override
    {
        PB::vec3 cameraMoveVec{};
        PB::vec3 cameraRotateVec{};

        if (inputActions().isCommandActive(InputActions::FORWARD) || inputActions().isCommandActive(InputActions::BACKWARD))
        {
            // +forward would be backwards since +z is toward user, so invert it
            cameraMoveVec.z = -(
                    inputActions().isCommandActive(InputActions::FORWARD)
                    + (-1 * inputActions().isCommandActive(InputActions::BACKWARD))
            );
        }

        if (inputActions().isCommandActive(InputActions::RIGHT) || inputActions().isCommandActive(InputActions::LEFT))
        {
            cameraMoveVec.x = (
                    inputActions().isCommandActive(InputActions::RIGHT)
                    + (-1 * inputActions().isCommandActive(InputActions::LEFT))
            );
        }

        if (inputActions().isCommandActive(InputActions::CAMERA_UP) || inputActions().isCommandActive(InputActions::CAMERA_DOWN))
        {
            // +yaw = turning left, so invert x value
            cameraRotateVec.x = -(
                    inputActions().isCommandActive(InputActions::CAMERA_UP)
                    + (-1 * inputActions().isCommandActive(InputActions::CAMERA_DOWN))
            );
        }

        if (inputActions().isCommandActive(InputActions::CAMERA_LEFT) || inputActions().isCommandActive(InputActions::CAMERA_RIGHT))
        {
            cameraRotateVec.y = (
                    inputActions().isCommandActive(InputActions::CAMERA_RIGHT)
                    + (-1 * inputActions().isCommandActive(InputActions::CAMERA_LEFT))
            );
        }

        auto event = std::make_shared<CameraEvent>();
        event->action = [cameraMoveVec, cameraRotateVec](PB::Camera& camera) {
            camera.directionalMove(cameraMoveVec);
            camera.rotate(cameraRotateVec);
        };
    };
};