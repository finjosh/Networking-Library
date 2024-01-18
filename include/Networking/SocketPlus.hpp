#ifndef SOCKETBASE_H
#define SOCKETBASE_H

#pragma once

#include <stdexcept>
#include <iostream>
#include <thread>
#include <list>
#include <set>
#include <map>

#include <SFML\Network.hpp>

#include "ClientData.hpp"
#include "include/Utils/funcHelper.hpp"
#include "include/Utils/EventHelper.hpp"
#include "include/Utils/UpdateLimiter.hpp"

// ID = Uint32
typedef sf::Uint32 ID;
// IP = ID (Uint32)
typedef ID IP;
typedef unsigned short Port;

enum PacketType
{
    ConnectionRequest = 0,
    ConnectionClose = 1,
    Data = 2,
    ConnectionConfirm = 3,
    RequestPassword = 4,
    Password = 5,
    WrongPassword = 6
};

// TODO make a function to handle packets here (do it with a switch case)
class SocketPlus : protected sf::UdpSocket
{
    protected:

        IP _ip = 0;
        bool _needsPassword = false;
        std::string _password = "";
        unsigned short _port = 777; // TODO reimplement _port
        unsigned short _serverPort = 777; // TODO reimplement _serverPort
        bool _connectionOpen = false; // if the server is open or the client is connected
        double _connectionTime = 0.0; // time that the connection has been up
        int _clientTimeoutTime = 20; 
        bool _threadSafeEvents = true;
        funcHelper::func<void> _packetSendFunction = {[](){}};

        //* thread variables

            // stop source is universal
            std::stop_source* _sSource = nullptr;
            // receiving thread
            std::jthread* _receive_thread = nullptr;
            // sending/updating thread
            std::jthread* _update_thread = nullptr;
            // if we should be sending packets in the thread
            bool _sendingPackets = true;
            // in updates/second
            unsigned int _socketUpdateRate = 64;
            /// @brief called every update
            /// @note not thread safe
            funcHelper::funcDynamic<float> _update = {[](){}};
            /// @brief called every second in when the update thread is running
            /// @note not thread safe
            funcHelper::func<void> _secondUpdate = {[](){}};

        // ----------------

        //* Protected thread functions
                            
            virtual void thread_receive_packets(std::stop_token sToken) = 0;
            virtual void thread_update(std::stop_token sToken);

            //* Pure virtual

                virtual void initThreadFunctions() = 0;

            // -------------

        // -------------------------

    public:

        //* Event stuff
            
            /// @brief invoked when data is received
            /// @note optional parameter sf::Packet
            EventHelper::EventDynamic<sf::Packet> onDataReceived;
            /// @brief invoked when the update rate is changed
            /// @note optional parameter unsigned int
            EventHelper::EventDynamic<unsigned int> onUpdateRateChanged;
            /// @brief invoked when the client timeout time has been changed
            /// @note optional parameter unsigned int
            EventHelper::EventDynamic<unsigned int> onClientTimeoutChanged;
            /// @brief invoked when this port is changed 
            /// @note optional parameter Port (unsigned short)
            EventHelper::EventDynamic<Port> onPortChanged; // TODO implement this
            /// @brief invoked when the server port is changed 
            /// @note optional parameter Port (unsigned short)
            EventHelper::EventDynamic<Port> onServerPortChanged; // TODO implement this
            /// @brief invoked when the password is changed
            /// @note optional parameter New Password (string)
            EventHelper::EventDynamic<std::string> onPasswordChanged;
            /// @brief invoked when the packet send function is changed
            EventHelper::Event onPacketSendChanged;
            /// @note Server -> Open
            /// @note Client -> Connection Confirmed
            EventHelper::Event onConnectionOpen;
            /// @note Server -> Closed
            /// @note Client -> Disconnected
            EventHelper::Event onConnectionClose;

            // /// @brief will determine if events are called in thread safe mode
            // /// @param threadSafe Default true
            // void setThreadSafeEvents(bool threadSafe = true);
            // bool isThreadSafeEvents();

        // ------

        //* initializer and deconstructor

