#include "puppetbox/AbstractSceneHandler.h"

namespace PB
{
    AbstractSceneHandler::AbstractSceneHandler(AbstractInputProcessor* inputProcessor, SceneView::Mode mode)
            : inputProcessor_(inputProcessor), viewMode_(mode)
    {

    }

    void AbstractSceneHandler::setUp()
    {

    }

    void AbstractSceneHandler::update(const float deltaTime)
    {
        camera_->update(deltaTime);

        // Update implementing application first in case new objects were added.
        updates(deltaTime);

        for (auto& e: sceneObjects_)
        {
            if (!e.second->isAttached() || e.second->getAttached().isUpdated)
            {
                e.second->update(deltaTime);
            }
            else
            {
                processLater_.add(e.second);
            }
        }

        bool keepProcessing = !processLater_.isEmpty();

        while (keepProcessing)
        {
            std::uint32_t queueCount = processLater_.size();

            for (std::uint32_t i = 0; i < queueCount; ++i)
            {
                SceneObject* object = processLater_.pop();

                if (!object->isAttached() || object->getAttached().isUpdated)
                {
                    object->update(deltaTime);
                }
                else
                {
                    processLater_.add(object);
                }
            }

            if (queueCount == processLater_.size())
            {
                throw std::exception("Circular dependency, can't update objects");
            }

            keepProcessing = !processLater_.isEmpty();
        }
    }

    void AbstractSceneHandler::render()
    {
        for (auto& e: sceneObjects_)
        {
            e.second->render();
        }

        renders();
    }

    void AbstractSceneHandler::setCamera(Camera* camera)
    {
        this->camera_ = camera;
    }

    Camera& AbstractSceneHandler::getCamera()
    {
        return *this->camera_;
    }

    SceneView::Mode AbstractSceneHandler::getViewMode() const
    {
        return viewMode_;
    }

    void AbstractSceneHandler::processInput()
    {
        // This can contain some engine specified checks, but right now I have nothing...
        processInputs();
    }

    AbstractInputProcessor* AbstractSceneHandler::input() const
    {
        return inputProcessor_;
    }

    void AbstractSceneHandler::processInputs()
    {

    }

    void AbstractSceneHandler::updates(float deltaTime)
    {

    }

    void AbstractSceneHandler::renders() const
    {

    }

    void AbstractSceneHandler::addSceneObject(SceneObject* sceneObject)
    {
        sceneObjects_.insert(
                std::pair<std::string, SceneObject*>{sceneObject->getId(), sceneObject}
        );
    }

    void AbstractSceneHandler::removeSceneObject(SceneObject* sceneObject)
    {
        sceneObjects_.erase(sceneObject->getId());
    }

    void AbstractSceneHandler::setViewMode(SceneView::Mode mode)
    {
        viewMode_ = mode;
    }
}