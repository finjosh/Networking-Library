#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#pragma once

#include "SocketPlus.hpp"

class Server : public SocketPlus
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

        void _parseDataPacket(sf::Packet* packet, sf::IpAddress senderIP, PORT senderPort);
        void _parseConnectionRequestPacket(sf::Packet* packet, sf::IpAddress senderIP, PORT senderPort);
        void _parseConnectionClosePacket(sf::Packet* packet, sf::IpAddress senderIP);
        void _parsePasswordPacket(sf::Packet* packet, sf::IpAddress senderIP, PORT senderPort);

    // -------------------------

public:

    //* Initializer and Deconstructor

        Server(PORT port, bool passwordRequired = false);
        ~Server();

    // ------------------------------

    //* Events

        EventHelper::EventDynamic<ID> onClientConnected;
        EventHelper::EventDynamic<ID> onClientDisconnected;

    // -------

    //* Connection Functions

        void requirePassword(bool requirePassword);
        void requirePassword(bool requirePassword, std::string password);
        void setPort(PORT port);
        void disconnectAllClients();
        /// @brief removes the client with the given ID
        /// @returns true if the client was removed returns false if it was not found
        bool disconnectClient(ID id);
        /// @returns a pointer to the clients map
        const std::unordered_map<ID, ClientData>& getClients() const;
        static ClientData getClientData(std::pair<const ID, ClientData> client);
        /// @returns the number of clients
        sf::Uint32 getClientsSize();
        float getClientTimeSinceLastPacket(ID clientID);
        double getClientConnectionTime(ID clientID);
        unsigned int getClientPacketsPerSec(ID clientID);
        /// @brief Sends the given packet to every client currently connected
        void sendToAll(sf::Packet& packet);
        /// @brief tries to send the given packet to the client with the given ID
        void sendTo(sf::Packet& packet, ID id);

        //* Pure Virtual Definitions
            
            /// @returns true if server was started with the port that was previously set
            virtual bool tryOpenConnection();
            /// @brief closes the server and disconnects all clients
            virtual void closeConnection();

        // -------------------------

    // ------------------------------------------------
};

#endif
