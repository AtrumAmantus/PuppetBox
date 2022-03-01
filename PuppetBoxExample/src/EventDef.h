#pragma once

#include <cstdint>
#include <functional>

#include <puppetbox/Camera.h>
#include <puppetbox/Constants.h>

#include "AbstractInputProcessor.h"
#include "Command.h"
#include "Entity.h"
#include "UIController.h"

#define PBEX_EVENT_CAMERA                   "pbex_camera_update"
#define PBEX_EVENT_PLAYER                   "pbex_player_update"
#define PBEX_EVENT_PLAYER_LOC               "pbex_player_location"
#define PBEX_EVENT_UI                       "pbex_ui_update"
#define PBEX_EVENT_USER_CHAT                "pbex_user_chat_event"
#define PBEX_EVENT_VIEW_MODE                "pbex_view_mode_update"
#define PBEX_EVENT_TERMINATE_APP            "pbex_terminate_app"
#define PBEX_EVENT_CREATE_ENTITY            "pbex_create_entity_event"
#define PBEX_EVENT_DESTROY_ENTITY           "pbex_destroy_entity_event"
#define PBEX_EVENT_SET_USER_ENTITY          "pbex_set_user_entity_event"
#define PBEX_EVENT_UPDATE_ENTITY_ACTION     "pbex_update_entity_action_event"
#define PBEX_EVENT_UPDATE_ENTITY_LOC        "pbex_update_entity_location_event"

namespace Event::Topic
{
    //TODO: This seems like the incorrect way to do these, prone to error

    // CORE PB
    std::uint32_t NETWORK_TOPIC = 0;
    std::uint32_t NETWORK_STATUS_TOPIC = 0;

    // PB Example
    std::uint32_t UI_TOPIC = 0;
    std::uint32_t TERMINATE_TOPIC = 0;
    std::uint32_t CAMERA_TOPIC = 0;
    std::uint32_t PLAYER_TOPIC = 0;
    std::uint32_t PLAYER_LOC_TOPIC = 0;
    std::uint32_t VIEW_MODE_TOPIC = 0;
    std::uint32_t USER_CHAT_TOPIC = 0;
    std::uint32_t CREATE_ENTITY_TOPIC = 0;
    std::uint32_t DESTROY_ENTITY_TOPIC = 0;
    std::uint32_t SET_USER_ENTITY_TOPIC = 0;
    std::uint32_t ENTITY_UPDATE_ACTION_TOPIC = 0;
    std::uint32_t ENTITY_UPDATE_LOCATION_TOPIC = 0;
}

struct UIControllerEvent
{
    std::function<void(UIController&)> action;
};

struct CameraEvent
{
    std::function<void(PB::Camera&)> action;
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