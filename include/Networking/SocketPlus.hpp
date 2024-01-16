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

// struct DataPacket
// {
//     inline DataPacket(sf::Packet packet)
//     { this->packet = packet; editing = false; }
//     DataPacket() = default;
//     inline ~DataPacket()
//     {
//         // bool endPacket = packet.endOfPacket();
//         // int dataSize = packet.getDataSize();
//         // TODO packet crashing??
//         // check if the packet is not being destroyed properly
//         this->packet.clear();
//     }
//     sf::Packet packet;
//     bool editing = true;
// };

// TODO make a function to handle packets here (do it with a switch case)
// if when sending packets from host to client about enemies creates errors the issue would be that the enemy is being deleted
// while the packet sending thread is trying to read from that enemy
class SocketPlus : protected sf::UdpSocket
{
    protected:

        int _clientTimeoutTime = 20; 
        IP _ip = 0;
        bool _needsPassword = false;
        std::string _password = "";
        unsigned short _port = 777;
        unsigned short _serverPort = 777;
        bool _connectionOpen = false; // if the server is open or the client is connected
        double _connectionTime = 0.0; // time that the connection has been up
        bool _threadSafeEvents = true;
        funcHelper::func<void> customPacketSendFunction;

        //* thread variables

            // stop source is universal
            std::stop_source* _sSource = nullptr;
            // receiving thread
            std::jthread* _receive_thread = nullptr;
            // sending/updating thread
            std::jthread* _update_thread = nullptr;
            bool _sendingPackets = true;
            // in updates/second
            unsigned int _socketUpdateRate = 64;

        // ----------------

        //* Protected thread functions

            //* PURE virtual functions
                
                // although pure virtual functions increase the time it takes for the function to be called it would be so minimal
                // it does not matter for this project
                
                virtual void thread_receive_packets(std::stop_token sToken) = 0;
                virtual void thread_update(std::stop_token sToken, funcHelper::func<void> customPacketSendFunction) = 0;

            // -----------------------

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
            /// @note Server -> Open
            /// @note Client -> Connection Confirmed
            EventHelper::Event onConnectionOpen;
            /// @note Server -> Closed
            /// @note Client -> Disconnected
            EventHelper::Event onConnectionClose;

            /// @brief will determine if events are called in thread safe mode
            /// @param threadSafe Default true
            void setThreadSafeEvents(bool threadSafe = true);
            bool isThreadSafeEvents();

        // ------

        //* Public data vars for ease of access
        
            // std::list<DataPacket> DataPackets;

        // ------------------------------------

        //* initializer and deconstructor

            SocketPlus();
            ~SocketPlus();

        // ------------------------------

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

        // * Other Useful functions

        //     void ClearDataPackets();
        //     /// @warning try to avoid using this function by deleting a packet once you use it
        //     /// @note very inefficient if used often with lots of data packets
        //     void ClearEmptyPackets();

        // ------------------------

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
