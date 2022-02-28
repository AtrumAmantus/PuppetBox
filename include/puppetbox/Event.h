#pragma once

#include <cstdint>
#include <functional>
#include <string>

#define PB_EVENT_NETWORK            "pb_network_update"
#define PB_EVENT_NETWORK_WRITER     "pb_network_listener"
#define PB_EVENT_NETWORK_READER     "pb_network_reader"
#define PB_EVENT_NETWORK_STATUS     "pb_network_status"
#define PB_EVENT_SCENE_ADD          "pb_engine_add_scene"
#define PB_EVENT_SCENE_SET          "pb_engine_set_scene"

typedef std::function<void(std::shared_ptr<void>, std::uint8_t**, std::uint32_t*)> pb_NetworkEventWriter;

typedef std::function<void(std::uint8_t*, std::uint32_t)> pb_NetworkEventReader;

namespace PB
{
    namespace Event
    {
        enum NetworkStatus
        {
            DISCONNECTED,
            CONNECTED,
            READY
        };

        enum NetworkEventType
        {
            CONNECT,
            DISCONNECT,
            TERMINATE,
            READY_CHECK
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

    struct NetworkStatusEvent
    {
        Event::NetworkStatus status;
    };
}