#include "puppetbox/AbstractSceneGraph.h"

#include "EventDef.h"
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
            std::unique_ptr<IPipeline> pipeline,
            std::shared_ptr<AbstractInputReader> inputReader)
            : name(sceneName),
            renderWindow_(renderWindow),
            pipeline_(std::move(pipeline)),
            inputReader_(inputReader),
            isInitialized_(true)
    {

    }

    bool AbstractSceneGraph::setUp()
    {
        bool success = true;

        if (!isSetup_)
        {
            if (isInitialized_)
            {
                pipeline_->init();
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
                pipeline_->tearDown();

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

        pipeline_->update(deltaTime);

        // Update implementing application's post loop updates
        postLoopUpdates(deltaTime);

        camera_.update(deltaTime);
    }

    void AbstractSceneGraph::addComponent(std::unique_ptr<AbstractObjectComponent> component)
    {
        pipeline_->addComponent(std::move(component), {});
    }

    void AbstractSceneGraph::render() const
    {
        pipeline_->render();
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

    void AbstractSceneGraph::lockPipeline()
    {
        pipeline_->lockPipeline();
    }

    void AbstractSceneGraph::unlockPipeline()
    {
        pipeline_->unlockPipeline();
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

        MessageBroker::instance().publish(Event::Pipeline::Topic::ADD_ENTITY_TOPIC, event);
    }

    void AbstractSceneGraph::setSceneObjectPosition(UUID uuid, vec3 position)
    {
        auto event = std::make_shared<PipelineSetObjectPositionEvent>();
        event->uuid = uuid;
        event->position = position;

        MessageBroker::instance().publish(Event::Pipeline::Topic::SET_ENTITY_POSITION_TOPIC, event);
    }

    void AbstractSceneGraph::addModelToSceneObject(const std::string& modelName, UUID uuid, Model model)
    {
        auto event = std::make_shared<PipelineAddModelEvent>();
        event->uuid = uuid;
        event->model = model;

        MessageBroker::instance().publish(Event::Pipeline::Topic::ADD_MODEL_TOPIC, event);
    }

    void AbstractSceneGraph::setSceneObjectSkeleton(UUID uuid, BoneMap boneMap)
    {
        auto event = std::make_shared<PipelineSetBoneMapEvent>();
        event->uuid = uuid;
        event->boneMap = std::move(boneMap);

        MessageBroker::instance().publish(Event::Pipeline::Topic::SET_BONE_MAP_TOPIC, event);
    }

    void AbstractSceneGraph::attachToSceneObject(UUID parasite, UUID host, std::uint32_t attachPointId)
    {
        auto event = std::make_shared<PipelineAttachObjectTo>();
        event->parasiteUUID = parasite;
        event->hostUUID = host;
        event->attachPointId = attachPointId;

        MessageBroker::instance().publish(Event::Pipeline::Topic::ATTACH_OBJECT_TO_TOPIC, event);
    }

    void AbstractSceneGraph::animateSceneObject(UUID uuid, const std::string& animationName)
    {
        auto getAnimatorEvent = std::make_shared<AnimationGetAnimatorEvent>();
        getAnimatorEvent->animationName = animationName;
        getAnimatorEvent->callback = [uuid](std::unique_ptr<IAnimator> animator) {
            auto addAnimatorEvent = std::make_shared<PipelineAddAnimatorEvent>();
            addAnimatorEvent->uuid = uuid;
            addAnimatorEvent->animator = std::move(animator);

            MessageBroker::instance().publish(Event::Pipeline::Topic::ADD_ANIMATOR_TOPIC, addAnimatorEvent);
        };

        MessageBroker::instance().publish(Event::Topic::ANIMATION_GET_ANIMATOR_TOPIC, getAnimatorEvent);
    }

    bool AbstractSceneGraph::createUIComponent(UI::Type componentType, UUID uuid)
    {
        switch (componentType)
        {
            case UI::TEXT_AREA:
                break;
            case UI::GROUP:
                break;
            default:
                break;
        }
        //TODO: Put ui components in a separate thread?  Separate pipeline?  I like handling them as objects, but
        // that's not how the scene objects are handled.

        return true;
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