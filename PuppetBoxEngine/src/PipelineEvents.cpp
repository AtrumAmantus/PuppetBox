#include "MessageBroker.h"
#include "PipelineEvents.h"

namespace PB
{
    namespace Event::Pipeline::Topic
    {
        std::uint32_t ADD_ANIMATOR_TOPIC = MessageBroker::instance().registerTopic(PB_EVENT_PIPELINE_ADD_ANIMATOR_TOPIC);
        std::uint32_t ADD_ENTITY_TOPIC = MessageBroker::instance().registerTopic(PB_EVENT_PIPELINE_ADD_ENTITY_TOPIC);
        std::uint32_t ADD_MODEL_TOPIC = MessageBroker::instance().registerTopic(PB_EVENT_PIPELINE_ADD_MODEL_TOPIC);
        std::uint32_t ATTACH_OBJECT_TO_TOPIC = MessageBroker::instance().registerTopic(PB_EVENT_PIPELINE_ATTACH_OBJECT_TO_TOPIC);
        std::uint32_t SET_BONE_MAP_TOPIC = MessageBroker::instance().registerTopic(PB_EVENT_PIPELINE_SET_BONE_MAP_TOPIC);
        std::uint32_t SET_ENTITY_POSITION_TOPIC = MessageBroker::instance().registerTopic(PB_EVENT_PIPELINE_SET_ENTITY_POSITION_TOPIC);
        std::uint32_t GET_ENTITY_TRANSFORM_TOPIC = MessageBroker::instance().registerTopic(PB_EVENT_PIPELINE_GET_ENTITY_TRANSFORM_TOPIC);
    }
}