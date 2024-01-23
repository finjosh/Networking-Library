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

namespace udp
{

// ID = Uint32
typedef sf::Uint32 ID;
// IP = ID (Uint32)
typedef ID IP;
typedef unsigned short PORT;

enum PacketType
{
    Data = 0,
    ConnectionRequest = 1,
    ConnectionClose = 2,
    ConnectionConfirm = 3,
    PasswordRequest = 4,
    Password = 5
};

class Socket : protected sf::UdpSocket
{
protected:

    //* Connection Data
    
        IP _ip = 0;
        bool _needsPassword = false;
        std::string _password = "";
        unsigned short _port = 777;
        /// @brief if the server is open or the client is connected
        bool _connectionOpen = false;
        /// @brief time that the connection has been up
        double _connectionTime = 0.f;
        int _clientTimeoutTime = 20; 
        funcHelper::func<void> _packetSendFunction = {[](){}};

    // ----------------

    //* Thread Variables

        const bool _threadSafeEvents = true;
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

    //* Protected Connection Functions

        /// @brief use only when connection is closed
        virtual void _resetConnectionData();
    
    // ---------------------

    //* Packet Parsing

        /// @brief Called when a data packet is received
        /// @note Optional parameter sf::Packet* (Do NOT store this packet, ONLY parse the data)
        /// @note Optional parameter sf::IpAddress, the senders IpAddress
        /// @note Optional parameter Port, the sender Port
        /// @note the packet only contains the data after the packet identifier
        funcHelper::funcDynamic3<sf::Packet*, sf::IpAddress, PORT> _parseData = {[](){}};
        /// @brief Called when a connection request packet is received
        /// @note Optional parameter sf::Packet* (Do NOT store this packet, ONLY parse the data)
        /// @note Optional parameter sf::IpAddress, the senders IpAddress
        /// @note Optional parameter Port, the sender Port
        /// @note the packet only contains the data after the packet identifier
        funcHelper::funcDynamic3<sf::Packet*, sf::IpAddress, PORT> _parseConnectionRequest = {[](){}};
        /// @brief Called when a connection close packet is received
        /// @note Optional parameter sf::Packet* (Do NOT store this packet, ONLY parse the data)
        /// @note Optional parameter sf::IpAddress, the senders IpAddress
        /// @note Optional parameter Port, the sender Port
        /// @note the packet only contains the data after the packet identifier
        funcHelper::funcDynamic3<sf::Packet*, sf::IpAddress, PORT> _parseConnectionClose = {[](){}};
        /// @brief Called when a connection confirm packet is received
        /// @note Optional parameter sf::Packet* (Do NOT store this packet, ONLY parse the data)
        /// @note Optional parameter sf::IpAddress, the senders IpAddress
        /// @note Optional parameter Port, the sender Port
        /// @note the packet only contains the data after the packet identifier
        funcHelper::funcDynamic3<sf::Packet*, sf::IpAddress, PORT> _parseConnectionConfirm = {[](){}};
        /// @brief Called when a password request packet is received
        /// @note Optional parameter sf::Packet* (Do NOT store this packet, ONLY parse the data)
        /// @note Optional parameter sf::IpAddress, the senders IpAddress
        /// @note Optional parameter Port, the sender Port
        /// @note the packet only contains the data after the packet identifier
        funcHelper::funcDynamic3<sf::Packet*, sf::IpAddress, PORT> _parsePasswordRequest = {[](){}};
        /// @brief Called when a password packet is received
        /// @note Optional parameter sf::Packet* (Do NOT store this packet, ONLY parse the data)
        /// @note Optional parameter sf::IpAddress, the senders IpAddress
        /// @note Optional parameter Port, the sender Port
        /// @note the packet only contains the data after the packet identifier
        funcHelper::funcDynamic3<sf::Packet*, sf::IpAddress, PORT> _parsePassword = {[](){}};

        //* Pure Virtual Functions

            /// @brief initializes the packet parsing functions
            virtual void _initPacketParsingFunctions() = 0;

        // -------------

    // -------------------------

    //* Socket Functions

        /// @brief attempts to send a packet to the given ip and port
        /// @note if the packet fails to send throws runtime error
        void _sendTo(sf::Packet& packet, const sf::IpAddress& ip, const PORT& port);

    // -----------------

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
        EventHelper::EventDynamic<PORT> onPortChanged;
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

        Socket();
        ~Socket();

    // ------------------------------

    //* Public Thread Functions

        void startThreads();
        void stopThreads();

    // ---------------------

    //* Connection Functions

        //* Pure Virtual Functions

            virtual bool tryOpenConnection() = 0;
            virtual void closeConnection() = 0;

        // -----------------------

    // ---------------------

    //* Getters

        /// @returns ID
        ID getID() const;
        /// @returns IP as IPAddress
        sf::IpAddress getIP() const;
        /// @returns Local IP as IPAddress
        sf::IpAddress getLocalIP() const;
        /// @returns IP as integer
        IP getIP_I() const;
        /// @returns the time in seconds
        double getConnectionTime() const;
        /// @returns the time in seconds
        double getOpenTime() const;
        /// @returns the update interval in updates per second
        unsigned int getUpdateInterval() const;
        /// @returns this port
        unsigned int getPort() const;
        /// @returns current client timeout time in seconds
        int getClientTimeout() const;
        /// @returns the current password
        std::string getPassword() const;
        /// @returns the function that is called when sending a packet
        const funcHelper::func<void>& getPacketSendFunction() const;

    // -------

    //* Setters

        /// @brief sets the update interval in updates/second 
        /// @note DEFAULT = 64 (64 updates/second)
        void setUpdateInterval(const unsigned int& interval);
        /// @returns true if packets are being sent at the interval that was set
        void sendingPackets(const bool& sendPackets);
        /// @brief sets this password
        /// @note if this derived class is the server, sets the server password, else sets the password that will be sent to server
        void setPassword(const std::string& password);
        /// @brief sets the time for a client to timeout if no packets are sent (seconds)
        void setClientTimeout(const int& timeout);
        /// @brief sets the sending packet function
        /// @note if the socket connection is open then you cannot set the function
        /// @returns true if the function was set
        bool setPacketSendFunction(const funcHelper::func<void>& packetSendFunction = {[](){}});
        void setPort(const PORT& port);

    // --------

    //* Boolean Question Functions

        /// @returns true if the client is connected or server is open
        bool isConnectionOpen() const;
        /// @brief if the receiving thread is running
        /// @returns true
        bool isReceivingPackets() const;
        /// @brief if this is sending packets
        bool isSendingPackets() const;
        /// @brief if this needs a password
        bool NeedsPassword() const;
        /// @brief Checks if the given ipAddress is valid
        /// @note if it is invalid program will freeze for a few seconds
        static bool isValidIpAddress(const  sf::IpAddress& ipAddress);
        /// @brief Checks if the given ipAddress is valid
        /// @note if it is invalid program will freeze for a few seconds
        static bool isValidIpAddress(const  sf::Uint32& ipAddress);
        /// @brief Checks if the given ipAddress is valid
        /// @note if it is invalid program will freeze for a few seconds
        static bool isValidIpAddress(const  std::string& ipAddress);

    // ---------------------------

    //* Template Functions

        static sf::Packet ConnectionCloseTemplate();
        static sf::Packet ConnectionRequestTemplate();
        static sf::Packet DataPacketTemplate();
        static sf::Packet ConnectionConfirmPacket(const sf::Uint32& id);
        static sf::Packet PasswordRequestPacket();
        static sf::Packet PasswordPacket(const std::string& password);

    // -------------------
};

}

#endif // SOCKETBASE_H
