#pragma once

#include <memory>

#include <PuppetBox.h>
#include <puppetbox/DataStructures.h>

#include "EventDef.h"
#include "GameEvent.h"

#define BIG_ENDIAN 0

static bool _networkInitialized = false;
static bool _networkConnected = false;
static std::vector<PB::UUID> _subscriptions{};

static std::uint8_t getHostEndian()
{
    std::uint32_t n = 1;
    std::uint8_t* p = (uint8_t*) &n;
    return *p;
};

static bool isHostBigEndian = getHostEndian() == BIG_ENDIAN;

static const std::uint32_t endianInt32Byte0 = isHostBigEndian ? 0 : 3;
static const std::uint32_t endianInt32Byte1 = isHostBigEndian ? 1 : 2;
static const std::uint32_t endianInt32Byte2 = isHostBigEndian ? 2 : 1;
static const std::uint32_t endianInt32Byte3 = isHostBigEndian ? 3 : 0;

static const std::uint8_t endianInt32Shift0 = isHostBigEndian ? 0 : 24;
static const std::uint8_t endianInt32Shift1 = isHostBigEndian ? 8 : 16;
static const std::uint8_t endianInt32Shift2 = isHostBigEndian ? 16 : 8;
static const std::uint8_t endianInt32Shift3 = isHostBigEndian ? 24 : 0;

extern bool networkInitialized()
{
    return _networkInitialized;
};

extern bool networkConnected()
{
    return _networkConnected;
};

extern std::vector<PB::UUID>& getSubscriptionUUIDs()
{
    return _subscriptions;
};

static inline void getBytesFromUInt32(std::uint8_t bytes[4], std::uint32_t value)
{
    std::uint8_t* valueBytes = reinterpret_cast<std::uint8_t*>(&value);

    bytes[0] = valueBytes[endianInt32Byte0];
    bytes[1] = valueBytes[endianInt32Byte1];
    bytes[2] = valueBytes[endianInt32Byte2];
    bytes[3] = valueBytes[endianInt32Byte3];
}

static inline void getBytesFromFloat(std::uint8_t bytes[4], float value)
{
    std::uint8_t* valueBytes = reinterpret_cast<std::uint8_t*>(&value);

    bytes[0] = valueBytes[endianInt32Byte0];
    bytes[1] = valueBytes[endianInt32Byte1];
    bytes[2] = valueBytes[endianInt32Byte2];
    bytes[3] = valueBytes[endianInt32Byte3];
}

static inline std::uint32_t getUInt32FromBytes(std::uint8_t* data, std::uint32_t* offset)
{
    return (data[(*offset)++] << endianInt32Shift0)
           | (data[(*offset)++] << endianInt32Shift1)
           | (data[(*offset)++] << endianInt32Shift2)
           | (data[(*offset)++] << endianInt32Shift3);
}

static inline float getFloatFromBytes(std::uint8_t* data, std::uint32_t* offset)
{
    float f;
    std::uint8_t b[] = {
            data[(*offset + (endianInt32Shift0 / 8))],
            data[(*offset + (endianInt32Shift1 / 8))],
            data[(*offset + (endianInt32Shift2 / 8))],
            data[(*offset + (endianInt32Shift3 / 8))]
    };

    memcpy(&f, &b, sizeof(f));

    *offset += 4;
    return f;
}

static inline void parseEventActionStarted(std::uint8_t* data, std::uint32_t dataOffset)
{
    auto event = std::make_shared<UpdateEntityActionEvent>();
    event->uuid = PB::UUID(
            getUInt32FromBytes(data, &dataOffset),
            getUInt32FromBytes(data, &dataOffset),
            getUInt32FromBytes(data, &dataOffset),
            getUInt32FromBytes(data, &dataOffset)
    );

    event->commandState = CommandState{
            (Command::Command) getUInt32FromBytes(data, &dataOffset),
            true
    };

    PB::PublishEvent(Event::Topic::ENTITY_UPDATE_ACTION_TOPIC, event);
}

