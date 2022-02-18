#pragma once

#include <string>

#include <sdl2/SDL_net.h>

#define MAX_PACKET 0xFF

namespace PB::Networking
{
    struct NetworkingDetails
    {
        /** The specific socket for this connection */
        TCPsocket socket;
        /** The socket set to associate the socket with */
        SDLNet_SocketSet socket_set;
        /** Network form of the resolved host IP and port */
        IPaddress ip;
        bool isConnected = false;
    };

    /**
     * \brief Connects to a remote host.
     *
     * \param networkingDetails The details associated with this connection
     * \param host The host to connect to.
     * \param port The port to connect to on the remote host.
     * \return True if the connection was made successfully, False otherwise.
     */
    bool connect(NetworkingDetails* networkingDetails, std::string host, std::uint16_t port);

    bool send(NetworkingDetails* networkingDetails, std::uint8_t* data, std::uint32_t length, bool isHostBigEndian);

    std::int16_t read(NetworkingDetails* networkingDetails, std::uint8_t* data, std::uint8_t maxToRead);

    bool close(NetworkingDetails* networkingDetails);
}