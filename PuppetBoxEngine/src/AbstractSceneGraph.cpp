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
            while (!moveToScene_.empty())
            {
                UUID moveUUID = moveToScene_.front();
                moveToScene_.pop();

                recursiveSceneObjectMove(moveUUID);
            }

            // Remove queued objects
            while (!removeFromScene_.empty())
            {
                UUID removeUUID = removeFromScene_.front();
                removeFromScene_.pop();

                recursiveSceneObjectRemove(removeUUID);
            }

            // Run attachments before deletions in case something was added/removed so there are no
            // invalid references
            while(!attachObjectsTo_.empty())
            {
                Attachment attach = attachObjectsTo_.front();
                attachObjectsTo_.pop();

                SceneObject* attachObject = getSceneObject(attach.attach);
                SceneObject* hostObject = getSceneObject(attach.attachedTo);

                if (attachObject == nullptr)
                {
                    LOGGER_WARN("Can not attach, attachment object does not exist.");
                }
                else if (hostObject == nullptr)
                {
                    LOGGER_WARN("Can not attach, host object does not exist.");
                }
                else
                {
                    auto attachmentItr = objectAttachedTo_.find(attach.attach);

                    if (attachmentItr != objectAttachedTo_.end())
                    {
                        // This is already attached to something else, remove it first
                        auto hostItr = objectAttachedWith_.find(attachmentItr->second.attachedTo);

                        if (hostItr != objectAttachedWith_.end())
                        {
                            hostItr->second.erase(attach.attach);
                        }
                    }

                    objectAttachedTo_[attach.attach] = Attachment{
                            attach.attach,
                            attach.attachedTo,
                            attach.attachPoint};

                    objectAttachedWith_.insert(
                            std::pair<PB::UUID, std::unordered_map<PB::UUID, std::uint32_t>>{
                                    attach.attachedTo,
                                    {}}
                    );

                    objectAttachedWith_[attach.attachedTo][attach.attach] = attach.attachPoint;

                    attachObject->attachTo(hostObject, attach.attachPoint);
                }
            }

            // Destroy queued objects
            while (!objectsToDestroy_.empty())
            {
                UUID destroyUUID = objectsToDestroy_.front();
                objectsToDestroy_.pop();

                // Find if this is attached to anything
                auto attachedToItr = objectAttachedTo_.find(destroyUUID);

                if (attachedToItr != objectAttachedTo_.end())
                {
                    // If it is, erase it from the host's attachments
                    auto hostItr = objectAttachedWith_.find(attachedToItr->second.attachedTo);

                    if (hostItr != objectAttachedWith_.end())
                    {
                        hostItr->second.erase(destroyUUID);
                    }
                }

                recursiveSceneObjectDestroy(destroyUUID);
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
                throw std::runtime_error("Circular dependency, can't update objects");
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

    void AbstractSceneGraph::attachToObject(PB::UUID attachment, PB::UUID host, std::uint32_t locationId)
    {
        std::unique_lock<std::mutex> mlock(mutex_);

        attachObjectsTo_.push(Attachment{attachment, host, locationId});
    }

    void AbstractSceneGraph::clearScene()
    {
        clearSceneInvoked_ = true;
    }

    void AbstractSceneGraph::clearSceneNow()
    {
        // Delete all active scene objects
        auto itr = activeSceneObjects_.begin();

        while (itr != activeSceneObjects_.end())
        {
            SceneObject* tmp = itr->second;
            itr = activeSceneObjects_.erase(itr);
            delete tmp;
        }

        // Delete all parked scene objects
        itr = parkedSceneObjects_.begin();

        while (itr != parkedSceneObjects_.end())
        {
            SceneObject* tmp = itr->second;
            itr = parkedSceneObjects_.erase(itr);
            delete tmp;
        }

        clearSceneCompleted_ = true;
    }

    bool AbstractSceneGraph::wasClearSceneInvoked()
    {
        return clearSceneInvoked_;
    }

    void AbstractSceneGraph::recursiveSceneObjectMove(UUID objectToMove)
    {
        auto itr = parkedSceneObjects_.find(objectToMove);

        if (itr != parkedSceneObjects_.end())
        {
            SceneObject* tmp = itr->second;
            parkedSceneObjects_.erase(itr);

            if (activeSceneObjects_.find(objectToMove) != activeSceneObjects_.end())
            {
                LOGGER_ERROR("Can't add, active scene object with the given UUID already exists");
            }
            else
            {
                activeSceneObjects_.insert(
                        std::pair<UUID, SceneObject*>{objectToMove, tmp}
                );

                auto attachmentsItr = objectAttachedWith_.find(objectToMove);

                if (attachmentsItr != objectAttachedWith_.end())
                {
                    for (auto& attachment : attachmentsItr->second)
                    {
                        recursiveSceneObjectMove(attachment.first);
                    }
                }
            }
        }
        else
        {
            LOGGER_ERROR("Can't move, no parked scene objects exist with the given UUID");
        }
    }

    void AbstractSceneGraph::recursiveSceneObjectRemove(UUID objectToRemove)
    {
        auto itr = activeSceneObjects_.find(objectToRemove);

        if (itr != activeSceneObjects_.end())
        {
            SceneObject* tmp = itr->second;
            activeSceneObjects_.erase(itr);

            if (parkedSceneObjects_.find(tmp->getId()) != parkedSceneObjects_.end())
            {
                LOGGER_ERROR("Can't remove, parked scene object already exists with that UUID");
            }
            else
            {
                parkedSceneObjects_.insert(
                        std::pair<UUID, SceneObject*>{objectToRemove, tmp}
                );

                auto attachmentsItr = objectAttachedWith_.find(objectToRemove);

                if (attachmentsItr != objectAttachedWith_.end())
                {
                    for (auto& attachment : attachmentsItr->second)
                    {
                        recursiveSceneObjectRemove(attachment.first);
                    }
                }
            }
        }
        else
        {
            LOGGER_WARN("Can't remove, no active scene object exists with that UUID");
        }
    }

    //TODO: Relatively safe assumptions could be made to speed up processing here.
    void AbstractSceneGraph::recursiveSceneObjectDestroy(UUID objectToDestroy)
    {
        auto itr = parkedSceneObjects_.find(objectToDestroy);

        if (itr != parkedSceneObjects_.end())
        {
            // Removed any attached objects first
            auto hostItr = objectAttachedWith_.find(objectToDestroy);

            if (hostItr != objectAttachedWith_.end())
            {
                // Delete each object attached to this one.
                for (auto& attachment : hostItr->second)
                {
                    // Destroy the actual object
                    recursiveSceneObjectDestroy(attachment.first);
                }
            }

            // Remove reference of this object attached to another
            objectAttachedTo_.erase(objectToDestroy);

            // Remove this object's attachments list
            objectAttachedWith_.erase(objectToDestroy);

            SceneObject* tmp = itr->second;
            parkedSceneObjects_.erase(itr);
            delete tmp;
        }
        else
        {
            LOGGER_WARN("Can't destroy (" + std::to_string(objectToDestroy) + "), no parked scene object exists with that UUID");
        }
    }
}