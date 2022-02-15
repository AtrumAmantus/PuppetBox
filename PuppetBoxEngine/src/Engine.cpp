#include "Engine.h"

namespace PB
{
    void Engine::run()
    {
        hardwareInitializer_.initializeGameTime();

        while (!inputProcessor_.window.windowClose)
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
        inputProcessor_.loadCurrentState();

        if (inputProcessor_.window.newWidth != 0 || inputProcessor_.window.newHeight != 0)
        {
            gfxApi_.setRenderDimensions(inputProcessor_.window.newWidth, inputProcessor_.window.newHeight);
        }

        scene_->processInput();
    }
}