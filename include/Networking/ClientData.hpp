#ifndef CLIENTDATA_H
#define CLIENTDATA_H

#include <SFML/Config.hpp>

struct ClientData
{
    inline ClientData(unsigned short port, sf::Uint32 id) : port(port), id(id)
    {}
    
    unsigned short port = 0;
    sf::Uint32 id = 0;

    /// @brief TEMP var for storing how many packets are sent between seconds
    unsigned int PacketsSent = 0;
    /// @brief Should be updated once every second using PacketsSent
    unsigned int PacketsPerSecond = 0;

    double ConnectionTime = 0.f;
    float TimeSinceLastPacket = 0.f;
};

#endif // CLIENTDATA_H
