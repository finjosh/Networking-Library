#ifndef CLIENTDATA_H
#define CLIENTDATA_H

#include <SFML\Network.hpp>

struct ClientData
{
    inline ClientData(unsigned short port, sf::Uint32 id) : port(port), id(id)
    {}
    
    unsigned short port = 0;
    sf::Uint32 id = 0;

    unsigned int PacketsSent = 0;
    unsigned int PacketsPerSecond = 0;

    double ConnectionTime = 0.f;
    float TimeSinceLastPacket = 0.f;
};

#endif // CLIENTDATA_H
