#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#pragma once

#include "SocketPlus.hpp"

// TODO make a function that resets all value to default state
// TODO update functions to camel case

class Client : public SocketPlus
{
    private:

        //* Client Variables

            sf::IpAddress _serverIP;
            // unsigned short _serverPort;
            bool _wrongPassword = false;
            // Time since last packet from server
            float _timeSinceLastPacket = 0.0;

        // -----------------

        //* Thread Functions

            //* Pure Virtual Definitions
        
                virtual void _initThreadFunctions();
            
            // ------------------------

            void _update(const float& deltaTime);

        // -----------------

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

        // TODO organize this
        Client(sf::IpAddress serverIP, Port serverPort);
        Client(unsigned short serverPort);
        ~Client();
        /// @brief is true until another password is sent
        /// @note password status is unknown until this is true or connection is open
        /// @return true is wrong password
        bool WasIncorrectPassword();
        void setAndSendPassword(std::string password);
        void sendPasswordToServer(); // TODO put this into connect to server
        // attempts to connect to the given server while handling the thread through fail and successful
        // true for successful send of connection attempt (DOES NOT MEAN THERE IS A CONNECTION CONFIRMATION)
        bool ConnectToServer(); // TODO rename to open connection
        void setServerData(sf::IpAddress serverIP, Port serverPort);
        void setServerData(sf::IpAddress serverIP);
        void setServerData(Port port);
        // sends the packet to the server
        void SendToServer(sf::Packet& packet);
        // resets all the data stored in the client
        void Disconnect();
        // returns the time in seconds
        float getTimeSinceLastPacket();
        sf::IpAddress getServerIP();
        unsigned int getServerPort();
};

#endif
