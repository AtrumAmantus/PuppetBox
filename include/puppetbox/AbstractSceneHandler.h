#pragma once

#include <vector>
#include <unordered_map>

#include "AbstractInputProcessor.h"
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

        explicit AbstractSceneHandler(AbstractInputProcessor* inputProcessor) : inputProcessor_(inputProcessor) {};

        /**
        * \brief Runs only once, when the scene first loads
        */
        virtual void setUp() {};

        /**
        * \brief Runs once per frame, after input processing, but before rendering.
        *
        * \param deltaTime The time in milliseconds since the last frame was run.
        */
        virtual void update(const float deltaTime)
        {
            processInput();
            camera_->update(deltaTime);

            for (auto& e: sceneObjects_)
            {
                e.second->update(deltaTime);
            }
        };

        /**
        * \brief Runs once per frame, after updates
        */
        virtual void render()
        {
            for (auto& e: sceneObjects_)
            {
                e.second->render();
            }
        };

        /**
         * \brief Sets the camera to be associated with the current scene.
         *
         * @param camera Pointer to the current camera.
         */
        void setCamera(Camera* camera)
        {
            this->camera_ = camera;
        }

        /**
         * \brief Returns a pointer to the {@link Camera} associated to the current scene.
         *
         * @return Pointer to the {@link Camera} associated to the current scene.
         */
        Camera* getCamera()
        {
            return this->camera_;
        }

    protected:
        /**
         * \brief Provides access to input state to the child class.
         *
         * @return Pointer to the current input processor holding input state.
         */
        AbstractInputProcessor* input() const
        {
            return inputProcessor_;
        };

        /**
         * \brief Defined by child class to all customization of input processing
         */
        virtual void processInputs() {};

        /**
         * \brief Add an object to the scene, allowing engine level handling of object events.
         *
         * @param sceneObject The object to add to the scene.
         */
        void addSceneObject(SceneObject* sceneObject)
        {
            sceneObjects_.insert(
                    std::pair<std::string, SceneObject*>{sceneObject->getId(), sceneObject}
            );
        }

        /**
         * \brief Remove a {@link SceneObject} from the scene, preventing engine level handling of the object's events.
         *
         * @param sceneObject The {@link SceneObject} to remove.
         */
        void removeSceneObject(SceneObject* sceneObject)
        {
            sceneObjects_.erase(sceneObject->getId());
        }

    private:
        AbstractInputProcessor* inputProcessor_ = nullptr;
        std::unordered_map<std::string, SceneObject*> sceneObjects_{};
        Camera* camera_ = nullptr;
    private:
        /**
         * \brief Base engine input processing logic, followed by user defined input processing.
         */
        void processInput()
        {
            processInputs();
        }
    };
}