#include "Logger.h"
#include "Networking.h"

namespace PB::Networking
{
    bool connect(NetworkingDetails* networkingDetails, std::string host, std::uint16_t port)
    {
        bool success = true;

        if (SDLNet_ResolveHost(&networkingDetails->ip, host.c_str(), port) != -1)
        {
            networkingDetails->socket = SDLNet_TCP_Open(&networkingDetails->ip);

            if (networkingDetails->socket != NULL)
            {
                networkingDetails->socket_set = SDLNet_AllocSocketSet(1);

                if (networkingDetails->socket_set != NULL)
                {
                    if (SDLNet_TCP_AddSocket(networkingDetails->socket_set, networkingDetails->socket) != -1)
                    {
                        // Success!
                        networkingDetails->isConnected = true;
                    }
                    else
                    {
                        success = false;
                        LOGGER_ERROR("ER: SDLNet_TCP_AddSocket");
                        LOGGER_ERROR(SDLNet_GetError());
                    }
                }
                else
                {
                    success = false;
                    LOGGER_ERROR("ER: SDLNet_AllocSocketSet");
                    LOGGER_ERROR(SDLNet_GetError());
                }
            }
            else
            {
                success = false;
                LOGGER_ERROR("ER: SDLNet_TCP_Open");
                LOGGER_ERROR(SDLNet_GetError());
            }
        }
        else
        {
            success = false;
            LOGGER_ERROR("ER: SDLNet_ResolveHost");
            LOGGER_ERROR(SDLNet_GetError());
        }

        if (!success)
        {
            close(networkingDetails);
        }

        return success;
    }

    // TODO: Make network/packet logic customizable by implementing application
    bool send(NetworkingDetails* networkingDetails, std::uint8_t* data, std::uint32_t length, bool isHostBigEndian)
    {
        bool success = true;

        uint8_t sendData[MAX_PACKET];

        std::uint32_t dataOffset = 0;

        const std::uint8_t bitShift0 = isHostBigEndian ? 0 : 24;
        const std::uint8_t bitShift1 = isHostBigEndian ? 8 : 16;
        const std::uint8_t bitShift2 = isHostBigEndian ? 16 : 8;
        const std::uint8_t bitShift3 = isHostBigEndian ? 24 : 0;

        sendData[0] = length >> bitShift0;
        sendData[1] = length >> bitShift1;
        sendData[2] = length >> bitShift2;
        sendData[3] = length >> bitShift3;

        std::uint8_t i = 4;
        std::uint32_t bytesLeft;

        while (dataOffset < length)
        {
            // Add 'i' to bytesLeft to account for first loop packet size header
            bytesLeft = i + length - dataOffset;

            while (i < bytesLeft && i < MAX_PACKET)
            {
                sendData[i++] = data[dataOffset++];
            }

            std::uint32_t numSent = SDLNet_TCP_Send(networkingDetails->socket, sendData, i);

            if (numSent < i)
            {
                success = false;
                LOGGER_ERROR("ER: SDLNet_TCP_Send");
                LOGGER_ERROR(SDLNet_GetError());
            }

            i = 0;
        }

        return success;
    }

    std::int16_t read(NetworkingDetails* networkingDetails, std::uint8_t* data, std::uint8_t maxToRead)
    {
        if (SDLNet_SocketReady(networkingDetails->socket))
        {
            return SDLNet_TCP_Recv(networkingDetails->socket, data, maxToRead);
        }
        else
        {
            return 0;
        }
    }

    bool close(NetworkingDetails* networkingDetails)
    {
        bool success = true;

        if (SDLNet_TCP_DelSocket(networkingDetails->socket_set, networkingDetails->socket) == -1)
        {
            success = false;
            LOGGER_ERROR("Failed to close connection");
            LOGGER_ERROR(SDLNet_GetError());
        }

        SDLNet_FreeSocketSet(networkingDetails->socket_set);
        SDLNet_TCP_Close(networkingDetails->socket);

        return success;
    }
}