static inline void parseEventActionEnded(std::uint8_t* data, std::uint32_t dataOffset)
{
    auto event = std::make_shared<UpdateEntityActionEvent>();
    event->uuid = PB::UUID(
            getUInt32FromBytes(data, &dataOffset),
            getUInt32FromBytes(data, &dataOffset),
            getUInt32FromBytes(data, &dataOffset),
            getUInt32FromBytes(data, &dataOffset)
    );

    event->commandState = CommandState{
            (Command::Command) getUInt32FromBytes(data, &dataOffset),
            false
    };

    PB::PublishEvent(Event::Topic::ENTITY_UPDATE_ACTION_TOPIC, event);
};

static inline void parseEventConnect(std::uint8_t* data, std::uint32_t dataOffset)
{
    std::uint32_t userId = getUInt32FromBytes(data, &dataOffset);

    std::cout << "User (" << userId << ") Connected" << std::endl;
};

static inline void parseEventDisconnect(std::uint8_t* data, std::uint32_t dataOffset)
{
    std::uint32_t userId = getUInt32FromBytes(data, &dataOffset);

    std::cout << "User (" << userId << ") Disconnected" << std::endl;
};

static inline void parseEventLocationUpdate(std::uint8_t* data, std::uint32_t dataOffset)
{
    auto event = std::make_shared<UpdateEntityLocationEvent>();
    event->uuid = PB::UUID(
            getUInt32FromBytes(data, &dataOffset),
            getUInt32FromBytes(data, &dataOffset),
            getUInt32FromBytes(data, &dataOffset),
            getUInt32FromBytes(data, &dataOffset)
    );
    event->location = {
            getFloatFromBytes(data, &dataOffset),
            getFloatFromBytes(data, &dataOffset),
            getFloatFromBytes(data, &dataOffset)
    };

    PB::PublishEvent(Event::Topic::ENTITY_UPDATE_LOCATION_TOPIC, event);
};

static inline void parseEventMessage(std::uint8_t* data, std::uint32_t dataOffset)
{
    std::cout << "MESSAGE event parsing not implemented" << std::endl;
};

static inline void parseEventCreateEntity(std::uint8_t* data, std::uint32_t dataOffset)
{
    PB::UUID uuid = PB::UUID(
            getUInt32FromBytes(data, &dataOffset),
            getUInt32FromBytes(data, &dataOffset),
            getUInt32FromBytes(data, &dataOffset),
            getUInt32FromBytes(data, &dataOffset));

    float x = getFloatFromBytes(data, &dataOffset);
    float y = getFloatFromBytes(data, &dataOffset);
    float z = getFloatFromBytes(data, &dataOffset);

    std::cout << "Add entity with UUID: " << uuid << ", Loc: {" << x << ", " << y << ", " << z << "}" << std::endl;

    auto event = std::make_shared<CreateEntityEvent>();
    event->uuid = uuid;
    event->position = {x, y, z};
    PB::PublishEvent(Event::Topic::CREATE_ENTITY_TOPIC, event);
};

static inline void parseEventDestroyEntity(std::uint8_t* data, std::uint32_t dataOffset)
{
    PB::UUID uuid = PB::UUID(
            getUInt32FromBytes(data, &dataOffset),
            getUInt32FromBytes(data, &dataOffset),
            getUInt32FromBytes(data, &dataOffset),
            getUInt32FromBytes(data, &dataOffset));

    std::cout << "Remove entity with UUID: " << uuid << std::endl;

    auto event = std::make_shared<DestroyEntityEvent>();
    event->uuid = uuid;
    PB::PublishEvent(Event::Topic::DESTROY_ENTITY_TOPIC, event);
};

