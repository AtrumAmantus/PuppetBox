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

    bool send(NetworkingDetails* networkingDetails, std::uint8_t* data, std::uint32_t length)
    {
        bool success = true;

        uint8_t sendData[MAX_PACKET];

        std::uint32_t offset = 0;

        sendData[0] = length >> 24;
        sendData[1] = length >> 16;
        sendData[2] = length >> 8;
        sendData[3] = length;

        std::uint8_t i = 4;

        while (offset < length)
        {
            std::uint32_t bytesLeft = length - offset;

            while (i < bytesLeft && i < MAX_PACKET)
            {
                sendData[i++] = data[offset++];
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

        if (i > 0)
        {
            std::uint32_t numSent = SDLNet_TCP_Send(networkingDetails->socket, sendData, i);

            if (numSent < i)
            {
                success = false;
                LOGGER_ERROR("ER: SDLNet_TCP_Send");
                LOGGER_ERROR(SDLNet_GetError());
            }
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