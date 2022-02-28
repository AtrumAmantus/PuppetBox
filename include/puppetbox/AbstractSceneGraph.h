#pragma once

#include <string>
#include <unordered_map>

#include "AbstractInputReader.h"
#include "Camera.h"
#include "Constants.h"
#include "DataStructures.h"
#include "RenderWindow.h"
#include "SceneObject.h"
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
                std::shared_ptr<AbstractInputReader> inputReader);

        /**
         * \brief Invokes the implementing class's setUps() method, checking first if the
         * {\link AbstractSceneGraph} was initialized propertly.
         *
         * \return True if the setup was successful, False otherwise.
         */
        bool setUp();

        /**
        * \brief Invokes the update() method of the sceneHandler, allowing updates for various scene
        * elements for the current frame.
        *
        * \param deltaTime	The amount of time passed since the last update invocation (in seconds).
        */
        void update(const float deltaTime);

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

    protected:
        /**
         * \brief Defined by the implementing class to handle any initial scene setup logic.
         *
         * \return True if the setup was successful, False otherwise.
         */
        virtual bool setUps();

        /**
         * \brief Defined by the implementing class to specify scene specific update logic.
         *
         * \param deltaTime The amount of time in seconds that has passed since the last update.
         */
        virtual void updates(const float deltaTime);

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
         * \brief Returns a reference to the current scene {\link PB::Camera}.
         *
         * TODO: Add multiple cameras option
         *
         * \return Reference to the current scene {\link PB::Camera}
         */
        Camera& getCamera();

        /**
         * \brief Adds a scene object to the base {\link PB::AbstractSceneGraph} to
         * allow the base logic to be invoked on it.
         *
         * \param sceneObject The {\link PB::SceneObject} to add to the base
         * {\link PB::AbstractSceneGraph}.
         */
        void addSceneObject(SceneObject* sceneObject);

    private:
        bool isInitialized_ = false;
        Camera camera_{};
        RenderWindow renderWindow_{};
        std::shared_ptr<AbstractInputReader> inputReader_{nullptr};
        SceneView::Mode viewMode_ = SceneView::ORTHO;
        std::unordered_map<std::string, SceneObject*> sceneObjects_{};
        Queue<SceneObject*> processLater_{};
    };
}