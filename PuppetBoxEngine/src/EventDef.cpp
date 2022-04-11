#include "EventDef.h"
#include "MessageBroker.h"

namespace PB
{
    namespace Event::Topic
    {
        std::uint32_t NETWORK_TOPIC = MessageBroker::instance().registerTopic(PB_EVENT_NETWORK);
        std::uint32_t NETWORK_STATUS_TOPIC = MessageBroker::instance().registerTopic(PB_EVENT_NETWORK_STATUS);
        std::uint32_t NETWORK_EVENT_WRITER_TOPIC = MessageBroker::instance().registerTopic(PB_EVENT_NETWORK_WRITER);
        std::uint32_t NETWORK_EVENT_READER_TOPIC = MessageBroker::instance().registerTopic(PB_EVENT_NETWORK_READER);
        std::uint32_t ENGINE_ADD_SCENE_TOPIC = MessageBroker::instance().registerTopic(PB_EVENT_SCENE_ADD);
        std::uint32_t ENGINE_SET_SCENE_TOPIC = MessageBroker::instance().registerTopic(PB_EVENT_SCENE_SET);
        std::uint32_t ANIMATION_GET_ANIMATOR_TOPIC = MessageBroker::instance().registerTopic(PB_EVENT_ANIMATION_GET_ANIMATOR_TOPIC);
    }
}