static inline void parseEventSetUserEntity(std::uint8_t* data, std::uint32_t dataOffset)
{
    PB::UUID uuid = PB::UUID(
            getUInt32FromBytes(data, &dataOffset),
            getUInt32FromBytes(data, &dataOffset),
            getUInt32FromBytes(data, &dataOffset),
            getUInt32FromBytes(data, &dataOffset));

    std::cout << "Set user control to entity with UUID: " << uuid << std::endl;

    auto event = std::make_shared<SetUserEntityEvent>();
    event->uuid = uuid;
    PB::PublishEvent(Event::Topic::SET_USER_ENTITY_TOPIC, event);
};

extern void networkReader(std::uint8_t* data, std::uint32_t dataLength)
{
    std::cout << "Received " << dataLength << " bytes of data" << std::endl;
    std::cout << "Headers:" << std::endl;
    std::cout << "\tMessage Length: " << dataLength << std::endl;
    std::cout << "Raw Packet:" << std::endl << "\t";

    for (std::uint32_t i = 0; i < dataLength; ++i)
    {
        std::cout << (int) data[i] << ", ";
    }

    std::cout << std::endl;

    // Ignore message length bytes
    std::uint32_t dataOffset = 4;

    GameEvent gameEvent = (GameEvent) getUInt32FromBytes(data, &dataOffset);

    std::cout << "GameEvent: " << fromGameEvent(gameEvent) << std::endl;

    switch (gameEvent)
    {
        case ACTION_STARTED:
            parseEventActionStarted(data, dataOffset);
            break;
        case ACTION_ENDED:
            parseEventActionEnded(data, dataOffset);
            break;
        case CONNECT:
            parseEventConnect(data, dataOffset);
            break;
        case DISCONNECT:
            parseEventDisconnect(data, dataOffset);
            break;
        case LOCATION_UPDATE:
            parseEventLocationUpdate(data, dataOffset);
            break;
        case MESSAGE:
            parseEventMessage(data, dataOffset);
            break;
        case ADD_ENTITY:
            parseEventCreateEntity(data, dataOffset);
            break;
        case REMOVE_ENTITY:
            parseEventDestroyEntity(data, dataOffset);
            break;
        case SET_USER_ENTITY:
            parseEventSetUserEntity(data, dataOffset);
            break;
        default:
            break;
    }
};

extern void networkWriterUserChatEvent(std::shared_ptr<void> event, std::uint8_t** dataOut, std::uint32_t* dataLength)
{
    auto chatEvent = std::static_pointer_cast<UserChatEvent>(event);

    // +4 bytes for event id
    *dataLength = chatEvent->message.size() + 4;
    *dataOut = new uint8_t[*dataLength];
    std::uint32_t dataOffset = 0;
    std::uint8_t bytes[4];

    std::uint32_t eventID = MESSAGE;
    getBytesFromUInt32(bytes, eventID);
    (*dataOut)[dataOffset++] = bytes[0];
    (*dataOut)[dataOffset++] = bytes[1];
    (*dataOut)[dataOffset++] = bytes[2];
    (*dataOut)[dataOffset++] = bytes[3];

    for (std::uint32_t i = 0; i < chatEvent->message.size(); ++i)
    {
        (*dataOut)[dataOffset++] = chatEvent->message.c_str()[i];
    }
};

extern void networkWriterPlayerEvent(std::shared_ptr<void> event, std::uint8_t** dataOut, std::uint32_t* dataLength)
{
    auto playerEvent = std::static_pointer_cast<PlayerEvent>(event);

    // 4 bytes for playerEvent id, +4 bytes for action type
    *dataLength = playerEvent->commandCount * (4 + 4);
    *dataOut = new uint8_t[*dataLength];

    std::uint32_t dataOffset = 0;
    std::uint8_t bytes[4];

    for (std::uint32_t i = 0; i < playerEvent->commandCount; ++i)
    {
        std::uint32_t eventID =
                playerEvent->commandStates[i].activated ? ACTION_STARTED : ACTION_ENDED;
        getBytesFromUInt32(bytes, eventID);
        (*dataOut)[dataOffset++] = bytes[0];
        (*dataOut)[dataOffset++] = bytes[1];
        (*dataOut)[dataOffset++] = bytes[2];
        (*dataOut)[dataOffset++] = bytes[3];

        std::uint32_t actionID = playerEvent->commandStates[i].command;
        getBytesFromUInt32(bytes, actionID);
        (*dataOut)[dataOffset++] = bytes[0];
        (*dataOut)[dataOffset++] = bytes[1];
        (*dataOut)[dataOffset++] = bytes[2];
        (*dataOut)[dataOffset++] = bytes[3];
    }
};

