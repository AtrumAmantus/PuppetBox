#pragma once

#include <cstdint>
#include <functional>

#include <puppetbox/Camera.h>
#include <puppetbox/Constants.h>

#include "AbstractInputProcessor.h"
#include "Entity.h"
#include "UIController.h"

#define PBEX_EVENT_CAMERA           "pbex_camera_update"
#define PBEX_EVENT_PLAYER           "pbex_player_update"
#define PBEX_EVENT_UI               "pbex_ui_update"
#define PBEX_EVENT_USER_CHAT        "pbex_user_chat_event"
#define PBEX_EVENT_VIEW_MODE        "pbex_view_mode_update"
#define PBEX_EVENT_TERMINATE_APP    "pbex_terminate_app"

namespace Event::Topic
{
    //TODO: This seems like the incorrect way to do these, prone to error

    // CORE PB
    std::uint32_t NETWORK_TOPIC = 0;

    // PB Example
    std::uint32_t UI_TOPIC = 0;
    std::uint32_t TERMINATE_TOPIC = 0;
    std::uint32_t CAMERA_TOPIC = 0;
    std::uint32_t PLAYER_TOPIC = 0;
    std::uint32_t VIEW_MODE_TOPIC = 0;
    std::uint32_t USER_CHAT_TOPIC = 0;
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
    std::function<void(Entity&)> action;
};

struct ViewModeEvent
{
    PB::SceneView::Mode mode;
};

struct UserChatEvent
{
    std::string message;
};