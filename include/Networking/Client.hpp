#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#pragma once

#include "SocketPlus.hpp"

// TODO make a function that resets all value to default state

class Client : public SocketPlus
{
private:

    //* Client Variables

        sf::IpAddress _serverIP;
        /// @brief unsigned short _serverPort;
        bool _wrongPassword = false;
        /// @brief Time since last packet from server
        float _timeSinceLastPacket = 0.0;

    // -----------------

    //* Thread Functions

        //* Pure Virtual Definitions
    
            virtual void _initThreadFunctions();
        
        // ------------------------

        void _update(const float& deltaTime);

    // -----------------

    //* Protected Connection Functions
    
        /// @brief use only when connection is closed
        virtual void _resetConnectionData();
    
    // ---------------------

    //* Packet Parsing Functions

        //* Pure Virtual Definition

            /// @brief initializes the packet parsing functions
            virtual void _initPacketParsingFunctions();

        // -------------

        void _parseDataPacket(sf::Packet* packet);
        void _parseConnectionClosePacket(sf::Packet* packet);
        void _parseConnectionConfirmPacket(sf::Packet* packet);
        void _parsePasswordRequestPacket(sf::Packet* packet);
        void _parseWrongPasswordPacket(sf::Packet* packet);

    // -------------------------

public:

    //* Events

        EventHelper::Event onPasswordRequest;

    // -------

    //* Connection Functions
        
        //* Pure Virtual Definitions

            /// @brief attempts to connect to the server with the current server data
            /// @returns true for successful send of connection attempt (DOES NOT MEAN THERE IS A CONNECTION CONFIRMATION)
            virtual bool tryOpenConnection();
            /// @brief closes the connection to the server
            virtual void closeConnection();
        
        // -------------------------

    // ------------------------------------------------

    // TODO organize this
    Client(sf::IpAddress serverIP, Port serverPort);
    Client(unsigned short serverPort);
    ~Client();
    /// @brief is true until another password is sent
    /// @note password status is unknown until this is true or connection is open
    /// @return true is wrong password
    bool wasIncorrectPassword();
    void setAndSendPassword(std::string password);
    void sendPasswordToServer();
    void setServerData(sf::IpAddress serverIP, Port serverPort);
    void setServerData(sf::IpAddress serverIP);
    void setServerData(Port port);
    /// @brief sends the packet to the server
    void sendToServer(sf::Packet& packet);
    /// @brief returns the time in seconds
    float getTimeSinceLastPacket();
    sf::IpAddress getServerIP();
    unsigned int getServerPort();
};

#endif
