#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

#include "puppetbox/AbstractSceneGraph.h"
#include "puppetbox/Event.h"

/**
 * These are "hidden" internal events and topics that are not visible to the implementing application.
 */
namespace PB
{
    namespace Event::Topic
    {
        extern std::uint32_t NETWORK_TOPIC;
        extern std::uint32_t NETWORK_LISTENER_TOPIC;
        extern std::uint32_t ENGINE_ADD_SCENE_TOPIC;
        extern std::uint32_t ENGINE_SET_SCENE_TOPIC;
    }

    // TODO: This could do with a better / more accurate name
    struct NetworkListenerEvent
    {
        std::string topicName;
        pb_EventTransformer transformer;
    };

    struct EngineAddSceneEvent
    {
        std::shared_ptr<AbstractSceneGraph> scene;
    };

    struct EngineSetSceneEvent
    {
        std::string sceneName;
    };
}