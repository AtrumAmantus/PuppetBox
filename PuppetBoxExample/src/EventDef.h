#pragma once

#include <cstdint>
#include <functional>

#include <puppetbox/Camera.h>
#include <puppetbox/Constants.h>

#include "Command.h"
#include "Constants.h"
#include "Entity.h"
#include "UIController.h"

#define PBEX_EVENT_CAMERA                   "pbex_camera_update"
#define PBEX_EVENT_CREATE_ENTITY            "pbex_create_entity_event"
#define PBEX_EVENT_DESTROY_ENTITY           "pbex_destroy_entity_event"
#define PBEX_EVENT_PLAYER                   "pbex_player_update"
#define PBEX_EVENT_PLAYER_CLEAR_BEHAVIOR    "pbex_player_clear_behavior_event"
#define PBEX_EVENT_PLAYER_LOC               "pbex_player_location"
#define PBEX_EVENT_PLAYER_SET_BEHAVIOR      "pbex_player_set_behavior_event"
#define PBEX_EVENT_SET_USER_ENTITY          "pbex_set_user_entity_event"
#define PBEX_EVENT_TERMINATE_APP            "pbex_terminate_app"
#define PBEX_EVENT_UI                       "pbex_ui_update"
#define PBEX_EVENT_UPDATE_ENTITY            "pbex_update_entity_event"
#define PBEX_EVENT_UPDATE_ENTITY_ACTION     "pbex_update_entity_action_event"
#define PBEX_EVENT_UPDATE_ENTITY_LOC        "pbex_update_entity_location_event"
#define PBEX_EVENT_USER_CHAT                "pbex_user_chat_event"
#define PBEX_EVENT_VIEW_MODE                "pbex_view_mode_update"

namespace Event::Topic
{
    // CORE PB
    extern std::uint32_t NETWORK_TOPIC;
    extern std::uint32_t NETWORK_STATUS_TOPIC;

    // PB Example
    extern std::uint32_t UI_TOPIC;
    extern std::uint32_t TERMINATE_TOPIC;
    extern std::uint32_t CAMERA_TOPIC;
    extern std::uint32_t PLAYER_TOPIC;
    extern std::uint32_t PLAYER_LOC_TOPIC;
    extern std::uint32_t VIEW_MODE_TOPIC;
    extern std::uint32_t USER_CHAT_TOPIC;
    extern std::uint32_t CREATE_ENTITY_TOPIC;
    extern std::uint32_t DESTROY_ENTITY_TOPIC;
    extern std::uint32_t SET_USER_ENTITY_TOPIC;
    extern std::uint32_t ENTITY_UPDATE_ACTION_TOPIC;
    extern std::uint32_t ENTITY_UPDATE_LOCATION_TOPIC;
    extern std::uint32_t UPDATE_ENTITY_TOPIC;
    extern std::uint32_t PLAYER_SET_BEHAVIOR_TOPIC;
    extern std::uint32_t PLAYER_CLEAR_BEHAVIOR_TOPIC;
}

struct UIControllerEvent
{
    std::function<void(UIController&)> action;
};

struct CameraEvent
{
    std::function<void(PB::Camera&)> action;
};

struct PlayerSetBehaviorEvent
{
     Constants::Behavior::Behavior behavior = Constants::Behavior::NONE;
};

struct PlayerClearBehaviorEvent
{

};

struct PlayerEvent
{
    std::uint32_t commandCount;
    CommandState commandStates[];
};

struct PlayerLocationEvent
{
    PB::vec3 location{};
};

struct ViewModeEvent
{
    PB::SceneView::Mode mode;
};

struct UserChatEvent
{
    std::string message;
};

// Incoming network events

struct CreateEntityEvent
{
    PB::UUID uuid{};
    PB::vec3 position{};
};

struct DestroyEntityEvent
{
    PB::UUID uuid{};
};

struct SetUserEntityEvent
{
    PB::UUID uuid{};
};

struct UpdateEntityActionEvent
{
    PB::UUID uuid{};
    CommandState commandState;
};

struct UpdateEntityLocationEvent
{
    PB::UUID uuid{};
    PB::vec3 location{};
};

struct UpdateEntityEvent
{
    std::function<void(Entity*)> action;
};