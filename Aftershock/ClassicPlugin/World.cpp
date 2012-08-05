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

#include "World.hpp"

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <zlib/zlib.h>

using namespace std;
using namespace Packets;
namespace format = Aftershock::format;


volatile void World::physics_thread()
{
    for(;;)
    {
        physWorking = true;

        physWorking = false;
        physDlock.lock();
        physDlock.lock();
        physDlock.unlock();
    }
}


World::World(string _n): Name(_n)
{
    if(load(Name))
    {
        Log->debug("Loading ", Name, " map data");
        Blocks.data = (char*) FileData["Map"]->ByteArrays["Blocks"].data.get();
        Blocks.dimensions.x = FileData["Map"]->Shorts["Width"];
        Blocks.dimensions.y = FileData["Map"]->Shorts["Height"];
        Blocks.dimensions.z = FileData["Map"]->Shorts["Length"];
        Blocks.size = Blocks.dimensions.x*Blocks.dimensions.y*Blocks.dimensions.z;
        sendData.final.set(Blocks.dimensions.x,Blocks.dimensions.y,Blocks.dimensions.z);
        processMapSend();
    }
    else
    {
        Log->info("Unable to load world '",Name,"'");
        throw(1);
    }
    Log->info("Booted world '",Name,"'");
    //physics = thread(&World::physics_thread, this);
    //Tasks->callLater(750,&World::checkPhysicsQueue,this);

}

bool World::load(std::string name)
{
    std::ifstream i( ("worlds/"+name+".mclevel").c_str(), std::ios::binary);
    if(!i) return false;
    boost::iostreams::filtering_istream in;
    in.push(boost::iostreams::gzip_decompressor());
    in.push(i);
    if(in.get() != TAG_COMPOUND) return false;
    else
    {
        FileData.read(in);
        return true;
    }
}
// Send packet to all clients in the world
void World::broadcastPacket(const Packet_Base& p){
    for(auto it=Players.begin(),itend=Players.end();it!=itend;++it)
        (*it)->sendPacket(p);
}
// Same as above, but not to player p2
void World::broadcastPacket(const Packet_Base& p, const Player& p2){
    for(auto it=Players.begin(),itend=Players.end();it!=itend;++it)
        if( !( (**it) == p2 ) )  (*it)->sendPacket(p);
}

void World::processMapSend()
{
    Log->debug("Processing map...");

    // Get network order integer of blocks
    int32_t BlockCountBE = ByteOrder::copy(Blocks.size);

    // Initialise array containing the blocks integer
    boost::iostreams::array_source mapSize((char*)&BlockCountBE,4);
    // Initialise array containing map data
    boost::iostreams::array_source mapBlocks(Blocks.data,Blocks.size);

    boost::iostreams::filtering_istream in;
    std::stringbuf stream;
    in.set_auto_close(false);

    // Gzip blocks integer
    in.push(boost::iostreams::gzip_compressor());
    in.push(mapSize);
    in >> &stream;

    // And then gzip the blocks
    in.reset();
    in.push(boost::iostreams::gzip_compressor());
    in.push(mapBlocks);
    in >> &stream;

    // Obtain zipped data
    std::string zippedData(stream.str());

    // Debug that data
    Log->debug("Blocks size: ", Blocks.size, ", Zipped size: " , zippedData.size());

    // Initialise packets
    sendData.chunks.assign( ceil(zippedData.size()/1024.0) ,Server_Packets::Packet_Level_Chunk() );
    Log->debug(sendData.chunks.size(), " chunks");

    // Initialise chunking data
    auto ChunkPacket = sendData.chunks.begin();
    uint32_t chunkSize;
    const char* it = zippedData.data();
    const char* const itend = it+zippedData.size();
    double percent = 0;
    const double percentIncrease = (double)100.0 / sendData.chunks.size();

    // Create chunks!
    do
    {
        percent+=percentIncrease;
        chunkSize = itend-it;
        if(chunkSize>1024) chunkSize = 1024;
        ChunkPacket->set(chunkSize,std::string(it,chunkSize),round(percent));
        ++ChunkPacket;
        it+=chunkSize;
    }
    while(chunkSize==1024);
}


World::~World()
{
    Log->info("Shutting down world '",Name,"'");
    blockQueue.in.clear();
    blockQueue.out.clear();

    physQueue.in.clear();
    physQueue.out.clear();
}

