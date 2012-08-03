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

#ifndef WORLD_HPP_INCLUDED
#define WORLD_HPP_INCLUDED

#include "Aftershock.hpp"
#include "taskScheduler.hpp"
#include "ClassicProtocol.hpp"
#include "eventChain.hpp"

// Stores map send packets
struct MapSendData
{
    Server_Packets::Packet_Level_Init init;
    std::list< Server_Packets::Packet_Level_Chunk > chunks;
    Server_Packets::Packet_Level_Final final;
    typedef std::list< Server_Packets::Packet_Level_Chunk >::iterator ChunkIterator;
};

class World;
struct MapSendData;

namespace Blocks {
    // Stores block type and location
    struct BlockInfo
    {
        uint32_t pos;
        uint8_t type;
    };
    // Just location
    struct BlockCoordinates
    {
        uint16_t x,y,z;
    };
    // Type and location
    struct BlockInfo2 : BlockCoordinates
    {
        uint8_t type;
    };
    // Type and location with reference to world dimensions
    struct BlockInfoEx : BlockInfo
    {
        const BlockCoordinates& dimensions;
        BlockInfoEx(const BlockCoordinates& d, uint32_t p, uint8_t t): BlockInfo( {p,t}),dimensions(d) {}
        BlockInfoEx(const BlockCoordinates& d, uint16_t x, uint16_t y, uint16_t z, uint8_t t): BlockInfo( {x*dimensions.x*dimensions.y + y*dimensions.y + z,t}), dimensions(d) {}
        uint16_t getX() const
        {
            return pos%dimensions.x;
        }
        uint16_t getY() const
        {
            return pos%dimensions.x*dimensions.y;
        }
        uint16_t getZ() const
        {
            return pos/dimensions.x % dimensions.z;
        }
    };
}

#include "Player.hpp"

/*
struct PlayerWorldInfo {
    struct PlayerPosition pos;
    bool spawned;
};
*/

#include <cstdio>
#include <iostream>
#include <list>
#include <set>
#include <queue>
#include <string>
#include <vector>
#include <atomic>

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

#include <libNBT/NBTLib.hpp>

class World
{
public:

    std::string Name;
private:

    // Stores block array stuff
    struct ThreeDimensionalArrayPointer
    {
        char* data;
        Blocks::BlockCoordinates dimensions;
        size_t size;

        ThreeDimensionalArrayPointer(char* d, uint16_t x, uint16_t y, uint16_t z):
            data(d),dimensions( {x,y,z}), size(x*y*z) {}
        ThreeDimensionalArrayPointer() = default;

        operator char*()
        {
            return data;
        }
        // Get block at coordinates
        char& operator()(size_t x, size_t y, size_t z)
        {
            assert(x < dimensions.x && y < dimensions.y && z < dimensions.z);
            return data[ x*dimensions.x*dimensions.y + y*dimensions.y + z ];
        }
        // Get block at offset
        char& operator()(size_t i)
        {
            assert(i < size);
            return data[i];
        }
    };
    // Stores block queue
    struct doubleBlockQueue
    {
        std::list<Blocks::BlockInfo> in;
        std::list<Blocks::BlockInfo> out;
    };

    // Map Data
    MapSendData sendData;
    NBTCompound FileData;
    ThreeDimensionalArrayPointer Blocks;

    // Physics resources
    boost::thread physics;
    time_t lastPhys;
    std::atomic<bool> physWorking;
    boost::mutex physDlock;
    doubleBlockQueue blockQueue;
    doubleBlockQueue physQueue;

    // Players
    std::set<Player*> Players;

    volatile void physics_thread();

    void processMapSend();
    void updateFromPhysQueue();
    void updatePhysQueue();
    void checkPhysicsQueue();
    bool load(std::string name);
    void broadcastPacket(const Packet_Base&);
    void broadcastPacket(const Packet_Base&, const Player&);
    void checkPlayer(Player&);

public:

    void addPlayer(Player&);
    void removePlayer(Player&);

    void spawnPlayer(Player&);
    void unSpawnPlayer(Player&);

    void spawnPlayers(Player&);
    void unSpawnPlayers(Player&);

    void sendChat(std::string, uint8_t=0);
    void sendChat(std::list<std::string>, uint8_t=0);

    void updatePosition(Player&, const struct PlayerPositionOffset&);

    void changeBlock(const Blocks::BlockInfo2&);

    MapSendData& getMapSend();
    PlayerPosition getMapSpawn();

    World(std::string);
    ~World();

    std::string str(){
        return "World("+Name+")";
    }
};


#endif // WORLD_HPP_INCLUDED
