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
    Data = 0,
    ConnectionRequest = 1,
    ConnectionClose = 2,
    ConnectionConfirm = 3,
    PasswordRequest = 4,
    Password = 5
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
        const bool _threadSafeEvents = true;
        funcHelper::func<void> _packetSendFunction = {[](){}};

        //* Thread Variables

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
            /// @brief called every update (at the socket update rate)
            /// @note not thread safe
            funcHelper::funcDynamic<float> _updateFunc = {[](){}};
            /// @brief called every second when the update thread is running
            /// @note not thread safe
            funcHelper::func<void> _secondUpdateFunc = {[](){}};

        // ----------------

        //* Protected Thread Functions
                            
            virtual void _thread_receive_packets(std::stop_token sToken);
            virtual void _thread_update(std::stop_token sToken);

            //* Pure Virtual Functions

                /// @brief Initializes the update and second update functions
                virtual void _initThreadFunctions() = 0;

            // -------------

        // -------------------------

        //* Packet Parsing Variables

            /// @brief Called when a data packet is received
            /// @note Optional parameter sf::Packet* (Do NOT store this packet, ONLY parse the data)
            /// @note Optional parameter sf::IpAddress, the senders IpAddress
            /// @note Optional parameter Port, the sender Port
            /// @note the packet only contains the data after the packet identifier
            funcHelper::funcDynamic3<sf::Packet*, sf::IpAddress, Port> _parseData = {[](){}};
            /// @brief Called when a connection request packet is received
            /// @note Optional parameter sf::Packet* (Do NOT store this packet, ONLY parse the data)
            /// @note Optional parameter sf::IpAddress, the senders IpAddress
            /// @note Optional parameter Port, the sender Port
            /// @note the packet only contains the data after the packet identifier
            funcHelper::funcDynamic3<sf::Packet*, sf::IpAddress, Port> _parseConnectionRequest = {[](){}};
            /// @brief Called when a connection close packet is received
            /// @note Optional parameter sf::Packet* (Do NOT store this packet, ONLY parse the data)
            /// @note Optional parameter sf::IpAddress, the senders IpAddress
            /// @note Optional parameter Port, the sender Port
            /// @note the packet only contains the data after the packet identifier
            funcHelper::funcDynamic3<sf::Packet*, sf::IpAddress, Port> _parseConnectionClose = {[](){}};
            /// @brief Called when a connection confirm packet is received
            /// @note Optional parameter sf::Packet* (Do NOT store this packet, ONLY parse the data)
            /// @note Optional parameter sf::IpAddress, the senders IpAddress
            /// @note Optional parameter Port, the sender Port
            /// @note the packet only contains the data after the packet identifier
            funcHelper::funcDynamic3<sf::Packet*, sf::IpAddress, Port> _parseConnectionConfirm = {[](){}};
            /// @brief Called when a password request packet is received
            /// @note Optional parameter sf::Packet* (Do NOT store this packet, ONLY parse the data)
            /// @note Optional parameter sf::IpAddress, the senders IpAddress
            /// @note Optional parameter Port, the sender Port
            /// @note the packet only contains the data after the packet identifier
            funcHelper::funcDynamic3<sf::Packet*, sf::IpAddress, Port> _parsePasswordRequest = {[](){}};
            /// @brief Called when a password packet is received
            /// @note Optional parameter sf::Packet* (Do NOT store this packet, ONLY parse the data)
            /// @note Optional parameter sf::IpAddress, the senders IpAddress
            /// @note Optional parameter Port, the sender Port
            /// @note the packet only contains the data after the packet identifier
            funcHelper::funcDynamic3<sf::Packet*, sf::IpAddress, Port> _parsePassword = {[](){}};

        // -----------------------

        //* Packet Parsing Functions

            //* Pure Virtual Functions

                /// @brief initializes the packet parsing functions
                virtual void _initPacketParsingFunctions() = 0;

            // -------------

        // -------------------------

    public:

        //* Events
            
            /// @brief Invoked when data is received
            /// @note Optional parameter sf::Packet
            EventHelper::EventDynamic<sf::Packet> onDataReceived;
            /// @brief Invoked when the update rate is changed
            /// @note Optional parameter unsigned int
            EventHelper::EventDynamic<unsigned int> onUpdateRateChanged;
            /// @brief Invoked when the client timeout time has been changed
            /// @note Optional parameter unsigned int
            EventHelper::EventDynamic<unsigned int> onClientTimeoutChanged;
            /// @brief Invoked when this port is changed 
            /// @note Optional parameter Port (unsigned short)
            EventHelper::EventDynamic<Port> onPortChanged; // TODO implement this
            /// @brief Invoked when the server port is changed 
            /// @note Optional parameter Port (unsigned short)
            EventHelper::EventDynamic<Port> onServerPortChanged; // TODO implement this
            /// @brief Invoked when the password is changed
            /// @note Optional parameter New Password (string)
            EventHelper::EventDynamic<std::string> onPasswordChanged;
            /// @brief Invoked when the packet send function is changed
            EventHelper::Event onPacketSendChanged;
            /// @note Server -> Open
            /// @note Client -> Connection Confirmed
            EventHelper::Event onConnectionOpen;
            /// @note Server -> Closed
            /// @note Client -> Disconnected
            EventHelper::Event onConnectionClose;

        // ------

        //* Initializer and Deconstructor

            SocketPlus();
            ~SocketPlus();

        // ------------------------------

        //* Public Thread Functions

            void startThreads();
            void stopThreads();

        // ---------------------

        //* Getters

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

        //* Boolean Question Functions

            // // TODO do these better
            // /// @returns true if the given packet is a connection request **Does NOT change ANYTHING about the packet
            // static bool isConnectionRequest(sf::Packet packet);
            // /// @returns true if the given packet is a connection close **Does NOT change ANYTHING about the packet
            // static bool isConnectionClose(sf::Packet packet);
            // /// @returns true if the given packet is a data packet **Does NOT change ANYTHING about the packet
            // static bool isData(sf::Packet packet);
            // /// @returns true if the given packet is a connection confirmation **Done NOT change ANYTHING about the packet
            // static bool isConnectionConfirm(sf::Packet packet);
            // /// @returns true if the given packet is a password request **Done NOT change ANYTHING about the packet
            // static bool isPasswordRequest(sf::Packet packet);
            // /// @returns true if the given packet is a password **Done NOT change ANYTHING about the packet
            // static bool isPassword(sf::Packet packet);
            // /// @returns true if the given packet is informing of a wrong packet **Done NOT change ANYTHING about the packet
            // static bool isWrongPassword(sf::Packet packet);
            // // ---------------------

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
            // static sf::Packet WrongPasswordPacket();

        // -------------------
};

#endif // SOCKETBASE_H