void World::updateFromPhysQueue()
{
    // This pushes the blocks from the physics output to the world output
    // (needs implementing)
    physQueue.out.clear();
    physDlock.unlock();
    Tasks->callLater(750,&World::checkPhysicsQueue,this);
}

void World::updatePhysQueue()
{
    physQueue.in = blockQueue.in;
}
void World::checkPhysicsQueue()
{
    if(!physWorking) Tasks->add(&World::updateFromPhysQueue,this);
}

// Player stuff

void World::checkPlayer(Player& p){
        assert(Players.find(&p) != Players.end());
    }

void World::addPlayer(Player& p){
    Players.insert(&p);
    Log->debug(p.str(), " joined ", this->str());
}
void World::removePlayer(Player&p){
    Players.erase(&p);
    Log->debug(p.str(), " left ", this->str());
}
void World::spawnPlayer(Player&p){
    checkPlayer(p);

    p.pos = getMapSpawn();
    Server_Packets::Packet_Teleport s;
    s.set(-1, p.pos.x, p.pos.y, p.pos.z, p.pos.h,p.pos.p);
    p.sendPacket(s);

    Server_Packets::Packet_Spawn s2;
    s2.set(p.id(), p.colourUsername(), p.pos.x, p.pos.y, p.pos.z, p.pos.h,p.pos.p);
    broadcastPacket(s2,p);
    Log->debug(p.str(), " spawned in ", this->str());
}
void World::unSpawnPlayer(Player&p){
    checkPlayer(p);
    Server_Packets::Packet_Unspawn up;
    up.set(p.id());
    broadcastPacket(up);
    Log->debug(p.str(), " unspawned from ", this->str());
}
void World::spawnPlayers(Player&p){
    checkPlayer(p);
    Log->debug(p.str(), " spawned players in ", this->str());
    Server_Packets::Packet_Spawn s;
    for(auto it=Players.begin(), itend=Players.end();it!=itend;++it){
        Player& p2= **it;
        if(p == p2) continue;
        s.set(p2.id(), p2.colourUsername(), p2.pos.x, p2.pos.y, p2.pos.z, p2.pos.h, p2.pos.p);
        p.sendPacket(s);
    }
}


void World::updatePosition(Player& p, const PlayerPositionOffset& offset){
    if( offset.x or offset.y or offset.z ){
        if( offset.h or offset.p) {
            Server_Packets::Packet_Movement pos;
            pos.set(p.id(), offset.x,offset.y,offset.z,p.pos.p,p.pos.h);
            broadcastPacket(pos,p);
        }
        else {
            Server_Packets::Packet_Position pos;
            pos.set(p.id(),offset.x,offset.y,offset.z);
            broadcastPacket(pos,p);
        }
    }
    else if(offset.h or offset.p){
        Server_Packets::Packet_Orientation pos;
        pos.set(p.id(),p.pos.h,p.pos.p);
        broadcastPacket(pos,p);
    }
}

void World::unSpawnPlayers(Player&p){
    checkPlayer(p);
    Log->debug(p.str(), " unspawned players from ", this->str());
    // todo
}

void World::sendChat(std::string msg, uint8_t type){
    sendChat(format::multiLine(msg),type);
}
void World::sendChat(std::list<std::string> msg, uint8_t type){
    Server_Packets::Packet_Chat c;
    c.at<c.pid>() = type;
    for(auto it=msg.begin(),itend=msg.end();it!=itend;++it)
    {
        c.at<c.msg>() = *it;
        broadcastPacket(c);
    }
}

void World::changeBlock(const Blocks::BlockInfo2& block){
    Server_Packets::Packet_SetBlock b;
    b.set(block.x,block.y,block.z,block.type);
    broadcastPacket(b);
}

MapSendData& World::getMapSend(){
    return sendData;
}

PlayerPosition World::getMapSpawn(){
    auto it = FileData["Map"]->Lists["Spawn"].begin<NBTShort>();
    PlayerPosition c;
    c.x = ((*it++)<<5)+16;
    c.y = ((*it++)<<5)+16;
    c.z = ((*it)<<5)+16;
    c.h = FileData["Map"]->Shorts["SpawnH"];
    c.p = FileData["Map"]->Shorts["SpawnP"];
    return c;
}

