#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#pragma once

#include "SocketPlus.hpp"

// TODO make a function that resets all value to default state

// TODO Update all names for camel case

// TODO make the password requirement able to be changed while the server is open

class Server : public SocketPlus
{
    private:  

        //* Client variables for the server

            // first value is for the ID and the second is for the client data
            std::unordered_map<ID, ClientData> _clientData;
            // ID _lastID = 0;

        // ------------

        //* Thread Functions

            //* Pure Virtual Definitions

                virtual void _initThreadFunctions();

            // ------------------------

            void _update(const float& deltaTime);
            /// @brief the function to be called every second in update
            void _secondUpdate();

        // -----------------

        //* Packet Parsing Functions

            //* Pure Virtual Definitions

                /// @brief initializes the packet parsing functions
                virtual void _initPacketParsingFunctions();

            // -------------

            void _parseDataPacket(sf::Packet* packet, sf::IpAddress senderIP, Port senderPort);
            void _parseConnectionRequestPacket(sf::Packet* packet, sf::IpAddress senderIP, Port senderPort);
            void _parseConnectionClosePacket(sf::Packet* packet, sf::IpAddress senderIP);
            void _parsePasswordPacket(sf::Packet* packet, sf::IpAddress senderIP, Port senderPort);

        // -------------------------

    public:
        //* Public data that the main will need access to 
            
            std::deque<ID> deletedClientIDs; // TODO remove this once 
            std::deque<ID> newClientIDs; // TODO remove this once 

            //* Events

                EventHelper::EventDynamic<ID> onClientConnected;
                EventHelper::EventDynamic<ID> onClientDisconnected;

            // -------
        
        // ----------------------------------------------

        // TODO organize this
        Server(Port port, bool passwordRequired = false);
        ~Server();
        void RequirePassword(bool requirePassword);
        void RequirePassword(bool requirePassword, std::string password);
        // returns true if server was started with the port that was set 
        // Also handles the thread if successful start or failed start
        bool openServer();
        // resets all the data stored in the server
        void CloseServer();
        void setPort(unsigned short port);
        // Sends the given packet to every client currently connected
        void SendToAll(sf::Packet& packet);
        // Tries to send the given packet to the client with the given ID
        void SendTo(sf::Packet& packet, ID id);
        void disconnectAllClients();
        // removes the client with the given ID
        // returns true if the client was removed returns false if it was not found
        bool disconnectClient(ID id);
        // returns a pointer to the clients map
        std::unordered_map<ID, ClientData>& getClients();
        static ClientData getClientData(std::pair<const ID, ClientData> client);
        sf::Uint32 NumberOfClients();
        float getClientTimeSinceLastPacket(ID clientID);
        double getClientConnectionTime(ID clientID);
        unsigned int getClientPacketsPerSec(ID clientID);
};

#endif