extern void
networkWriterPlayerLocationEvent(std::shared_ptr<void> event, std::uint8_t** dataOut, std::uint32_t* dataLength)
{
    auto playerLocEvent = std::static_pointer_cast<PlayerLocationEvent>(event);
    std::uint8_t bytes[4];

    // 4 bytes eventId, +4 bytes for each x/y/z coord.
    *dataLength = 4 + (3 * 4);
    *dataOut = new uint8_t[*dataLength];
    std::uint32_t dataOffset = 0;

    std::uint32_t eventID = LOCATION_UPDATE;
    getBytesFromUInt32(bytes, eventID);
    (*dataOut)[dataOffset++] = bytes[0];
    (*dataOut)[dataOffset++] = bytes[1];
    (*dataOut)[dataOffset++] = bytes[2];
    (*dataOut)[dataOffset++] = bytes[3];

    getBytesFromFloat(bytes, playerLocEvent->location.x);
    (*dataOut)[dataOffset++] = bytes[0];
    (*dataOut)[dataOffset++] = bytes[1];
    (*dataOut)[dataOffset++] = bytes[2];
    (*dataOut)[dataOffset++] = bytes[3];

    getBytesFromFloat(bytes, playerLocEvent->location.y);
    (*dataOut)[dataOffset++] = bytes[0];
    (*dataOut)[dataOffset++] = bytes[1];
    (*dataOut)[dataOffset++] = bytes[2];
    (*dataOut)[dataOffset++] = bytes[3];

    getBytesFromFloat(bytes, playerLocEvent->location.z);
    (*dataOut)[dataOffset++] = bytes[0];
    (*dataOut)[dataOffset++] = bytes[1];
    (*dataOut)[dataOffset++] = bytes[2];
    (*dataOut)[dataOffset++] = bytes[3];
};

extern void networkReadyStatusEvent(std::shared_ptr<void> event)
{
    auto networkEvent = std::static_pointer_cast<PB::NetworkStatusEvent>(event);

    switch (networkEvent->status)
    {
        case PB::Event::NetworkStatus::READY:
            if (!_networkInitialized)
            {
                _networkInitialized = true;

                // Topic registrations
                Event::Topic::USER_CHAT_TOPIC = PB::RegisterTopic(PBEX_EVENT_USER_CHAT);
                Event::Topic::PLAYER_TOPIC = PB::RegisterTopic(PBEX_EVENT_PLAYER);
                Event::Topic::PLAYER_LOC_TOPIC = PB::RegisterTopic(PBEX_EVENT_PLAYER_LOC);

                // Network Reader
                PB::RegisterNetworkEventReader(&networkReader);

                // Network Writers
                PB::RegisterNetworkEventWriter(PBEX_EVENT_USER_CHAT, &networkWriterUserChatEvent);
                PB::RegisterNetworkEventWriter(PBEX_EVENT_PLAYER, &networkWriterPlayerEvent);
                PB::RegisterNetworkEventWriter(PBEX_EVENT_PLAYER_LOC, &networkWriterPlayerLocationEvent);
            }
            break;
        case PB::Event::NetworkStatus::CONNECTED:
            _networkConnected = true;
            std::cout << "NETWORK CONNECTED" << std::endl;
            break;
        case PB::Event::NetworkStatus::DISCONNECTED:
            _networkConnected = false;
            std::cout << "NETWORK DISCONNECTED" << std::endl;
            break;
        default:
            break;
    }
};