#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "puppetbox/AbstractSceneHandler.h"
#include "puppetbox/Camera.h"
#include "puppetbox/Constants.h"
#include "puppetbox/DataStructures.h"
#include "puppetbox/IRenderWindow.h"
#include "GfxMath.h"
#include "Logger.h"
#include "RenderWindowImplementation.h"
#include "TypeDef.h"

namespace PB
{
    /**
    * \brief SceneGraph represents a single rendering scene within an application.  This could be a
    * title menu, or the game itself, allowing for switching between different scenes.
    */
    class SceneGraph
    {
    public:
        /**
        * \brief Creates a new scene with a given name to be later referenced with.
        *
        * \return The unique name this scene should be referenced by later.
        */
        SceneGraph(std::string sceneName, IGfxApi* gfxApi, AbstractInputReader* inputReader)
                : id_(std::move(sceneName)), gfxApi_(gfxApi), inputReader_(inputReader)
        {

        };

        /**
        * \brief Sets a SceneHandler for this scene.
        *
        * \param sceneHandler	The SceneHandler to be used for the scene.
        */
        bool setSceneHandler(AbstractSceneHandler* sceneHandler)
        {
            if (sceneHandler_ != nullptr)
            {
                sceneHandler_->setCamera(nullptr);
            }

            *sceneHandler = AbstractSceneHandler{inputReader_, SceneView::ORTHO};
            sceneHandler_ = sceneHandler;
            sceneHandler_->setCamera(&camera_);
            return sceneHandler_->setUp();
        };

        /**
        * \brief Invokes the update() method of the sceneHandler, allowing updates for various scene
        * elements for the current frame.
        *
        * \param deltaTime	The amount of time passed since the last update invocation (in Milliseconds).
        */
        void update(const float deltaTime)
        {
            sceneHandler_->update(deltaTime);

            SceneView::Mode viewMode = sceneHandler_->getViewMode();

            mat4 view = camera_.calculateViewMatrix(viewMode);

            mat4 projection = camera_.zoomMatrix() * GfxMath::Projection(
                    gfxApi_->getRenderWidth(),
                    gfxApi_->getRenderHeight(),
                    gfxApi_->getRenderDistance(),
                    viewMode
            );

            mat4 uiProjection = GfxMath::Projection(
                    gfxApi_->getRenderWidth(),
                    gfxApi_->getRenderHeight(),
                    gfxApi_->getRenderDistance(),
                    SceneView::UI
            );

            gfxApi_->setTransformUBOData(view, projection, uiProjection);
        };

        /**
         * \brief Makes a call to process the current input for the active {@link AbstractSceneHandler}.
         */
        void processInput()
        {
            sceneHandler_->processInput();
        };

        /**
        * \brief Invokes the render() method of the sceneHandler, rendering out any desired SceneObjects
        * for the current frame in the scene.
        */
        void render() const
        {
            sceneHandler_->render();
        };

    private:
        std::string id_;
        Camera camera_{};
        IGfxApi* gfxApi_;
        AbstractInputReader* inputReader_;
        AbstractSceneHandler* sceneHandler_ = nullptr;
    };
}