#include "puppetbox/AbstractSceneGraph.h"

#include "GfxMath.h"
#include "Logger.h"
#include "MessageBroker.h"
#include "PipelineEvents.h"

namespace PB
{
    AbstractSceneGraph::AbstractSceneGraph(const std::string& sceneName) : name(sceneName)
    {

    }

    AbstractSceneGraph::AbstractSceneGraph(
            const std::string& sceneName,
            RenderWindow renderWindow,
            std::shared_ptr<AbstractInputReader> inputReader,
            std::unique_ptr<IRenderComponent> renderComponent)
            : name(sceneName),
            renderWindow_(renderWindow),
            inputReader_(inputReader),
            isInitialized_(true)
    {
        pipeline_.setRenderComponent(std::move(renderComponent));
    }

    bool AbstractSceneGraph::setUp()
    {
        bool success = true;

        if (!isSetup_)
        {
            if (isInitialized_)
            {
                pipeline_.init();
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
                pipeline_.tearDown();

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
        // Update implementing application first in case new objects were added or modified.
        preLoopUpdates(deltaTime);

        pipeline_.update(deltaTime);

        // Update implementing application's post loop updates
        postLoopUpdates(deltaTime);

        camera_.update(deltaTime);
    }

    void AbstractSceneGraph::addComponent(std::unique_ptr<AbstractObjectComponent> component)
    {
        pipeline_.addComponent(std::move(component));
    }

    void AbstractSceneGraph::render() const
    {
        pipeline_.render();
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

    void AbstractSceneGraph::createSceneObject(UUID uuid)
    {
        auto event = std::make_shared<PipelineAddEntityEvent>();
        event->uuid = uuid;

        MessageBroker::instance().publish(PB_EVENT_PIPELINE_ADD_ENTITY_TOPIC, event);
    }

    void AbstractSceneGraph::setSceneObjectPosition(UUID uuid, vec3 position)
    {
        auto event = std::make_shared<PipelineSetObjectPositionEvent>();
        event->uuid = uuid;
        event->position = position;

        MessageBroker::instance().publish(PB_EVENT_PIPELINE_SET_ENTITY_POSITION_TOPIC, event);
    }

    void AbstractSceneGraph::addModelToSceneObject(const std::string& modelName, UUID uuid, Model model)
    {
        auto event = std::make_shared<PipelineAddModelEvent>();
        event->uuid = uuid;
        event->model = model;

        MessageBroker::instance().publish(PB_EVENT_PIPELINE_ADD_MODEL_TOPIC, event);
    }

    void AbstractSceneGraph::setSceneObjectSkeleton(UUID uuid, BoneMap boneMap)
    {
        auto event = std::make_shared<PipelineSetBoneMapEvent>();
        event->uuid = uuid;
        event->boneMap = std::move(boneMap);

        MessageBroker::instance().publish(PB_EVENT_PIPELINE_SET_BONE_MAP_TOPIC, event);
    }

    void AbstractSceneGraph::attachToSceneObject(UUID parasite, UUID host, std::uint32_t attachPoint)
    {
        auto referenceEvent = std::make_shared<PipelineGetAbsoluteBoneTransformMatrixReferenceEvent>();
        referenceEvent->uuid = host;
        referenceEvent->boneId = attachPoint;
        referenceEvent->callback = [parasite, host](std::shared_ptr<IValueReference> reference) {
            auto event = std::make_shared<PipelineAttachObjectTo>();
            event->parasiteUUID = parasite;
            event->hostUUID = host;
            event->reference = std::move(reference);

            MessageBroker::instance().publish(PB_EVENT_PIPELINE_ATTACH_OBJECT_TO_TOPIC, event);
        };

        MessageBroker::instance().publish(PB_EVENT_PIPELINE_GET_ABS_BONE_TRANSFORM_TOPIC, referenceEvent);
    }

    AbstractSceneGraph& AbstractSceneGraph::operator=(AbstractSceneGraph rhs)
    {
		this->isInitialized_ = rhs.isInitialized_;
		this->isSetup_ = rhs.isSetup_;
		this->camera_ = rhs.camera_;
		this->renderWindow_ = rhs.renderWindow_;
		this->inputReader_ = std::move(rhs.inputReader_);
		this->viewMode_ = rhs.viewMode_;
		this->nextViewMode_ = rhs.nextViewMode_;
		this->pipeline_ = std::move(rhs.pipeline_);
        return *this;
    }
}