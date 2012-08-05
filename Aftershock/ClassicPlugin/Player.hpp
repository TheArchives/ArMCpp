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
      names of its contributors may be used to endorse o Lr promote products
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

#ifndef PLAYER_HPP_INCLUDED
#define PLAYER_HPP_INCLUDED

class Player;
struct PlayerCoordinates;
struct PlayerPosition;
struct plr_disconnect;

#include "Aftershock.hpp"
#include "ClassicProtocol.hpp"
#include "PlayerDB.hpp"
#include "taskScheduler.hpp"
#include "World.hpp"

#include <string>
#include <iostream>
#include <boost/make_shared.hpp>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/logic/tribool.hpp>

// Precise player location
struct PlayerCoordinates {
    uint32_t x,y,z;
};
struct PlayerPosition : PlayerCoordinates {
    uint8_t h,p;
};

struct PlayerCoordinatesOffset {
    int8_t x,y,z;
};
struct PlayerPositionOffset : PlayerCoordinatesOffset {
    int8_t h,p;
};
/*

class Player {

public:
    virtual void disconnect() = 0;
    virtual void kick(std::string) = 0;
    virtual void step() = 0;
    virtual void sendPacket(const PacketBase&) = 0;
    virtual PlayerInfoRef* operator->() = 0;
    virtual std::string str() = 0;
    virtual std::string colourUsername() = 0;
};
*/


class Player
{
    std::shared_ptr<boost::asio::ip::tcp::socket> sock;
    boost::logic::tribool disconnected;

    // World info
    World* currentWorld;
    bool isVisible;
    PlayerPosition pos;

    PlayerInfoRef DBInfo;
    TaskScheduler::Task sendMapTask;
    const uint8_t ID;

    void registerDataHandler();
    void onData(std::shared_ptr<char> d, const boost::system::error_code& ec, std::size_t bt);

    void sendMapInit(MapSendData& data);
    void sendMapChunk(MapSendData& data, MapSendData::ChunkIterator next, MapSendData::ChunkIterator end);
    void sendMapFinal(MapSendData & data);

    void leaveCurrentWorld();

    friend class World;

public:

    Player(std::shared_ptr<boost::asio::ip::tcp::socket> _sock, const PlayerInfoRef&, uint8_t id);
    ~Player();
    void disconnect();
    void kick(std::string msg = "Kicked");

    void joinWorld(World&);


    bool step();

    // Optaining player information
    PlayerInfoRef* operator->();
    std::string colourUsername(){
        return (*this)->username;
    }
    const uint8_t& id();
    const PlayerPosition& getPos();

    World& world(){
        return *currentWorld;
    }

    template<class...T>
    void info(const T&...t){
        Log->info(str(), ": ", t...);
    }
    template<class...T>
    void error(const T&...t){
        Log->error(str(), ": ", t...);
    }
    template<class...T>
    void debug(const T&...t){
        Log->debug(str(), ": ", t...);
    }


    void sendPacket(const Packets::Packet_Base& p){
        if(disconnected == true) {
            this->error("Tried to send data to disconnected player");
            return;
        }
        else if(boost::logic::indeterminate(disconnected)){
            return; // Socket has failed in last recv/send
        }
        try {
            p.send(*sock);
        }
        catch(const boost::exception&){
            this->error("Caught boost exception in player send");
            disconnected = boost::logic::indeterminate;
            disconnect(); // Wait for next step
        }
    }

    std::string str(){
        return "Player(" + boost::lexical_cast<std::string>((int)ID) + ", " + (*this)->username + ")";
    }

    bool operator==(const Player& p) const {
        return this->ID == p.ID;
    }
};



#endif // PLAYER_HPP_INCLUDED
