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
        bool success = true;

        if (!isSetup_)
        {
            if (isInitialized_)
            {
                success = setUps();
            }
            else
            {
                success = false;
                LOGGER_ERROR(
                        "This object has not been properly initialized, this should be handled by the core engine.");
            }

            isSetup_ = true;
        }

        return success;
    }

    bool AbstractSceneGraph::tearDown()
    {
        bool success = true;

        if (isSetup_)
        {
            if (isInitialized_)
            {
                success = tearDowns();
            }
            else
            {
                success = false;
                LOGGER_ERROR(
                        "This object has not been properly initialized, this should be handled by the core engine.");
            }

            isSetup_ = false;
        }

        return success;
    }

    void AbstractSceneGraph::update(const float deltaTime)
    {
        // Clean up scene if it was requested
        if (clearSceneInvoked_)
        {
            clearSceneNow();
        }

        // Update implementing application first in case new objects were added or modified.
        preLoopUpdates(deltaTime);

        // Add queued objects
        Result<SceneObject*> addSceneObject = objectsToAdd_.pop();
        while (addSceneObject.hasResult)
        {
            sceneObjects_.insert(
                    std::pair<UUID, SceneObject*>{addSceneObject.result->getId(), addSceneObject.result}
            );

            addSceneObject = objectsToAdd_.pop();
        }

        // Remove queued objects
        Result<UUID> removeSceneObject = objectsToRemove_.pop();
        while (removeSceneObject.hasResult)
        {
            auto itr = sceneObjects_.find(removeSceneObject.result);

            if (itr != sceneObjects_.end())
            {
                SceneObject* object = itr->second;
                sceneObjects_.erase(itr);
                delete object;
            }

            removeSceneObject = objectsToRemove_.pop();
        }

        // Update all scene objects
        for (auto& e: sceneObjects_)
        {
            if (!e.second->isAttached() || e.second->getAttached().isUpdated)
            {
                e.second->update(deltaTime);
            }
            else
            {
                processLater_.push(e.second);
            }
        }

        // Update any scene objects that couldn't be updated due to dependencies
        while (!processLater_.empty())
        {
            std::uint32_t queueCount = processLater_.size();

            for (std::uint32_t i = 0; i < queueCount; ++i)
            {
                SceneObject* object = processLater_.front();
                processLater_.pop();

                if (!object->isAttached() || object->getAttached().isUpdated)
                {
                    object->update(deltaTime);
                }
                else
                {
                    processLater_.push(object);
                }
            }

            // Check if even one thing processed successfully
            if (queueCount == processLater_.size())
            {
                throw std::exception("Circular dependency, can't update objects");
            }
        }

        // Update implementing application's post loop updates
        postLoopUpdates(deltaTime);

        camera_.update(deltaTime);

        if (clearSceneCompleted_)
        {
            clearSceneInvoked_ = false;
            clearSceneCompleted_ = false;
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
        return camera_.calculateViewMatrix(renderWindow_, viewMode_);
    }

    mat4 AbstractSceneGraph::getProjection() const
    {
        return GfxMath::Projection(
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
                SceneView::ORTHO
        );
    }

    bool AbstractSceneGraph::setUps()
    {
        return true;
    }

    bool AbstractSceneGraph::tearDowns()
    {
        return true;
    }

    void AbstractSceneGraph::preLoopUpdates(const float deltaTime)
    {
        // No default executions
    }

    void AbstractSceneGraph::postLoopUpdates(const float deltaTime)
    {
        // No default executions
    }

    void AbstractSceneGraph::renders() const
    {
        // No default executions
    }

    void AbstractSceneGraph::processInputs()
    {
        // No default executions
    }

    SceneObject* AbstractSceneGraph::getSceneObject(UUID uuid)
    {
        auto itr = sceneObjects_.find(uuid);

        if (itr != sceneObjects_.end())
        {
            return itr->second;
        }

        return nullptr;
    }

    std::shared_ptr<AbstractInputReader>& AbstractSceneGraph::input()
    {
        return inputReader_;
    }

    void AbstractSceneGraph::setViewMode(SceneView::Mode mode)
    {
        viewMode_ = mode;
    }

    const SceneView::Mode AbstractSceneGraph::viewMode()
    {
        return viewMode_;
    }

    Camera& AbstractSceneGraph::camera()
    {
        return camera_;
    }

    const RenderWindow& AbstractSceneGraph::renderWindow() const
    {
        return renderWindow_;
    }

    void AbstractSceneGraph::addSceneObject(SceneObject* sceneObject)
    {
        objectsToAdd_.push(sceneObject);
    }

    void AbstractSceneGraph::removeSceneObject(UUID uuid)
    {
        objectsToRemove_.push(uuid);
    }

    void AbstractSceneGraph::clearScene()
    {
        clearSceneInvoked_ = true;
    }

    void AbstractSceneGraph::clearSceneNow()
    {
        std::unordered_map<PB::UUID, SceneObject*>::iterator iter = sceneObjects_.begin();

        while (iter != sceneObjects_.end())
        {
            SceneObject* object = iter->second;
            iter = sceneObjects_.erase(iter);
            delete object;
        }

        clearSceneCompleted_ = true;
    }

    bool AbstractSceneGraph::wasClearSceneInvoked()
    {
        return clearSceneInvoked_;
    }
}