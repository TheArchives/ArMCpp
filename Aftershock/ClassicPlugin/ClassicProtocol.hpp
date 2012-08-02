/*
Copyright (c) 2011, Adam Guy
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Aftershock nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ADAM GUY BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef CLASSICPROTOCOL_HPP_INCLUDED
#define CLASSICPROTOCOL_HPP_INCLUDED

#include "Packets.hpp"

/*
#define C_BLACK "&0"
#define C_DBLUE "&1"
#define C_DGREEN "&2"
#define C_DTEAL "&3"
#define C_DRED "&4"
#define C_PURPLE "&5"
#define C_DYELLOW "&6"
#define C_GREY "&7"
#define C_DGREY "&8"
#define C_BLUE "&9"
#define C_GREEN "&a"
#define C_TEAL "&b"
#define C_RED "&c"
#define C_PINK "&d"
#define C_YELLOW "&e"
#define C_WHITE "&f"
#define C_RESET "&r"
*/

namespace Client_Packets
{
    enum: uint8_t { Info, Blockchange = 5, Movement = 8, Chat = 13 };

    // Client/Server information
    struct Packet_Info : Packet<0,uint8_t,std::string,std::string,char>::StaticLength<64,64>{
        enum:uint8_t{version,name,motd,op};
    };

    // Client->Server blockchange
    struct Packet_Blockchange : Packet<5,uint16_t,uint16_t,uint16_t,bool,uint8_t>::StaticLength<>{
        enum:uint8_t{x,y,z,mode,block};
    };

    // Client->Server movement
    struct Packet_Movement : Packet<8,uint8_t,int16_t,int16_t,int16_t,int8_t, int8_t>::StaticLength<> {
        enum:uint8_t{pid,x,y,z,h,p};
    };

    // Client->Server chat
    struct Packet_Chat : Packet<13,char,std::string>::StaticLength<64>{
        enum:uint8_t{pid,msg};
    };
}

namespace Server_Packets
{
    using Client_Packets::Packet_Info;

    typedef Packet<1>::StaticLength<> Packet_Ping;

    typedef Packet<2>::StaticLength<> Packet_Level_Init;

    struct Packet_Level_Chunk : Packet<3,uint16_t,std::string,unsigned char>::StaticLength<1024>{
        enum:uint8_t{size,mapdata,progress};
    };
    struct Packet_Level_Final :Packet<4,uint16_t,uint16_t,uint16_t>::StaticLength<>{
        enum:uint8_t{x,y,z};
    };
    struct Packet_SetBlock : Packet<6,uint16_t,uint16_t,uint16_t,unsigned char>::StaticLength<>{
        enum:uint8_t{x,y,z,block};
    };
    struct Packet_Spawn : Packet<7,uint8_t,std::string,int16_t,int16_t,int16_t,int8_t, int8_t>::StaticLength<64>{
        enum:uint8_t{pid,name,x,y,z,h,p};
    };

    typedef Client_Packets::Packet_Movement Packet_Teleport;

    struct Packet_Movement : Packet<9,char,char,char,char,char,char>::StaticLength<> {
        enum:uint8_t{pid,x,y,z,h,p};
    };
    struct Packet_Position : Packet<10,char,char,char,char>::StaticLength<> {
        enum:uint8_t{pid,x,y,z};
    };
    struct Packet_Orientation : Packet<11,char,char,char>::StaticLength<> {
        enum:uint8_t{pid,h,p};
    };
    struct Packet_Unspawn : Packet<12,char>::StaticLength<> {
        enum:uint8_t{pid};
    };

    using Client_Packets::Packet_Chat;

    struct Packet_Disconnect : Packet<14,std::string>::StaticLength<64>{
        enum:uint8_t{msg};
    };
    struct Packet_OP : Packet<15,char>::StaticLength<> {
        enum:uint8_t{type};
    };
}

enum MAP_BLOCK :
uint8_t
{
    BLOCK_AIR,
    BLOCK_STONE,
    BLOCK_GRASS,
    BLOCK_DIRT,
    BLOCK_COBBLESTONE,
    BLOCK_WOOD,
    BLOCK_SAPLING,
    BLOCK_ADMINIUM,
    BLOCK_WATER,
    BLOCK_STATIC_WATER,
    BLOCK_LAVA,
    BLOCK_STATIC_LAVA,
    BLOCK_SAND,
    BLOCK_GRAVEL,
    BLOCK_GOLD_ORE,
    BLOCK_IRON_ORE,
    BLOCK_COAL_ORE,
    BLOCK_TREE,
    BLOCK_LEAVES,
    BLOCK_SPONGE,
    BLOCK_GLASS,
    BLOCK_RED_CLOTH,
    BLOCK_ORANGE_CLOTH,
    BLOCK_YELLOW_CLOTH,
    BLOCK_LIGHT_GREEN_CLOTH,
    BLOCK_GREEN_CLOTH,
    BLOCK_AQUA_GREEN_CLOTH,
    BLOCK_CYAN_CLOTH,
    BLOCK_BLUE_CLOTH,
    BLOCK_PURPLE_CLOTH,
    BLOCK_INDIGO_CLOTH,
    BLOCK_VIOLET_CLOTH,
    BLOCK_MEGENTA_CLOTH,
    BLOCK_PINK_CLOTH,
    BLOCK_BLACK_CLOTH,
    BLOCK_GREY_CLOTH,
    BLOCK_WHITE_CLOTH,
    BLOCK_FLOWER,
    BLOCK_ROSE,
    BLOCK_RED_MUSHROOM,
    BLOCK_BROWN_MUSHROOM,
    BLOCK_GOLD,
    BLOCK_IRON,
    BLOCK_DOUBLE_STAIR,
    BLOCK_STAIR,
    BLOCK_BRICK,
    BLOCK_TNT,
    BLOCK_BOOKCASE,
    BLOCK_MOSSY_COBBLESTONE,
    BLOCK_OBSIDIAN
};
#endif // CLASSICPROTOCOL_HPP_INCLUDED
