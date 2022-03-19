#pragma once

enum GameEvent
{
    ACTION_STARTED,
    ACTION_ENDED,
    ADD_ENTITY,
    BONE_CLEAR_OVERRIDE,
    BONE_OVERRIDE,
    CONNECT,
    DISCONNECT,
    LOCATION_UPDATE,
    MESSAGE,
    REMOVE_ENTITY,
    SET_USER_ENTITY
};

extern inline std::string fromGameEvent(GameEvent gameEvent)
{
    std::string value;

    switch (gameEvent)
    {
        case ACTION_STARTED:
            value = "ACTION_STARTED";
            break;
        case ACTION_ENDED:
            value = "ACTION_ENDED";
            break;
        case CONNECT:
            value = "CONNECT";
            break;
        case DISCONNECT:
            value = "DISCONNECT";
            break;
        case LOCATION_UPDATE:
            value = "LOCATION_UPDATE";
            break;
        case MESSAGE:
            value = "MESSAGE";
            break;
        case ADD_ENTITY:
            value = "ADD_ENTITY";
            break;
        case REMOVE_ENTITY:
            value = "REMOVE_ENTITY";
            break;
        case SET_USER_ENTITY:
            value = "SET_USER_ENTITY";
            break;
        default:
            value = "INVALID_GAME_EVENT";
            break;
    }

    return value;
};