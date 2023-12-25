#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#pragma once

#include "SocketPlus.hpp"

class Client : public SocketPlus
{
    private:

        //* Server variables

            sf::IpAddress _serverIP;
            unsigned short _serverPort;
            bool _wrongPassword = false;
            // Time since last packet from server
            float _timeSinceLastPacket = 0.0;

        // -----------------

        //* Thread Functions

            //* Pure Virtual Definition
        
                virtual void thread_receive_packets(std::stop_token stoken);
                virtual void thread_update(std::stop_token stoken, funcHelper::func<void> customPacketSendFunctions);
            
            // ------------------------

            void StopThreads();

        // -----------------

    public:

        //* Events

            EventHelper::Event onWrongPassword;
            EventHelper::Event onPasswordRequest;
            EventHelper::Event onConnectionConfirmed;
            EventHelper::Event onConnectionClosed;

        // -------

        Client(sf::IpAddress serverIP, unsigned short serverPort);
        Client(unsigned short serverPort);
        ~Client();
        // CAN only be used once per wrong password sent as the bool is reset after call
        // if the password is not wrong that mean that the connection has started 
        // OR
        // the server has not responded yet
        bool WasIncorrectPassword();
        void setAndSendPassword(std::string password);
        void sendPasswordToServer();
        // attempts to connect to the given server while handling the thread through fail and successful
        // true for successful send of connection attempt (DOES NOT MEAN THERE IS A CONNECTION CONFIRMATION)
        bool ConnectToServer(funcHelper::func<void> customPacketSendFunction);
        void setServerData(sf::IpAddress serverIP, unsigned short serverPort);
        void setServerData(sf::IpAddress serverIP);
        // sends the packet to the server
        void SendToServer(sf::Packet& packet);
        // returns true if the given packet is a data packet 
        // - removes the packet type
        virtual bool isData(sf::Packet& packet);
        // returns true if the given packet is a connection close 
        // - removes the ID
        // - sets isConnected to false
        virtual bool isConnectionClose(sf::Packet packet);
        // returns true if the given packet is a connection confirmation
        // - resets ID to the given one if the ID is different
        virtual bool isConnectionConfirm(sf::Packet& packet);
        // returns true if the given packet is a indication that the wrong password was given
        // - sets the "wrongPassword" to true
        virtual bool isWrongPassword(sf::Packet& packet);
        // returns true if the given packet is a password request
        // - sets "serverNeedsPassword" to true
        virtual bool isPasswordRequest(sf::Packet& packet);
        // resets all the data stored in the client
        void Disconnect();
        // returns the time in seconds
        float getTimeSinceLastPacket();
        sf::IpAddress getServerIP();
        unsigned int getServerPort();
};

#endif
