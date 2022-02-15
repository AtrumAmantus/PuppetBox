#pragma once

#include <vector>
#include <unordered_map>

#include "AbstractInputReader.h"
#include "Camera.h"
#include "SceneObject.h"
#include "TypeDef.h"

namespace PB
{
    /**
    * \brief Base class to create derived SceneHandlers for guiding events within a scene.
    */
    class PUPPET_BOX_API AbstractSceneHandler
    {
    public:
        AbstractSceneHandler() = default;

        AbstractSceneHandler(AbstractInputReader* inputReader, SceneView::Mode mode);

        /**
        * \brief Runs only once, when the scene first loads
        */
        virtual bool setUp();

        /**
        * \brief Runs once per frame, after input processing, but before rendering.
         *
         * <p>Can be overridden to get complete control of update logic.</p>
        *
        * \param deltaTime The time in milliseconds since the last frame was run.
        */
        //TODO: Think about breaking this out into different update phases
        // OutOfFrameUpdates, InFrameUpdates, PausedUpdates, etc.
        virtual void update(const float deltaTime);

        /**
        * \brief Runs once per frame, after updates
        */
        virtual void render();

        /**
         * \brief Sets the camera to be associated with the current scene.
         *
         * \param camera Pointer to the current camera.
         */
        void setCamera(Camera* camera);

        /**
         * \brief Returns a pointer to the {\link Camera} associated to the current scene.
         *
         * \return Pointer to the {\link Camera} associated to the current scene.
         */
        Camera& getCamera();

        /**
         * \brief Returns the currently {\link SceneView::Mode} for the scene.
         *
         * \return The currently set {\link SceneView::Mode}.
         */
        SceneView::Mode getViewMode() const;

        /**
         * \brief Base engine input processing logic, followed by user defined input processing.
         */
        void processInput();

    protected:
        /**
         * \brief Provides access to input state to the child class.
         *
         * \return Pointer to the current input processor holding input state.
         */
        AbstractInputReader* input() const;

        /**
         * \brief Defined by child class for additional customized input processing.
         */
        virtual void processInputs();

        /**
         * \brief Defined by child class for additional custom frame update logic.
         *
         * \param deltaTime The time (in milliseconds) that has passed since the last update().
         */
        virtual void updates(float deltaTime);

        /**
         * \brief Defined by child class for additional custom rendering logic.
         */
        virtual void renders() const;

        /**
         * \brief Add an object to the scene, allowing engine level handling of object events.
         *
         * \param sceneObject The object to add to the scene.
         */
        void addSceneObject(SceneObject* sceneObject);

        /**
         * \brief Remove a {@link SceneObject} from the scene, preventing engine level handling of the object's events.
         *
         * \param sceneObject The {@link SceneObject} to remove.
         */
        void removeSceneObject(SceneObject* sceneObject);

        /**
         * \brief Sets the view mode of the scene
         *
         * \param mode The desired {\link SceneView::Mode} for the scene.
         */
        void setViewMode(SceneView::Mode mode);

    private:
        AbstractInputReader* inputReader_ = nullptr;
        SceneView::Mode viewMode_ = SceneView::ORTHO;
        std::unordered_map<std::string, SceneObject*> sceneObjects_{};
        Camera* camera_ = nullptr;
        Queue<SceneObject*> processLater_{};
    };
}