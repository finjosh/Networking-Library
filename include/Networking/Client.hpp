#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#pragma once

#include "Socket.hpp"

namespace udp

{

class Client : public Socket
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

    //* Initializer and Deconstructor

        Client(const sf::IpAddress& serverIP, const PORT& serverPort);
        Client(const PORT& serverPort);
        ~Client();

    // ------------------------------

    //* Events

        /// @brief Called when ever password is requested
        /// @note password is requested when wrong password is sent
        EventHelper::Event onPasswordRequest;

    // -------

    //* Connection Functions
        
        /// @brief is true until another password is sent
        /// @note password status is unknown until this is true or connection is open
        /// @return true is wrong password
        bool wasIncorrectPassword();
        void setAndSendPassword(const std::string& password);
        void sendPasswordToServer();
        void setServerData(const sf::IpAddress& serverIP, const PORT& serverPort);
        void setServerData(const sf::IpAddress& serverIP);
        void setServerData(const PORT& port);
        /// @brief sends the packet to the server
        void sendToServer(sf::Packet& packet);
        /// @brief returns the time in seconds
        float getTimeSinceLastPacket() const;
        sf::IpAddress getServerIP() const;
        unsigned int getServerPort() const;

        //* Pure Virtual Definitions

            /// @brief attempts to connect to the server with the current server data
            /// @returns true for successful send of connection attempt (DOES NOT MEAN THERE IS A CONNECTION CONFIRMATION)
            virtual bool tryOpenConnection();
            /// @brief closes the connection to the server
            virtual void closeConnection();
        
        // -------------------------

    // ------------------------------------------------

};

}

#endif
