#pragma once

#define PB_EVENT_PIPELINE_ADD_ANIMATOR_TOPIC    "pb_event_pipeline_add_animator"
#define PB_EVENT_PIPELINE_ADD_ENTITY_TOPIC      "pb_event_pipeline_add_entity"
#define PB_EVENT_PIPELINE_ADD_MODEL_TOPIC       "pb_event_pipeline_add_model"
#define PB_EVENT_PIPELINE_BONE_TRANSFORM_TOPIC  "pb_event_pipeline_bone_transform"

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
}