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
        SceneGraph(std::string sceneName, IGfxApi* gfxApi, AbstractInputProcessor* inputProcessor)
                : id_(std::move(sceneName)), gfxApi_(gfxApi), inputProcessor_(inputProcessor)
        {
            camera_ = Camera{std::make_unique<RenderWindowImplementation>(gfxApi_)};
        };

        /**
        * \brief Sets a SceneHandler for this scene.
        *
        * \param sceneHandler	The SceneHandler to be used for the scene.
        */
        void setSceneHandler(AbstractSceneHandler* sceneHandler)
        {
            if (sceneHandler_ != nullptr)
            {
                sceneHandler_->setCamera(nullptr);
            }

            *sceneHandler = AbstractSceneHandler{inputProcessor_};
            sceneHandler_ = sceneHandler;
            sceneHandler_->setCamera(&camera_);
            sceneHandler_->setUp();
        };

        /**
        * \brief Sets the projection mode for the scene.
        *
        * \param mode	The projection mode for the scene.
        */
        void setCameraMode(SceneView::Mode mode)
        {
            viewMode_ = mode;
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

            mat4 view = camera_.calculateViewMatrix(viewMode_);
            mat4 projection = GfxMath::Projection(
                    gfxApi_->getRenderWidth(),
                    gfxApi_->getRenderHeight(),
                    gfxApi_->getRenderDistance(),
                    viewMode_
            );
            mat4 orthoProjection = GfxMath::Projection(
                    gfxApi_->getRenderWidth(),
                    gfxApi_->getRenderHeight(),
                    gfxApi_->getRenderDistance(),
                    SceneView::ORTHO
            );

            gfxApi_->setTransformUBOData(view, projection, orthoProjection);
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
        Camera camera_{nullptr};
        SceneView::Mode viewMode_ = SceneView::Mode::ORTHO;
        IGfxApi* gfxApi_;
        AbstractInputProcessor* inputProcessor_;
        AbstractSceneHandler* sceneHandler_ = nullptr;
    };
}