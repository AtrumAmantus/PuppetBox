#include "puppetbox/AbstractSceneGraph.h"
#include "GfxMath.h"
#include "Logger.h"

namespace PB
{
    AbstractSceneGraph::AbstractSceneGraph(const std::string& sceneName) : name(sceneName)
    {

    }

    AbstractSceneGraph::AbstractSceneGraph(
            const std::string& sceneName,
            RenderWindow renderWindow,
            std::shared_ptr<AbstractInputReader> inputReader)
            : name(sceneName), renderWindow_(renderWindow), inputReader_(inputReader), isInitialized_(true)
    {

    }

    bool AbstractSceneGraph::setUp()
    {
        bool success;

        if (isInitialized_)
        {
            success = setUps();
        }
        else
        {
            success = false;
            LOGGER_ERROR("This object has not been properly initialized, this should be handled by the core engine.");
        }

        return success;
    }

    void AbstractSceneGraph::update(const float deltaTime)
    {
        camera_.update(deltaTime);

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

    void AbstractSceneGraph::render() const
    {
        for (auto& e: sceneObjects_)
        {
            e.second->render();
        }

        renders();
    }

    void AbstractSceneGraph::processInput()
    {
        // This can contain some engine specified checks, but right now I have nothing...
        processInputs();
    }

    mat4 AbstractSceneGraph::getView() const
    {
        return camera_.calculateViewMatrix(viewMode_);
    }

    mat4 AbstractSceneGraph::getProjection() const
    {
        return camera_.zoomMatrix() * GfxMath::Projection(
                *renderWindow_.width,
                *renderWindow_.height,
                *renderWindow_.depth,
                viewMode_);
    }

    mat4 AbstractSceneGraph::getUIProjection() const
    {
        return GfxMath::Projection(
                *renderWindow_.width,
                *renderWindow_.height,
                *renderWindow_.depth,
                SceneView::UI
        );
    }

    bool AbstractSceneGraph::setUps()
    {
        return true;
    }

    void AbstractSceneGraph::updates(const float deltaTime)
    {

    }

    void AbstractSceneGraph::renders() const
    {

    }

    void AbstractSceneGraph::processInputs()
    {

    }

    std::shared_ptr<AbstractInputReader>& AbstractSceneGraph::input()
    {
        return inputReader_;
    }

    void AbstractSceneGraph::setViewMode(SceneView::Mode mode)
    {
        viewMode_ = mode;
    }

    Camera& AbstractSceneGraph::getCamera()
    {
        return camera_;
    }

    void AbstractSceneGraph::addSceneObject(SceneObject* sceneObject)
    {
        sceneObjects_.insert(
                std::pair<std::string, SceneObject*>{sceneObject->getId(), sceneObject}
        );
    }
}