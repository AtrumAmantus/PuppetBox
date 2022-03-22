#pragma once

#include <iosfwd>
#include <memory>
#include <sstream>
#include <string>

#include <PuppetBox.h>
#include <puppetbox/DataStructures.h>

#include "EventDef.h"
#include "GameEvent.h"

#define BIG_ENDIAN 0

bool _networkInitialized = false;
bool _networkConnected = false;
std::vector<PB::UUID> _subscriptions{};

static std::uint8_t getHostEndian()
{
    std::uint32_t n = 1;
    std::uint8_t* p = (uint8_t*) &n;
    return *p;
}

bool isHostBigEndian = getHostEndian() == BIG_ENDIAN;

const std::uint32_t endianInt32Byte0 = isHostBigEndian ? 0 : 3;
const std::uint32_t endianInt32Byte1 = isHostBigEndian ? 1 : 2;
const std::uint32_t endianInt32Byte2 = isHostBigEndian ? 2 : 1;
const std::uint32_t endianInt32Byte3 = isHostBigEndian ? 3 : 0;

const std::uint8_t endianInt32Shift0 = isHostBigEndian ? 0 : 24;
const std::uint8_t endianInt32Shift1 = isHostBigEndian ? 8 : 16;
const std::uint8_t endianInt32Shift2 = isHostBigEndian ? 16 : 8;
const std::uint8_t endianInt32Shift3 = isHostBigEndian ? 24 : 0;

bool networkInitialized()
{
    return _networkInitialized;
}

bool networkConnected()
{
    return _networkConnected;
}

std::vector<PB::UUID>& getSubscriptionUUIDs()
{
    return _subscriptions;
}

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

static inline void writeUInt32(std::uint32_t value, std::uint8_t* dataOut, std::uint32_t* dataOffset)
{
    std::uint8_t bytes[4];

    getBytesFromUInt32(bytes, value);
    dataOut[(*dataOffset)++] = bytes[0];
    dataOut[(*dataOffset)++] = bytes[1];
    dataOut[(*dataOffset)++] = bytes[2];
    dataOut[(*dataOffset)++] = bytes[3];
}

static inline void writeFloat(float value, std::uint8_t* dataOut, std::uint32_t* dataOffset)
{
    std::uint8_t bytes[4];

    getBytesFromFloat(bytes, value);
    dataOut[(*dataOffset)++] = bytes[0];
    dataOut[(*dataOffset)++] = bytes[1];
    dataOut[(*dataOffset)++] = bytes[2];
    dataOut[(*dataOffset)++] = bytes[3];
}

static inline std::uint32_t getUInt32FromBytes(std::uint8_t* data, std::uint32_t* offset)
{
    return (data[(*offset)++] << endianInt32Shift0)
           | (data[(*offset)++] << endianInt32Shift1)
           | (data[(*offset)++] << endianInt32Shift2)
           | (data[(*offset)++] << endianInt32Shift3);
}

