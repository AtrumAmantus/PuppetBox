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
        viewMode_ = nextViewMode_;

        // Clean up scene if it was requested
        if (clearSceneInvoked_)
        {
            clearSceneNow();
        }

        // Update implementing application first in case new objects were added or modified.
        preLoopUpdates(deltaTime);

        // Scene object add/remove behind mutex lock to avoid race conditions
        {
            std::unique_lock<std::mutex> mlock(mutex_);

            // Add queued objects
            Result<UUID> movedByUUID = moveToScene_.pop();
            while (movedByUUID.hasResult)
            {
                auto itr = parkedSceneObjects_.find(movedByUUID.result);

                if (itr != parkedSceneObjects_.end())
                {
                    SceneObject* objectToMove = itr->second;
                    parkedSceneObjects_.erase(itr);

                    if (activeSceneObjects_.find(movedByUUID.result) != activeSceneObjects_.end())
                    {
                        LOGGER_ERROR("Can't add, active scene object with the given UUID already exists");
                    }
                    else
                    {
                        activeSceneObjects_.insert(
                                std::pair<UUID, SceneObject*>{movedByUUID.result, objectToMove}
                        );
                    }
                }
                else
                {
                    LOGGER_ERROR("Can't move, no parked scene objects exist with the given UUID");
                }

                movedByUUID = moveToScene_.pop();
            }

            // Remove queued objects
            Result<UUID> removedByUUID = removeFromScene_.pop();
            while (removedByUUID.hasResult)
            {
                auto itr = activeSceneObjects_.find(removedByUUID.result);

                if (itr != activeSceneObjects_.end())
                {
                    SceneObject* objectToRemove = itr->second;
                    activeSceneObjects_.erase(itr);

                    if (parkedSceneObjects_.find(objectToRemove->getId()) != parkedSceneObjects_.end())
                    {
                        LOGGER_ERROR("Can't remove, parked scene object already exists with that UUID");
                    }
                    else
                    {
                        parkedSceneObjects_.insert(
                                std::pair<UUID, SceneObject*>{removedByUUID.result, objectToRemove}
                        );
                    }
                }
                else
                {
                    LOGGER_WARN("Can't remove, no active scene object exists with that UUID");
                }

                removedByUUID = removeFromScene_.pop();
            }

            // Destroy queued objects
            Result<UUID> destroyByUUID = objectsToDestroy_.pop();
            while (removedByUUID.hasResult)
            {
                auto itr = parkedSceneObjects_.find(destroyByUUID.result);

                if (itr != parkedSceneObjects_.end())
                {
                    SceneObject* tmp = itr->second;
                    parkedSceneObjects_.erase(itr);
                    delete tmp;
                }
                else
                {
                    LOGGER_WARN("Can't destroy, no parked scene object exists with that UUID");
                }

                destroyByUUID = removeFromScene_.pop();
            }
        }

        // Update all scene objects
        for (auto& e : activeSceneObjects_)
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
        for (auto& e : activeSceneObjects_)
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
        auto itr = activeSceneObjects_.find(uuid);

        if (itr != activeSceneObjects_.end())
        {
            return itr->second;
        }
        else
        {
            itr = parkedSceneObjects_.find(uuid);

            if (itr != parkedSceneObjects_.end())
            {
                return itr->second;
            }
        }

        return nullptr;
    }

    std::shared_ptr<AbstractInputReader>& AbstractSceneGraph::input()
    {
        return inputReader_;
    }

    void AbstractSceneGraph::setViewMode(SceneView::Mode mode)
    {
        nextViewMode_ = mode;
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
        std::unique_lock<std::mutex> mlock(mutex_);

        if (parkedSceneObjects_.find(sceneObject->getId()) != parkedSceneObjects_.end())
        {
            LOGGER_ERROR("Can't add, parked scene object already exists with that UUID");
        }
        else
        {
            parkedSceneObjects_.insert(
                    std::pair<UUID, SceneObject*>{sceneObject->getId(), sceneObject}
            );
        }
    }

    void AbstractSceneGraph::moveToScene(UUID uuid)
    {
        std::unique_lock<std::mutex> mlock(mutex_);

        moveToScene_.push(uuid);
    }

    void AbstractSceneGraph::removeFromScene(UUID uuid)
    {
        std::unique_lock<std::mutex> mlock(mutex_);

        removeFromScene_.push(uuid);
    }

    void AbstractSceneGraph::destroySceneObject(UUID uuid)
    {
        std::unique_lock<std::mutex> mlock(mutex_);

        objectsToDestroy_.push(uuid);
    }

    void AbstractSceneGraph::clearScene()
    {
        clearSceneInvoked_ = true;
    }

    void AbstractSceneGraph::clearSceneNow()
    {
        std::unordered_map<PB::UUID, SceneObject*>::iterator iter = activeSceneObjects_.begin();

        while (iter != activeSceneObjects_.end())
        {
            SceneObject* object = iter->second;
            iter = activeSceneObjects_.erase(iter);
            delete object;
        }

        clearSceneCompleted_ = true;
    }

    bool AbstractSceneGraph::wasClearSceneInvoked()
    {
        return clearSceneInvoked_;
    }
}