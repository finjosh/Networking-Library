#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#pragma once

#include "Socket.hpp"

namespace udp
{

class Server : public Socket
{
private:  

    //* Server Variables

        /// @brief first value is for the ID and the second is for the client data
        std::unordered_map<ID, ClientData> _clientData;

    // ------------

    //* Thread Functions

        //* Pure Virtual Definitions

            virtual void _initThreadFunctions();

        // ------------------------

        void _update(const float& deltaTime);
        /// @brief the function to be called every second in update
        void _secondUpdate();

    // -----------------

    //* Protected Connection Functions
    
        /// @brief use only when connection is closed
        virtual void _resetConnectionData();
    
    // ---------------------

    //* Packet Parsing Functions

        //* Pure Virtual Definitions

            /// @brief initializes the packet parsing functions
            virtual void _initPacketParsingFunctions();

        // -------------

        void _parseDataPacket(sf::Packet* packet, const sf::IpAddress& senderIP, const PORT& senderPort);
        void _parseConnectionRequestPacket(sf::Packet* packet, const sf::IpAddress& senderIP, const PORT& senderPort);
        void _parseConnectionClosePacket(sf::Packet* packet, const sf::IpAddress& senderIP);
        void _parsePasswordPacket(sf::Packet* packet, const sf::IpAddress& senderIP, const PORT& senderPort);

    // -------------------------

public:

    //* Initializer and Deconstructor

        Server(const PORT& port, const bool& passwordRequired = false);
        ~Server();

    // ------------------------------

    //* Events

        EventHelper::EventDynamic<ID> onClientConnected;
        EventHelper::EventDynamic<ID> onClientDisconnected;

    // -------

    //* Connection Functions

        void setPasswordRequired(const bool& requirePassword);
        void setPasswordRequired(const bool& requirePassword, const std::string& password);
        bool isPasswordRequired() const;
        void disconnectAllClients();
        /// @brief removes the client with the given ID
        /// @returns true if the client was removed returns false if it was not found
        bool disconnectClient(const ID& id);
        /// @returns a pointer to the clients map
        const std::unordered_map<ID, ClientData>& getClients() const;
        static ClientData getClientData(const std::pair<const ID, ClientData>& client);
        /// @returns the number of clients
        sf::Uint32 getClientsSize() const;
        float getClientTimeSinceLastPacket(const ID& clientID) const;
        double getClientConnectionTime(const ID& clientID) const;
        unsigned int getClientPacketsPerSec(const ID& clientID) const;
        /// @brief Sends the given packet to every client currently connected
        void sendToAll(sf::Packet& packet);
        /// @brief tries to send the given packet to the client with the given ID
        void sendTo(sf::Packet& packet, const ID& id);

        //* Pure Virtual Definitions
            
            /// @returns true if server was started with the port that was previously set
            virtual bool tryOpenConnection();
            /// @brief closes the server and disconnects all clients
            virtual void closeConnection();

        // -------------------------

    // ------------------------------------------------
};

}

#endif