            SocketPlus();
            ~SocketPlus();

        // ------------------------------

        //* Public thread functions

            void startThreads();
            void stopThreads();

        // ---------------------

        //* Getter

            /// @returns ID
            ID getID();
            /// @returns IP as IPAddress
            sf::IpAddress getIP();
            /// @returns Local IP as IPAddress
            sf::IpAddress getLocalIP();
            /// @returns IP as integer
            IP getIP_I();
            /// @returns the time in seconds
            double getConnectionTime();
            /// @returns the time in seconds
            double getOpenTime();
            /// @returns the update interval in updates per second
            unsigned int getUpdateInterval();
            /// @returns this port
            unsigned int getPort();
            /// @returns current client timeout time in seconds
            int getClientTimeout();
            /// @returns the current password
            std::string getPassword();
            /// @returns the function that is called when sending a packet
            const funcHelper::func<void>& getPacketSendFunction();

        // -------

        //* Setters

            /// @brief sets the update interval in updates/second 
            /// @note DEFAULT = 64 (64 updates/second)
            void setUpdateInterval(unsigned int interval);
            /// @returns true if packets are being sent at the interval that was set
            void sendingPackets(bool sendPackets);
            /// @brief sets this password
            /// @note if this derived class is the server, sets the server password, else sets the password that will be sent to server
            void setPassword(std::string password);
            /// @brief sets the time for a client to timeout if no packets are sent (seconds)
            void setClientTimeout(const int& timeout);
            /// @brief sets the sending packet function
            /// @note if the socket connection is open then you cannot set the function
            /// @returns true if the function was set
            bool setPacketSendFunction(funcHelper::func<void> packetSendFunction = {[](){}});

        // --------

        //* Boolean question Functions

            /// @returns true if the given packet is a connection request **Does NOT change ANYTHING about the packet
            virtual bool isConnectionRequest(sf::Packet packet);
            /// @returns true if the given packet is a connection close **Does NOT change ANYTHING about the packet
            virtual bool isConnectionClose(sf::Packet packet);
            /// @returns true if the given packet is a data packet **Does NOT change ANYTHING about the packet
            virtual bool isData(sf::Packet packet);
            /// @returns true if the given packet is a connection confirmation **Done NOT change ANYTHING about the packet
            virtual bool isConnectionConfirm(sf::Packet packet);
            /// @returns true if the given packet is a password request **Done NOT change ANYTHING about the packet
            virtual bool isPasswordRequest(sf::Packet packet);
            /// @returns true if the given packet is a password **Done NOT change ANYTHING about the packet
            virtual bool isPassword(sf::Packet packet);
            /// @returns true if the given packet is informing of a wrong packet **Done NOT change ANYTHING about the packet
            virtual bool isWrongPassword(sf::Packet packet);
            /// @returns true if the client is connected or server is open
            bool isConnectionOpen();
            /// @brief if the receiving thread is running
            /// @returns true
            bool isReceivingPackets();
            /// @brief if this is sending packets
            bool isSendingPackets();
            /// @brief if this needs a password
            bool NeedsPassword();
            /// @brief Checks if the given ipAddress is valid
            /// @note if it is invalid program will freeze for a few seconds
            static bool isValidIpAddress(sf::IpAddress ipAddress);
            /// @brief Checks if the given ipAddress is valid
            /// @note if it is invalid program will freeze for a few seconds
            static bool isValidIpAddress(sf::Uint32 ipAddress);
            /// @brief Checks if the given ipAddress is valid
            /// @note if it is invalid program will freeze for a few seconds
            static bool isValidIpAddress(std::string ipAddress);

        // ---------------------------

        //* Template Functions

            static sf::Packet ConnectionCloseTemplate();
            static sf::Packet ConnectionRequestTemplate();
            static sf::Packet DataPacketTemplate();
            static sf::Packet ConnectionConfirmPacket(sf::Uint32 id);
            static sf::Packet PasswordRequestPacket();
            static sf::Packet PasswordPacket(std::string password);
            static sf::Packet WrongPasswordPacket();

        // -------------------
};

#endif // SOCKETBASE_H
