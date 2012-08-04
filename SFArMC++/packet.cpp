/****************************************************************************************************\
*packet.cpp: Packet specifications                                                                   *
*The functions in this file form the                                                                 *
*packets that will be sent to the                                                                    *
*Minecraft client.                                                                                   *
*All code belongs to Mischa "Aster" Alff, Adam Guy (Adam01), and ArchivesMC (www.archivesmc.com)     *
\****************************************************************************************************/

#include <SFML/Network.hpp>
#include <wstring.h>

sf::Packet KeepAlive0x00(int keepaliveid)
{
    sf::Packet packet;
    packet << '\x00' << keepaliveid;
    return packet;
}

sf::Packet LoginRequest0x01(int32_t entityid, wchar_t[] leveltype, int8_t servermode, int8_t dimension, int8_t difficulty, uint8_t worldheight, uint8_t maxplayers)
{
    sf::Packet packet;
    packet << '\x01' << entityid << leveltype << servermode << dimension << difficulty << worldheight << maxplayers;
    return packet;
}

sf::Packet Handshake0x02(int8_t protocolversion, wchar_t[] usrname, wchar_t[] serverhost, int32_t port)
{
    sf::Packet packet;
    packet << '\x02' << protocolversion << usrname << serverhost << port;
    return packet;
}

sf::Packet ChatMessage0x03(wchar_t[] message)
{
    sf::Packet packet;
    packet << '\x03' << message;
    return packet;
}

sf::Packet TimeUpdate0x04(int64_t _time)
{
    sf::Packet packet;
    packet << '\x04' << _time;
    return packet;
}

sf::Packet EntityEquipment(int32_t entityid, int16_t slot, )




