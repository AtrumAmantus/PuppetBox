#include "Engine.h"

namespace PB
{
    void Engine::run()
    {
        loop();
    }

    void Engine::shutdown()
    {
        hardwareInitializer_.destroy();
    }

    void Engine::setScene(SceneGraph* scene)
    {
        scene_ = scene;
    }

    void Engine::loop()
    {
        hardwareInitializer_.initializeGameTime();
        while (!inputProcessor_.window.windowClose)
        {
            float deltaTime = hardwareInitializer_.updateElapsedTime();

            gfxApi_.preLoopCommands();

            processInput();

            scene_->update(deltaTime);
            scene_->render();

            hardwareInitializer_.postLoopCommands();
        }
    }

    void Engine::processInput()
    {
        inputProcessor_.loadCurrentState();

        if (inputProcessor_.window.newWidth != 0 || inputProcessor_.window.newHeight != 0)
        {
            gfxApi_.setRenderDimensions(inputProcessor_.window.newWidth, inputProcessor_.window.newHeight);
        }
    }
}