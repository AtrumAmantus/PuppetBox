#include "Engine.h"

namespace PB
{
    void Engine::run()
    {
        hardwareInitializer_.initializeGameTime();

        while (!inputReader_.window.windowClose)
        {
            float deltaTime = hardwareInitializer_.updateElapsedTime();

            processInput();

            gfxApi_.preLoopCommands();

            scene_->update(deltaTime);
            scene_->render();

            hardwareInitializer_.postLoopCommands();
        }
    }

    void Engine::shutdown()
    {
        hardwareInitializer_.destroy();
    }

    void Engine::setScene(SceneGraph* scene)
    {
        scene_ = scene;
    }

    void Engine::processInput()
    {
        inputReader_.loadCurrentState();

        if (inputReader_.window.newWidth != 0 || inputReader_.window.newHeight != 0)
        {
            gfxApi_.setRenderDimensions(inputReader_.window.newWidth, inputReader_.window.newHeight);
        }

        scene_->processInput();
    }
}