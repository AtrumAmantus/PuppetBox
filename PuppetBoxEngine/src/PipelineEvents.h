#pragma once

#define PB_EVENT_PIPELINE_ADD_ANIMATOR_TOPIC        "pb_event_pipeline_add_animator"
#define PB_EVENT_PIPELINE_ADD_ENTITY_TOPIC          "pb_event_pipeline_add_entity"
#define PB_EVENT_PIPELINE_ADD_MODEL_TOPIC           "pb_event_pipeline_add_model"
#define PB_EVENT_PIPELINE_BONE_TRANSFORM_TOPIC      "pb_event_pipeline_bone_transform"
#define PB_EVENT_PIPELINE_ENTITY_TRANSFORM_TOPIC    "pb_event_pipeline_entity_transform"
#define PB_EVENT_PIPELINE_SET_BONE_MAP_TOPIC        "pb_event_pipeline_set_bone_map"
#define PB_EVENT_PIPELINE_SET_ENTITY_POSITION_TOPIC "pb_event_pipeline_set_entity_position"

#include "puppetbox/DataStructures.h"
#include "puppetbox/IAnimationCatalogue.h"

#include "puppetbox/RenderData.h"

namespace PB
{
    struct PipelineAddAnimatorEvent
    {
        UUID uuid;
        std::unique_ptr <IAnimator> animator;
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