static inline PB::UUID getUuidFromBytes(std::uint8_t* data, std::uint32_t* offset)
{
    const std::uint32_t a = getUInt32FromBytes(data, offset);
    const std::uint32_t b = getUInt32FromBytes(data, offset);
    const std::uint32_t c = getUInt32FromBytes(data, offset);
    const std::uint32_t d = getUInt32FromBytes(data, offset);

    return PB::UUID{a, b, c, d};
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

static inline std::string getStringFromBytes(std::uint8_t* data, std::uint32_t* offset, std::uint32_t dataLength)
{
    std::ostringstream stringBuilder;

    while (*offset < dataLength)
    {
        stringBuilder << data[(*offset)++];
    }

    return stringBuilder.str();
}

static inline void parseEventActionStarted(std::uint8_t* data, std::uint32_t dataOffset)
{
    auto event = std::make_shared<UpdateEntityActionEvent>();
    event->uuid = getUuidFromBytes(data, &dataOffset);

    event->commandState = CommandState{
            (Command::Command) getUInt32FromBytes(data, &dataOffset),
            true
    };

    PB::PublishEvent(Event::Topic::ENTITY_UPDATE_ACTION_TOPIC, event);
}

static inline void parseEventActionEnded(std::uint8_t* data, std::uint32_t dataOffset)
{
    auto event = std::make_shared<UpdateEntityActionEvent>();
    event->uuid = getUuidFromBytes(data, &dataOffset);

    event->commandState = CommandState{
            (Command::Command) getUInt32FromBytes(data, &dataOffset),
            false
    };

    PB::PublishEvent(Event::Topic::ENTITY_UPDATE_ACTION_TOPIC, event);
}

static inline void parseEventAddToInventory(std::uint8_t* data, std::uint32_t dataOffset, std::uint32_t dataLength)
{
    auto event = std::make_shared<AddToInventoryEvent>();
    event->mobUUID = getUuidFromBytes(data, &dataOffset);
    event->itemUUID = getUuidFromBytes(data, &dataOffset);
    event->equipSlot = getUInt32FromBytes(data, &dataOffset);
    event->itemType = getStringFromBytes(data, &dataOffset, dataLength);

    PB::PublishEvent(Event::Topic::ADD_TO_INVENTORY_TOPIC, event);

#ifdef _DEBUG
    std::cout << "Add item with UUID: " << event->itemUUID << ", "
              << "To mob with UUID: " << event->mobUUID << ", "
              << "Slot: {" << event->equipSlot << "}, "
              << "Type: '" << event->itemType << "'" << std::endl;
#endif
}

static inline void parseBoneClearOverride(std::uint8_t* data, std::uint32_t dataOffset)
{
    auto event = std::make_shared<BoneClearOverrideEvent>();
    event->uuid = getUuidFromBytes(data, &dataOffset);
    event->boneId = getUInt32FromBytes(data, &dataOffset);

    PB::PublishEvent(Event::Topic::BONE_CLEAR_OVERRIDE_TOPIC, event);
}

static inline void parseBoneOverride(std::uint8_t* data, std::uint32_t dataOffset)
{
    auto event = std::make_shared<BoneOverrideEvent>();
    event->uuid = getUuidFromBytes(data, &dataOffset);
    event->boneId = getUInt32FromBytes(data, &dataOffset);
    event->rotation.x = getFloatFromBytes(data, &dataOffset);
    event->rotation.y = getFloatFromBytes(data, &dataOffset);
    event->rotation.z = getFloatFromBytes(data, &dataOffset);

    PB::PublishEvent(Event::Topic::BONE_OVERRIDE_TOPIC, event);
}

static inline void parseEventConnect(std::uint8_t* data, std::uint32_t dataOffset)
{
    std::uint32_t userId = getUInt32FromBytes(data, &dataOffset);

    std::cout << "User (" << userId << ") Connected" << std::endl;
}

static inline void parseEventDisconnect(std::uint8_t* data, std::uint32_t dataOffset)
{
    std::uint32_t userId = getUInt32FromBytes(data, &dataOffset);

    std::cout << "User (" << userId << ") Disconnected" << std::endl;
}

static inline void parseEventEquipItem(std::uint8_t* data, std::uint32_t dataOffset, std::uint32_t dataLength)
{
    auto event = std::make_shared<EquipItemEvent>();
    event->mobUUID = getUuidFromBytes(data, &dataOffset);
    event->itemUUID = getUuidFromBytes(data, &dataOffset);
    event->itemType = getStringFromBytes(data, &dataOffset, dataLength);

#ifdef _DEBUG
    std::cout << "Equip item on entity with UUID: " << event->mobUUID << ", "
              << "Item UUID: " << event->itemUUID
              << "Item Type: '" << event->itemType << "'" << std::endl;
#endif

    PB::PublishEvent(Event::Topic::EQUIP_ITEM_TOPIC, event);
}

static inline void parseEventLocationUpdate(std::uint8_t* data, std::uint32_t dataOffset)
{
    auto event = std::make_shared<UpdateEntityLocationEvent>();
    event->uuid = getUuidFromBytes(data, &dataOffset);
    event->location.x = getFloatFromBytes(data, &dataOffset);
    event->location.y = getFloatFromBytes(data, &dataOffset);
    event->location.z = getFloatFromBytes(data, &dataOffset);

    PB::PublishEvent(Event::Topic::ENTITY_UPDATE_LOCATION_TOPIC, event);
}

static inline void parseEventMessage(std::uint8_t* data, std::uint32_t dataOffset, std::uint32_t dataLength)
{
    std::cout << "MESSAGE event parsing not implemented" << std::endl;
}

static inline void parseEventCreateEntity(std::uint8_t* data, std::uint32_t dataOffset, std::uint32_t dataLength)
{
    auto event = std::make_shared<CreateEntityEvent>();

    event->uuid = getUuidFromBytes(data, &dataOffset);
    event->position.x = getFloatFromBytes(data, &dataOffset);
    event->position.y = getFloatFromBytes(data, &dataOffset);
    event->position.z = getFloatFromBytes(data, &dataOffset);
    event->type = getStringFromBytes(data, &dataOffset, dataLength);

#ifdef _DEBUG
    std::cout << "Add entity with UUID: " << event->uuid << ", "
              << "Loc: {" << event->position.x << ", " << event->position.y << ", " << event->position.z << "}, "
              << "Type: '" << event->type << "'" << std::endl;
#endif

    PB::PublishEvent(Event::Topic::CREATE_ENTITY_TOPIC, event);
}

static inline void parseEventDestroyEntity(std::uint8_t* data, std::uint32_t dataOffset)
{
    PB::UUID uuid = getUuidFromBytes(data, &dataOffset);

    std::cout << "Remove entity with UUID: " << uuid << std::endl;

    auto event = std::make_shared<DestroyEntityEvent>();
    event->uuid = uuid;
    PB::PublishEvent(Event::Topic::DESTROY_ENTITY_TOPIC, event);
}

static inline void parseEventSetUserEntity(std::uint8_t* data, std::uint32_t dataOffset)
{
    PB::UUID uuid = getUuidFromBytes(data, &dataOffset);

    std::cout << "Set user control to entity with UUID: " << uuid << std::endl;

    auto event = std::make_shared<SetUserEntityEvent>();
    event->uuid = uuid;
    PB::PublishEvent(Event::Topic::SET_USER_ENTITY_TOPIC, event);
}

extern void networkReader(std::uint8_t* data, std::uint32_t dataLength)
{
#ifdef _DEBUG
    std::cout << "Received " << dataLength << " bytes of data" << std::endl;
    std::cout << "Headers:" << std::endl;
    std::cout << "\tMessage Length: " << dataLength << std::endl;
    std::cout << "Raw Packet:" << std::endl << "\t";

    for (std::uint32_t i = 0; i < dataLength; ++i)
    {
        std::cout << (int) data[i] << ", ";
    }

    std::cout << std::endl;
#endif

    // Ignore message length bytes
    std::uint32_t dataOffset = 4;

    GameEvent gameEvent = (GameEvent) getUInt32FromBytes(data, &dataOffset);

#ifdef _DEBUG
    std::cout << "GameEvent: " << fromGameEvent(gameEvent) << std::endl;
#endif

    switch (gameEvent)
    {
        case ACTION_STARTED:
            parseEventActionStarted(data, dataOffset);
            break;
        case ACTION_ENDED:
            parseEventActionEnded(data, dataOffset);
            break;
        case ADD_ENTITY:
            parseEventCreateEntity(data, dataOffset, dataLength);
            break;
        case ADD_TO_INVENTORY:
            parseEventAddToInventory(data, dataOffset, dataLength);
            break;
        case BONE_CLEAR_OVERRIDE:
            parseBoneClearOverride(data, dataOffset);
            break;
        case BONE_OVERRIDE:
            parseBoneOverride(data, dataOffset);
            break;
        case CONNECT:
            parseEventConnect(data, dataOffset);
            break;
        case DISCONNECT:
            parseEventDisconnect(data, dataOffset);
            break;
        case EQUIP_ITEM:
            parseEventEquipItem(data, dataOffset, dataLength);
            break;
        case LOCATION_UPDATE:
            parseEventLocationUpdate(data, dataOffset);
            break;
        case MESSAGE:
            parseEventMessage(data, dataOffset, dataLength);
            break;
        case REMOVE_ENTITY:
            parseEventDestroyEntity(data, dataOffset);
            break;
        case SET_USER_ENTITY:
            parseEventSetUserEntity(data, dataOffset);
            break;
        default:
            std::cout << "Unhandled event: " << fromGameEvent(gameEvent) << std::endl;
            break;
    }
}

extern void networkWriterUserChatEvent(std::shared_ptr<void> event, std::uint8_t** dataOut, std::uint32_t* dataLength)
{
    auto chatEvent = std::static_pointer_cast<UserChatEvent>(event);

    // +4 bytes for event id
    *dataLength = chatEvent->message.size() + 4;
    *dataOut = new uint8_t[*dataLength];
    std::uint32_t dataOffset = 0;

    std::uint32_t eventID = MESSAGE;
    writeUInt32(eventID, *dataOut, &dataOffset);

    for (std::uint32_t i = 0; i < chatEvent->message.size(); ++i)
    {
        (*dataOut)[dataOffset++] = chatEvent->message.c_str()[i];
    }
}

extern void networkWriterPlayerEvent(std::shared_ptr<void> event, std::uint8_t** dataOut, std::uint32_t* dataLength)
{
    auto playerEvent = std::static_pointer_cast<PlayerEvent>(event);

    // 4 bytes for playerEvent id, +4 bytes for action type
    *dataLength = playerEvent->commandCount * (4 + 4);
    *dataOut = new uint8_t[*dataLength];

    std::uint32_t dataOffset = 0;

    for (std::uint32_t i = 0; i < playerEvent->commandCount; ++i)
    {
        std::uint32_t eventID =
                playerEvent->commandStates[i].activated ? ACTION_STARTED : ACTION_ENDED;
        writeUInt32(eventID, *dataOut, &dataOffset);

        std::uint32_t actionID = playerEvent->commandStates[i].command;
        writeUInt32(actionID, *dataOut, &dataOffset);
    }
}

extern void networkWriterPlayerLocationEvent(
        std::shared_ptr<void> event,
        std::uint8_t** dataOut,
        std::uint32_t* dataLength)
{
    auto playerLocEvent = std::static_pointer_cast<PlayerLocationEvent>(event);

    // 4 bytes eventId, +4 bytes for each x/y/z coord.
    *dataLength = 4 + (3 * 4);
    *dataOut = new uint8_t[*dataLength];
    std::uint32_t dataOffset = 0;

    std::uint32_t eventID = LOCATION_UPDATE;
    writeUInt32(eventID, *dataOut, &dataOffset);
    writeFloat(playerLocEvent->location.x, *dataOut, &dataOffset);
    writeFloat(playerLocEvent->location.y, *dataOut, &dataOffset);
    writeFloat(playerLocEvent->location.z, *dataOut, &dataOffset);
}

extern void networkWriterPlayerBoneOverrideEvent(
        std::shared_ptr<void> event,
        std::uint8_t** dataOut,
        std::uint32_t* dataLength)
{
    auto data = std::static_pointer_cast<PlayerBoneOverrideEvent>(event);

    // 4 bytes eventId, +4 bytes for bone ID, +4 bytes for each x/y/z coord.
    *dataLength = 4 + 4 + (3 * 4);
    *dataOut = new uint8_t[*dataLength];
    std::uint32_t dataOffset = 0;

    std::uint32_t eventID = BONE_OVERRIDE;
    writeUInt32(eventID, *dataOut, &dataOffset);
    writeUInt32(data->boneId, *dataOut, &dataOffset);
    writeFloat(data->override.x, *dataOut, &dataOffset);
    writeFloat(data->override.y, *dataOut, &dataOffset);
    writeFloat(data->override.z, *dataOut, &dataOffset);
}

extern void networkWriterPlayerClearBoneOverrideEvent(
        std::shared_ptr<void> event,
        std::uint8_t** dataOut,
        std::uint32_t* dataLength)
{
    auto data = std::static_pointer_cast<PlayerClearBoneOverrideEvent>(event);

    // 4 bytes eventId, +4 bytes for bone ID
    *dataLength = 4 + 4;
    *dataOut = new uint8_t[*dataLength];
    std::uint32_t dataOffset = 0;

    std::uint32_t eventID = BONE_CLEAR_OVERRIDE;
    writeUInt32(eventID, *dataOut, &dataOffset);
    writeUInt32(data->boneId, *dataOut, &dataOffset);
}

extern void networkWriterPlayerEquipItem(std::shared_ptr<void> event, std::uint8_t** dataOut, std::uint32_t* dataLength)
{
    auto data = std::static_pointer_cast<PlayerEquipItemEvent>(event);
    std::uint8_t bytes[4];

    // 4 bytes eventId, +4 bytes for equip slot
    *dataLength = 4 + 4;
    *dataOut = new uint8_t[*dataLength];
    std::uint32_t dataOffset = 0;

    std::uint32_t eventID = EQUIP_ITEM;
    writeUInt32(eventID, *dataOut, &dataOffset);
    writeUInt32(data->equipSlot, *dataOut, &dataOffset);
}

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
                PB::RegisterNetworkEventWriter(PBEX_EVENT_PLAYER_BONE_OVERRIDE, &networkWriterPlayerBoneOverrideEvent);
                PB::RegisterNetworkEventWriter(PBEX_EVENT_PLAYER_BONE_CLEAR_OVERRIDE, &networkWriterPlayerClearBoneOverrideEvent);
                PB::RegisterNetworkEventWriter(PBEX_EVENT_PLAYER_EQUIP_ITEM, &networkWriterPlayerEquipItem);
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
}