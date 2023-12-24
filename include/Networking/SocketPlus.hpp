#ifndef SOCKETBASE_H
#define SOCKETBASE_H

#pragma once

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

using namespace std;
using namespace sf;

// ID = Uint32
typedef Uint32 ID;
// IP = ID (Uint32)
typedef ID IP;

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

struct DataPacket
{
    inline DataPacket(Packet packet)
    { this->packet = packet; editing = false; }
    DataPacket() = default;
    inline ~DataPacket()
    {
        bool endPacket = packet.endOfPacket();
        int dataSize = packet.getDataSize();
        // TODO packet crashing 
        // check if the packet is not being destroyed properly
        this->packet.clear();
    }
    Packet packet;
    bool editing = true;
};

// if when sending packets from host to client about enemies creates errors the issue would be that the enemy is being deleted
// while the packet sending thread is trying to read from that enemy
class SocketPlus : public UdpSocket
{
    protected:

        int _clientTimeoutTime = 120; // in seconds (2 min)

        //* Base Class variables

            IP _ip = 0;
            ID _id = 0;
            bool _needsPassword = false;
            string _password = "";
            unsigned short _port = 0;
            bool _isOpen_Connected = false;
            double _open_Connected_Time = 0.0;
            bool _threadSafeEvents = true;

        // ----------------

        //* thread variables

            // stop source is universal
            stop_source* _ssource = nullptr;
            // receiving thread
            jthread* _receive_thread = nullptr;
            // sending/updating thread
            jthread* _update_thread = nullptr;
            bool _sendingPackets = false;
            // in updates/second
            unsigned int _socketUpdateRate = 64;

        // ----------------

        //* Private thread functions

            //* PURE virtual functions
                
                // although pure virtual functions increase the time it takes for the function to be called it would be so minimal
                // it does not matter for this project
                
                virtual void thread_receive_packets(std::stop_token stoken) = 0;
                virtual void thread_update(std::stop_token stoken, funcHelper::func<void> customPacketSendFunction) = 0;

            // -----------------------

        // -------------------------

    public:

        //* Event stuff
            
            EventHelper::Event onDataReceived;
            EventHelper::Event onUpdateRateChanged;

            /// @brief will determine if events are called in thread safe mode
            /// @param threadSafe Default true
            void setThreadSafeEvents(bool threadSafe = true);
            bool isThreadSafeEvents();

        // ------

        //* Public data vars for ease of access
        
            list<DataPacket> DataPackets;

        // ------------------------------------

        //* initializer and deconstructor

            SocketPlus();
            ~SocketPlus();

        // ------------------------------

        //* Getter

            /// @returns ID
            ID getID();
            /// @returns IP as IPAddress
            IpAddress getIP();
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

        // -------

        //* Setters

            /// @brief sets the update interval in updates/second 
            /// @note DEFAULT = 64 (64 updates/second)
            void setUpdateInterval(unsigned int interval);
            /// @returns true if packets are being sent at the interval that was set
            void sendingPackets(bool sendPackets);
            /// @brief sets this password
            /// @note if this derived class is the server, sets the server password, else sets the password that will be sent to server
            void setPassword(string password);
            /// @brief sets the time for a client to timeout if no packets are sent (seconds)
            void setClientTimeout(const int& timeout);

        // --------

        //* Boolean question Functions

            /// @returns true if the given packet is a connection request **Does NOT change ANYTHING about the packet
            virtual bool isConnectionRequest(Packet packet);
            /// @returns true if the given packet is a connection close **Does NOT change ANYTHING about the packet
            virtual bool isConnectionClose(Packet packet);
            /// @returns true if the given packet is a data packet **Does NOT change ANYTHING about the packet
            virtual bool isData(Packet packet);
            /// @returns true if the given packet is a connection confirmation **Done NOT change ANYTHING about the packet
            virtual bool isConnectionConfirm(Packet packet);
            /// @returns true if the given packet is a password request **Done NOT change ANYTHING about the packet
            virtual bool isPasswordRequest(Packet packet);
            /// @returns true if the given packet is a password **Done NOT change ANYTHING about the packet
            virtual bool isPassword(Packet packet);
            /// @returns true if the given packet is informing of a wrong packet **Done NOT change ANYTHING about the packet
            virtual bool isWrongPassword(Packet packet);
            /// @returns true if the server is open
            bool isOpen();
            /// @returns true if the client is connected
            bool isConnected();
            /// @brief if the receiving thread is running
            /// @returns true
            bool isReceivingPackets();
            /// @brief if this is sending packets
            bool isSendingPackets();
            /// @brief if this needs a password
            bool NeedsPassword();
            static bool isValidIpAddress(IpAddress ipAddress);
            static bool isValidIpAddress(Uint32 ipAddress);
            static bool isValidIpAddress(string ipAddress);

        // ---------------------------

        //* Other Useful functions

            void ClearDataPackets();
            // @warning try to avoid using this function by deleting a packet once you use it
            // (very inefficient if used often with lots of data packets)
            void ClearEmptyPackets();

        // ------------------------

        //* Template Functions

            static Packet ConnectionCloseTemplate();
            static Packet ConnectionRequestTemplate();
            static Packet DataPacketTemplate();
            static Packet ConnectionConfirmPacket(Uint32 id);
            static Packet PasswordRequestPacket();
            static Packet PasswordPacket(string password);
            static Packet WrongPasswordPacket();

        // -------------------
};

#endif // SOCKETBASE_H
