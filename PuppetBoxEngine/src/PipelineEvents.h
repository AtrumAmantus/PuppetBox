#pragma once

#include "puppetbox/DataStructures.h"
#include "puppetbox/IAnimationCatalogue.h"
#include "puppetbox/IValueReference.h"
#include "puppetbox/RenderData.h"

#define PB_EVENT_PIPELINE_ADD_ANIMATOR_TOPIC            "pb_event_pipeline_add_animator"
#define PB_EVENT_PIPELINE_ADD_ENTITY_TOPIC              "pb_event_pipeline_add_entity"
#define PB_EVENT_PIPELINE_ADD_MODEL_TOPIC               "pb_event_pipeline_add_model"
#define PB_EVENT_PIPELINE_ATTACH_OBJECT_TO_TOPIC        "pb_event_pipeline_attach_object_to"
#define PB_EVENT_PIPELINE_DELETE_INSTANCE_SET_TOPIC     "pb_event_pipeline_delete_instance_set"
#define PB_EVENT_PIPELINE_NEW_INSTANCE_SET_TOPIC        "pb_event_pipeline_new_instance_set"
#define PB_EVENT_PIPELINE_SET_BONE_MAP_TOPIC            "pb_event_pipeline_set_bone_map"
#define PB_EVENT_PIPELINE_SET_ENTITY_POSITION_TOPIC     "pb_event_pipeline_set_entity_position"
#define PB_EVENT_PIPELINE_SET_INSTANCE_MODEL_TOPIC      "pb_event_pipeline_set_instance_model"
#define PB_EVENT_PIPELINE_SET_INSTANCE_SET_TOPIC        "pb_event_pipeline_set_instance_set"
#define PB_EVENT_PIPELINE_GET_ENTITY_TRANSFORM_TOPIC    "pb_event_pipeline_get_model_transform"

namespace PB
{
    namespace Event::Pipeline::Topic
    {
        extern std::uint32_t ADD_ANIMATOR_TOPIC;
        extern std::uint32_t ADD_ENTITY_TOPIC;
        extern std::uint32_t ADD_MODEL_TOPIC;
        extern std::uint32_t DELETE_INSTANCE_SET_TOPIC;
        extern std::uint32_t ATTACH_OBJECT_TO_TOPIC;
        extern std::uint32_t INSTANCE_SET_NEW_TOPIC;
        extern std::uint32_t INSTANCE_SET_INSTANCE_SET_TOPIC;
        extern std::uint32_t INSTANCE_SET_MODEL_TOPIC;
        extern std::uint32_t SET_BONE_MAP_TOPIC;
        extern std::uint32_t SET_ENTITY_POSITION_TOPIC;
        extern std::uint32_t GET_ENTITY_TRANSFORM_TOPIC;
    }

    struct PipelineAddAnimatorEvent
    {
        UUID uuid;
        std::unique_ptr<IAnimator> animator;
    };

    struct PipelineUUIDReference
    {
        UUID uuid;
    };

    struct PipelineInstance
    {
        mat4 transform;
        mat4 data;
    };

    struct PipelineSetInstanceSetEvent
    {
        UUID uuid;
        std::uint32_t instanceCount = 0;
        PipelineInstance instance[];
    };

    struct PipelineNewInstanceSetEvent
    {
        UUID uuid;
        mat4 transform;
        Model model;
    };

    struct PipelineAddModelEvent
    {
        UUID uuid;
        Model model;
    };

    struct PipelineAttachObjectTo
    {
        UUID parasiteUUID;
        UUID hostUUID;
        std::uint32_t attachPointId;
    };

    struct PipelineSetBoneMapEvent
    {
        UUID uuid;
        BoneMap boneMap;
    };

    struct PipelineSetObjectPositionEvent
    {
        UUID uuid;
        vec3 position;
    };

    struct PipelineInstanceSetModelEvent
    {
        UUID uuid;
        Model model;
    };
}