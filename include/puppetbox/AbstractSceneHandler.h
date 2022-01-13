#pragma once

#include <vector>
#include <unordered_map>

#include "AbstractInputProcessor.h"
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
    protected:
        AbstractInputProcessor* input() const
        {
            return inputProcessor_;
        };

        virtual void processInput() const {};

        void addSceneObject(SceneObject* sceneObject)
        {
            sceneObjects_.insert(
                    std::pair<std::string, SceneObject*>{sceneObject->getId(), sceneObject}
            );
        }

        void removeSceneObject(SceneObject* sceneObject)
        {
            sceneObjects_.erase(sceneObject->getId());
        }

    private:
        AbstractInputProcessor* inputProcessor_ = nullptr;
        std::unordered_map<std::string, SceneObject*> sceneObjects_{};
    };
}