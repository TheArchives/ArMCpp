/****************************************************************************************************\
*packet.cpp: Packet specifications                                                                   *
*The functions in this file form the                                                                 *
*packets that will be sent to the                                                                    *
*Minecraft client.                                                                                   *
*All code belongs to Mischa "Aster" Alff, Adam Guy (Adam01), and ArchivesMC (www.archivesmc.com)     *
\****************************************************************************************************/

#include <SFML/Network.hpp>
#include <string.h>


class SlotData
{
    public:
        short length;
        void define(std::string data)
        {
           static char ByteArray = new static char [data.size()];
           ByteArray = data.c_str;
           length=data,size();
        }
};


namespace MCPROT39
{
    namespace ClientPackets
    {

        sf::Packet KeepAlive0x00(int keepaliveid)
        {
            sf::Packet packet;
            packet << '\x00' << keepaliveid;
            return packet;
        }

        sf::Packet Handshake0x02(char protocolversion, std::string usrname, std::string serverhost, int port)
        {
            sf::Packet packet;
            packet << '\x02' << protocolversion << usrname << serverhost << port;
            return packet;
        }

        sf::Packet ChatMessage0x03(std::string message)
        {
            sf::Packet packet;
            packet << '\x03' << message;
            return packet;
        }
    }

    namespace ServerPackets
    {
        sf::Packet KeepAlive0x00(int keepaliveid)
        {
            sf::Packet packet;
            packet << '\x00' << keepaliveid;
            return packet;
        }

        sf::Packet LoginRequest0x01(int entityid, std::string leveltype, char servermode, char dimension, char difficulty, unsigned char worldheight, unsigned char maxplayers)
        {
            sf::Packet packet;
            packet << '\x01' << entityid << leveltype << servermode << dimension << difficulty << worldheight << maxplayers;
            return packet;
        }

        sf::Packet ChatMessage0x03(std::string message)
        {
            sf::Packet packet;
            packet << '\x03' << message;
            return packet;
        }

        sf::Packet TimeUpdate0x04(long _time)
        {
            sf::Packet packet;
            packet << '\x04' << _time;
            return packet;
        }

        sf::Packet EntityEquipment0x05(int entityid, short slot, SlotData item)
        {
            sf::Packet packet;
            packet << '\x05' << entityid << slot << item;
        }


    }
}







