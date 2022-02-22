#pragma once

#include <cstdint>
#include <string>

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