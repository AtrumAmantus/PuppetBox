#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

#include "puppetbox/AbstractSceneGraph.h"
#include "puppetbox/IAnimationCatalogue.h"
#include "puppetbox/Event.h"

#define PB_EVENT_ANIMATION_GET_ANIMATOR_TOPIC  "pb_event_animation_get_animator"

typedef std::function<void(std::unique_ptr<PB::IAnimator>)> pb_AnimatorConsumer;

/**
 * These are "hidden" internal events and topics that are not visible to the implementing application.
 */
namespace PB
{
    namespace Event::Topic
    {
        extern std::uint32_t NETWORK_TOPIC;
        extern std::uint32_t NETWORK_EVENT_WRITER_TOPIC;
        extern std::uint32_t NETWORK_EVENT_READER_TOPIC;
        extern std::uint32_t NETWORK_STATUS_TOPIC;
        extern std::uint32_t ENGINE_ADD_SCENE_TOPIC;
        extern std::uint32_t ENGINE_SET_SCENE_TOPIC;
        extern std::uint32_t ANIMATION_GET_ANIMATOR_TOPIC;
    }

    struct AnimationGetAnimatorEvent
    {
        std::string animationName;
        pb_AnimatorConsumer callback;
    };

    struct NetworkEventWriterEvent
    {
        std::string topicName;
        pb_NetworkEventWriter writer;
    };

    struct NetworkEventReaderEvent
    {
        pb_NetworkEventReader reader;
    };

    struct EngineAddSceneEvent
    {
        std::shared_ptr<AbstractSceneGraph> scene;
    };

    struct EngineSetSceneEvent
    {
        std::string sceneName;
        bool resetLast = false;
    };
}