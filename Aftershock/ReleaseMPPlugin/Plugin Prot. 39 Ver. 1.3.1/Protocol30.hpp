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

#ifndef RELEASEPROTOCOL30_HPP_INCLUDED
#define RELEASEPROTOCOL30_HPP_INCLUDED

#include "Packets.hpp"

typedef int8_t byte;
typedef uint8_t ubyte;
/*
#define C_BLACK "\u00A70"
#define C_DBLUE "\u00A71"
#define C_DGREEN "\u00A72"
#define C_DCYAN "\u00A73"
#define C_DRED "\u00A74"
#define C_PURPLE "\u00A75"
#define C_GOLD "\u00A76"
#define C_GREY "\u00A77"
#define C_DGREY "\u00A78"
#define C_BLUE "\u00A79"
#define C_BGREEN "\u00A7a"
#define C_CYAN "\u00A7b"
#define C_RED "\u00A7c"
#define C_PINK "\u00A7d"
#define C_YELLOW "\u00A7e"
#define C_WHITE "\u00A7f"
#define C_RESET "\u00A7r"
#define S_RANDOM "\u00A7k"
#define S_BOLD "\u00A7l"
#define S_STRIKEOUT "\u00A7m"
#define S_UNDERLINE "\u00A7n"
#define S_ITALIC "\u00A7o"
#define S_RESET "\u00A7r"
*/

class SlotData
{
    public:
        std::string SlotDataString;
        
        SlotData(std::string SlotDataString_)
        {
            SlotDataString = SlotDataString_;
        }
        SlotData(const SlotData &Source)
        {
            SlotDataString = Source.SlotDataString;
        }
        
        SlotData& operator= (const SlotData &source);
};

SlotData& SlotData::operator= (const SlotData &Source)
{
    SlotDataString = Source.SlotDataString;
    static char ByteArray = new static char [Source.SlotDataString.size()];
    ByteArray = Source.SlotDataString.c_str;
    return *this;
}


namespace Client_Packets
{
    enum: uint8_t { Info, Blockchange = 5, Movement = 8, Chat = 13 };

    struct Packet_KeepAlive0x00<\x00,int>::DynamicLength
    {
        enum:uint32_t
        {
            keepaliveid
        };
    };
    
    struct Packet_Handshake0x02<\x02,byte,std::wstring,std::wstring,int>::DynamicLength
    {
        enum:uint32_t
        {
            protocolversion,
            username,
            serverhost,
            serverport
        };
    };
    
    struct Packet_ChatMessage0x03<\x03,std::wstring>::DynamicLength
    {
        enum:uint32_t
        {
            message
        };
    };
    
}

namespace Server_Packets
{
    struct Packet_KeepAlive0x00<\x00,int32_t>::DynamicLength
    {
        enum:uint32_t
        {
            keepaliveid
        };
    };
    
    struct Packet_LoginRequest0x01<\x01,int32_t,std::wstring,byte,byte,byte,ubyte,ubyte>::DynamicLength
    {
        enum:uint32_t
        {
            entityid,
            leveltype,
            gamemode,
            dimension,
            difficulty,
            worldheight,
            maxplayers
        };
    };
    
    struct Packet_ChatMessage0x03<\x03,std::wstring>::DynamicLength
    {
        enum:uint32_t
        {
            message
        };
    };
    
    struct Packet_TimeUpdate0x04<\x04,long>::DynamicLength
    {
        enum:uint32_t
        {
            time
        };
    };
    
    struct Packet_Entity
    
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
