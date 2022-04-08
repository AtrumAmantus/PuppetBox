#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "AbstractInputReader.h"
#include "Camera.h"
#include "Constants.h"
#include "DataStructures.h"
#include "IRenderComponent.h"
#include "Pipeline.h"
#include "RenderData.h"
#include "RenderWindow.h"
#include "TypeDef.h"

namespace PB
{
    /**
    * \brief AbstractSceneGraph represents a single rendering scene within an application.  This could be a
    * title menu, or the game itself, allowing for switching between different scenes.
    */
    class PUPPET_BOX_API AbstractSceneGraph
    {
    public:
        std::string name;
    public:
        AbstractSceneGraph(const std::string& sceneName);

        /**
        * \brief Creates a new scene with a given name to be later referenced with.
        *
        * \return The unique name this scene should be referenced by later.
        */
        AbstractSceneGraph(
                const std::string& sceneName,
                RenderWindow renderWindow,
                std::shared_ptr<AbstractInputReader> inputReader,
                std::unique_ptr<IRenderComponent> renderComponent);

        /**
         * \brief Invokes the implementing class's setUps() method, checking first if the
         * {\link AbstractSceneGraph} was initialized propertly.
         *
         * \return True if the setup was successful, False otherwise.
         */
        bool setUp();

        /**
         * \breif Tears down the current scene, destroying any allocated resources.
         *
         * \return True if the scene was successfully destroyed, False otherwise.
         */
        bool tearDown();

        /**
        * \brief Invokes the update() method of the sceneHandler, allowing updates for various scene
        * elements for the current frame.
        *
        * \param deltaTime	The amount of time passed since the last update invocation (in seconds).
        */
        void update(const float deltaTime);

        void addComponent(std::unique_ptr<AbstractObjectComponent> component);

        /**
        * \brief Invokes the render() method of the sceneHandler, rendering out any desired SceneObjects
        * for the current frame in the scene.
        */
        void render() const;

        /**
         * \brief Makes a call to process the current input for the active {@link AbstractSceneHandler}.
         */
        void processInput();

        /**
         * \brief Returns a View matrix for the current scene.
         *
         * \return The View matrix for the current scene.
         */
        mat4 getView() const;

        /**
         * \brief Returns a Projection matrix for the current scene.
         *
         * \return The Projection matrix for the current scene.
         */
        mat4 getProjection() const;

        /**
         * \brief Returns a UIProjection matrix for the current scene.  A UIProjection is
         * an orthographic projection, except with the 0,0 coordinates in the lower left
         * (instead of the center like regular orthographic) for easier UI component placement.
         *
         * \return The UIProjection matrix for the current scene.
         */
        mat4 getUIProjection() const;

        AbstractSceneGraph& operator=(AbstractSceneGraph rhs);

    protected:
        /**
         * \brief Defined by the implementing class to handle any initial scene setup logic.
         *
         * \return True if the setup was successful, False otherwise.
         */
        virtual bool setUps();

        /**
         * \brief Implementation specific tear down logic for destroying allocated resources and cleanup.
         *
         * \return True if the scene was destroyed successfully, False otherwise.
         */
        virtual bool tearDowns();

        /**
         * \brief Defined by the implementing class to specify scene specific update logic that is
         * invoked at the beginning of the update loop.
         *
         * \param deltaTime The amount of time in seconds that has passed since the last update.
         */
        virtual void preLoopUpdates(const float deltaTime);

        /**
         * \brief Defined by the implementing class to specify scene specific update logic that is
         * invoked at the end of the update loop, just before the {\link PB::Camera} is updated.
         *
         * \param deltaTime The amount of time in seconds that has passed since the last update.
         */
        virtual void postLoopUpdates(const float deltaTime);

        /**
         * \brief Defined by the implementing class to specify scene specific render logic.
         */
        virtual void renders() const;

        /**
         * \brief Defined by the implementing class to specify scene specific input processing logic.
         */
        virtual void processInputs();

        /**
         * \brief Provides access to the {\link AbstractInputReader} object from the implementing scene class.
         *
         * \return Reference to the {\link AbstractInputReader} object instance.
         */
        std::shared_ptr<AbstractInputReader>& input();

        /**
         * \brief Sets the view mode for the current scene (Orthographic or Perspective).
         *
         * \param mode The view mode to set for the scene.
         */
        void setViewMode(SceneView::Mode mode);

        /**
         * \brief Returns the current {\link PB::SceneView::Mode} of the scene.
         *
         * \return The current {\link PB::SceneView::Mode} of the scene.
         */
        const SceneView::Mode viewMode();

        /**
         * \brief Returns a reference to the current scene {\link PB::Camera}.
         *
         * TODO: Add multiple cameras option
         *
         * \return Reference to the current scene {\link PB::Camera}
         */
        Camera& camera();

        /**
         * \brief Returns a reference to the {\link PB::RenderWindow}.
         *
         * \return The reference to the {\link PB::RenderWindow}.
         */
        const RenderWindow& renderWindow() const;

        /**
         * \brief Creates a new scene object with the given {\link PB::UUID}.
         */
        void createSceneObject(UUID uuid);

        void setSceneObjectPosition(UUID uuid, vec3 position);

        void addModelToSceneObject(const std::string& modelName, UUID uuid, Model model);

        void setSceneObjectSkeleton(UUID uuid, BoneMap boneMap);

    private:
        bool isInitialized_ = false;
        bool isSetup_ = false;
        Camera camera_{};
        RenderWindow renderWindow_{};
        std::shared_ptr<AbstractInputReader> inputReader_{nullptr};
        SceneView::Mode viewMode_ = SceneView::ORTHO;
        SceneView::Mode nextViewMode_ = SceneView::ORTHO;
        Pipeline pipeline_{};
        std::mutex mutex_;
    };
}