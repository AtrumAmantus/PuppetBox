#pragma once

#include <cstdint>
#include <functional>
#include <string>

#define PB_EVENT_NETWORK            "pb_network_update"
#define PB_EVENT_NETWORK_LISTENER   "pb_network_listener"
#define PB_EVENT_SCENE_ADD          "pb_engine_add_scene"
#define PB_EVENT_SCENE_SET          "pb_engine_set_scene"

typedef std::function<void(std::shared_ptr<void>, std::uint8_t**, std::uint32_t*)> pb_EventTransformer;

namespace PB
{
    namespace Event
    {
        enum NetworkEventType
        {
            CONNECT,
            DISCONNECT,
            TERMINATE,
            READY_CHECK,
            READY
        };
    }

    struct NetworkEvent
    {
        Event::NetworkEventType type;
        std::string host;
        std::uint16_t port;
        std::uint32_t dataLength = 0;
        std::uint8_t* data;
    };
}