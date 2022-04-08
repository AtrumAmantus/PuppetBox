#pragma once

#include "puppetbox/DataStructures.h"
#include "puppetbox/IAnimationCatalogue.h"
#include "puppetbox/IValueReference.h"
#include "puppetbox/RenderData.h"

#define PB_EVENT_PIPELINE_ADD_ANIMATOR_TOPIC            "pb_event_pipeline_add_animator"
#define PB_EVENT_PIPELINE_ADD_ENTITY_TOPIC              "pb_event_pipeline_add_entity"
#define PB_EVENT_PIPELINE_ADD_MODEL_TOPIC               "pb_event_pipeline_add_model"
#define PB_EVENT_PIPELINE_ATTACH_OBJECT_TO_TOPIC        "pb_event_pipeline_attach_object_to"
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

    struct PipelineAddModelEvent
    {
        UUID uuid;
        Model model;
    };

    struct PipelineAttachObjectTo
    {
        UUID parasiteUUID;
        UUID hostUUID;
        std::shared_ptr<IValueReference> reference;
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