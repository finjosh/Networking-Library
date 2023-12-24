#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#pragma once

#include "SocketPlus.hpp"

class Server : public SocketPlus
{
    private:  

        //* Client variables for the server

            // first value is for the ID and the second is for the client data
            unordered_map<ID, ClientData> _clientData;
            // ID _lastID = 0;

        // ------------

        //* Thread Functions

            //* Pure Virtual Definition

                virtual void thread_receive_packets(std::stop_token stoken);
                virtual void thread_update(std::stop_token stoken, funcHelper::func<void> customPacketSendFunction);

            // ------------------------
            
            // Starts the receiving packets thread so that the client can communicate to the server
            void StartThreads(funcHelper::func<void> customPacketSendFunction);

        // -----------------

    public:
        //* Public data that the main will need access to 
            
            deque<ID> deletedClientIDs;
            deque<ID> newClientIDs;

            //* Events

                EventHelper::Event onClientConnected;
                EventHelper::Event onClientDisconnected;
                EventHelper::Event onServerClosed;
                EventHelper::Event onServerOpened;

            // -------
        
        // ----------------------------------------------

        Server(unsigned short port, bool passwordRequired = false);
        ~Server();
        void RequirePassword(bool requirePassword);
        void RequirePassword(bool requirePassword, string password);
        // returns true if server was started with the port that was set 
        // Also handles the thread if successful start or failed start
        bool StartServer(funcHelper::func<void> customPacketSendFunction);
        // resets all the data stored in the server
        void CloseServer();
        void setPort(unsigned short port);
        // returns true if the given packet is a connection request **removes the identifier from the packet
        // if the server requires a password then this is false
        // - adds client to client list
        // - adds ID to current ID list
        virtual bool isConnectionRequest(Packet& packet, IpAddress senderIPAddress, unsigned short senderPort);
        // returns true if the given packet is a connection close **removes the identifier from the packet
        // - removes the client from client list
        // - adds the ID to the list of closed connections by ID
        virtual bool isConnectionClose(Packet& packet, IpAddress senderIPAddress, unsigned short senderPort);
        // returns true if the given password is correct **removes the identifier from the packet
        // - if the password is wrong it requests a password again
        // if the password is correct
        // - removes the client from client list
        // - adds the ID to the list of closed connections by ID
        virtual bool isPassword(Packet& packet, IpAddress senderIPAddress, unsigned short senderPort);
        // returns true if the given packet is a data packet **removes the identifier from the packet
        virtual bool isData(Packet& packet);
        // Sends the given packet to every client currently connected
        void SendToAll(Packet& packet);
        // Tries to send the given packet to the client with the given ID
        void SendTo(Packet& packet, ID id);
        // // returns the ID of the last connected client if it was found
        // // if the last connected client could not be found returns 0 (the last client was lost/removed)
        // ID getLastConnectedClientID();
        void disconnectAllClients();
        // removes the client with the given ID
        // returns true if the client was removed returns false if it was not found
        bool disconnectClient(ID id);
        // returns a pointer to the clients map
        unordered_map<ID, ClientData>& getClients();
        static ClientData getClientData(std::pair<const ID, ClientData> client);
        Uint32 NumberOfClients();
        float getClientTimeSinceLastPacket(ID clientID);
        double getClientConnectionTime(ID clientID);
        unsigned int getClientPacketsPerSec(ID clientID);
};

#endif
