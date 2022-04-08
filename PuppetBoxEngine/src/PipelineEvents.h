#pragma once

#include "puppetbox/DataStructures.h"
#include "puppetbox/IAnimationCatalogue.h"
#include "puppetbox/IValueReference.h"
#include "puppetbox/RenderData.h"

#define PB_EVENT_PIPELINE_ADD_ANIMATOR_TOPIC            "pb_event_pipeline_add_animator"
#define PB_EVENT_PIPELINE_ADD_ENTITY_TOPIC              "pb_event_pipeline_add_entity"
#define PB_EVENT_PIPELINE_ADD_INSTANCE_SET_TOPIC        "pb_event_pipeline_add_instance_set"
#define PB_EVENT_PIPELINE_ADD_INSTANCE_TO_SET_TOPIC     "pb_event_pipeline_add_instance_to_set"
#define PB_EVENT_PIPELINE_ADD_MODEL_TOPIC               "pb_event_pipeline_add_model"
#define PB_EVENT_PIPELINE_ATTACH_INSTANCE_TO_TOPIC      "pb_event_pipeline_attach_instance_to"
#define PB_EVENT_PIPELINE_BONE_TRANSFORM_TOPIC          "pb_event_pipeline_bone_transform"
#define PB_EVENT_PIPELINE_ENTITY_TRANSFORM_TOPIC        "pb_event_pipeline_entity_transform"
#define PB_EVENT_PIPELINE_GET_ABS_BONE_TRANSFORM_TOPIC  "pb_event_pipeline_get_abs_bone_transform"
#define PB_EVENT_PIPELINE_GET_BONE_TRANSFORM_TOPIC      "pb_event_pipeline_get_bone_transform"
#define PB_EVENT_PIPELINE_SET_BONE_MAP_TOPIC            "pb_event_pipeline_set_bone_map"
#define PB_EVENT_PIPELINE_SET_ENTITY_POSITION_TOPIC     "pb_event_pipeline_set_entity_position"
#define PB_EVENT_PIPELINE_GET_MODEL_LOCATION_TOPIC      "pb_event_pipeline_get_model_location"

namespace PB
{
    struct PipelineAddAnimatorEvent
    {
        UUID uuid;
        std::unique_ptr<IAnimator> animator;
    };

    struct PipelineAddEntityEvent
    {
        UUID uuid;
    };

    struct PipelineAddInstanceEvent
    {
        std::uint32_t id;
        UUID uuid;

    };

    struct PipelineAddInstanceSetEvent
    {
        UUID uuid;
    };

    struct PipelineAddInstanceToSetEvent
    {
        UUID instanceUUID;
        UUID instanceSetUUID;
    };

    struct PipelineAddModelEvent
    {
        UUID uuid;
        Model model;
    };

    struct PipelineAttachInstanceToEvent
    {
        UUID instanceUUID;
        UUID instanceSetUUID;
        UUID hostUUID;
        std::uint32_t attachPointID;
    };

    struct PipelineBoneTransformEvent
    {
        UUID uuid;
        std::vector<mat4> transforms;
    };

    struct PipelineEntityTransformEvent
    {
        UUID uuid;
        mat4 transform;
    };

    struct PipelineGetAbsoluteBoneTransformMatrixReferenceEvent
    {
        UUID uuid;
        std::uint32_t boneId;
        pb_ValueReferenceCallback callback;
    };

    struct PipelineGetBoneTransformReferenceEvent
    {
        UUID uuid;
        std::uint32_t boneId;
        pb_ValueReferenceCallback callback;
    };

    struct PipelineGetModelTransformReferenceEvent
    {
        UUID uuid;
        pb_ValueReferenceCallback callback;
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
}