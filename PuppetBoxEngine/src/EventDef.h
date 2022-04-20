#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

#include "puppetbox/AbstractSceneGraph.h"
#include "puppetbox/Event.h"
#include "puppetbox/IAnimationCatalogue.h"
#include "puppetbox/UIComponent.h"

#define PB_EVENT_ANIMATION_GET_ANIMATOR_TOPIC  "pb_event_animation_get_animator"
#define PB_EVENT_UI_SET_STRING_ATTRIBUTE       "pb_event_ui_set_string_attribute"
#define PB_EVENT_UI_SET_UINT_ATTRIBUTE         "pb_event_ui_set_uint_attribute"
#define PB_EVENT_UI_SET_FLOAT_ATTRIBUTE        "pb_event_ui_set_float_attribute"
#define PB_EVENT_UI_SET_BOOL_ATTRIBUTE         "pb_event_ui_set_bool_attribute"

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

    struct UIUpdateStringEvent
    {
        UI::Attribute::String attribute;
        std::string value;
    };

    struct UIUpdateUIntEvent
    {
        UI::Attribute::UInteger attribute;
        std::uint32_t value;
    };

    struct UIUpdateFloatEvent
    {
        UI::Attribute::Float attribute;
        float value;
    };

    struct UIUpdateBoolEvent
    {
        UI::Attribute::Boolean attribute;
        bool value;
    };
}