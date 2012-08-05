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

#include "Player.hpp"
#include "Server.hpp"
#include "Players.hpp"
#include "Worlds.hpp"

#include <boost/algorithm/string/trim.hpp>

using namespace boost::algorithm;
using namespace std;
namespace format = Aftershock::format;

Player::Player(std::shared_ptr<boost::asio::ip::tcp::socket> _sock, const PlayerInfoRef& i, uint8_t id): sock(_sock),disconnected(false), currentWorld(0), DBInfo(i), ID(id)
{
    this->debug("Created instance");
    joinWorld(Worlds->get("Default"));
    registerDataHandler();
}

Player::~Player(){
    if(!disconnected) kick("Server shut down");
}

void Player::registerDataHandler()
{
    std::shared_ptr<char> d = std::make_shared<char>(0);
    sock->async_receive(boost::asio::buffer(d.get(),1),
                        boost::bind(&Player::onData,
                                    this,
                                    d,
                                    boost::asio::placeholders::error,
                                    boost::asio::placeholders::bytes_transferred)
                       );
}


void Player::leaveCurrentWorld(){
    if(!currentWorld) {
        this->error("Unable to leave a null world");
        return;
    }
    if(isVisible) currentWorld->unSpawnPlayer(*this);
    currentWorld->unSpawnPlayers(*this);
    currentWorld->removePlayer(*this);
    currentWorld = nullptr;
}

void Player::disconnect()
{
    if(disconnected == true) return;
    if(sendMapTask->active()) sendMapTask->cancel();
    leaveCurrentWorld();
    sock->cancel(); // Cancel async operations
    sock->shutdown(boost::asio::ip::tcp::socket::shutdown_both); // Stop sending and receiving
    sock->close(); // Close the connection
    this->info("Disconnected");
    disconnected = true;
}

bool Player::step()
{
    if(disconnected) return false;
    /*
    boost::system::error_code error;
    if(sock->available(error) != 0){
        char d;
        sock->read_some(boost::asio::buffer(&d,1), error);
        if(error == boost::asio::error::misc_errors::eof) disconnect();
        else if(error) throw boost_exception(error);
        else onData(d);
    }
    else if(error == boost::asio::error::misc_errors::eof) disconnect();
    else if(error) throw boost_exception(error);
    */
    return true;
}

void Player::onData(std::shared_ptr<char> d, const boost::system::error_code& ec, std::size_t bt)
{
    if(ec or bt == 0) {
            this->error("Failed to receive data");
            disconnected = boost::logic::indeterminate;
            disconnect();
            return;
    }
    switch(*d)
    {
    case Client_Packets::Movement:
    {
        if(!currentWorld) break;
        Client_Packets::Packet_Movement movement;
        movement.recv(*sock);

        // Use a eventChain here

        // This can be eventChained
        PlayerPositionOffset offset;
        offset.x = movement.at<movement.x>() - pos.x;
        offset.y = movement.at<movement.y>() - pos.y;
        offset.z = movement.at<movement.z>() - pos.z;
        offset.h = movement.at<movement.h>() - pos.h;
        offset.p = movement.at<movement.p>() - pos.p;

        pos.x = movement.at<movement.x>();
        pos.y = movement.at<movement.y>();
        pos.z = movement.at<movement.z>();
        pos.h = movement.at<movement.h>();
        pos.p = movement.at<movement.p>();

        if( offset.x or offset.y or offset.z ) {
                pos.x = movement.at<movement.x>();
                pos.y = movement.at<movement.y>();
                pos.z = movement.at<movement.z>();
        }
        if( offset.h or offset.p ) {
                pos.h = movement.at<movement.h>();
                pos.p = movement.at<movement.p>();
        }
        currentWorld->updatePosition(*this, offset);
        // And another eventChain here
    }
    break;
    case Client_Packets::Blockchange:
    {
        Client_Packets::Packet_Blockchange change;
        change.recv(*sock);

        if(!currentWorld) break;

        Blocks::BlockInfo2 b;
        b.x = change.at<change.x>();
        b.y = change.at<change.y>();
        b.z = change.at<change.z>();
        b.type = change.at<change.mode>() ? change.at<change.block>() : BLOCK_AIR;
        currentWorld->changeBlock(b);
    }
    break;
    case Client_Packets::Chat:
    {
        Client_Packets::Packet_Chat chat;
        chat.recv(*sock);
        trim_right(chat.at<chat.msg>());
        Log->info(DBInfo.username,": ", chat.at<chat.msg>());
        currentWorld->sendChat(format::player::multiLine(DBInfo.username, chat.at<chat.msg>()));
    }
    break;
    }
    registerDataHandler();
}

PlayerInfoRef* Player::operator->()
{
    return &DBInfo;
}

void Player::sendMapInit(MapSendData& data)
{
    sendPacket(data.init);
    this->debug("Send map init");
    sendMapTask = Tasks->callLater(50,&Player::sendMapChunk,this,data,data.chunks.begin(),data.chunks.end());
}
void Player::sendMapChunk(MapSendData& data, MapSendData::ChunkIterator next, MapSendData::ChunkIterator end)
{
    sendPacket(*next);
    this->debug("Sent map chunk");
    next++;
    if(next != end)
    {
        sendMapTask = Tasks->callLater(50,&Player::sendMapChunk,this,data,next,end);
    }
    else sendMapFinal(data);
}
void Player::sendMapFinal(MapSendData & data)
{
    sendPacket(data.final);
    this->debug("Sent map final");

    currentWorld->addPlayer(*this);
    currentWorld->spawnPlayer(*this);
    currentWorld->spawnPlayers(*this);
}

void Player::joinWorld(World& w){
        if(currentWorld){
            if(isVisible) currentWorld->unSpawnPlayer(*this);
            currentWorld->removePlayer(*this);
        }
        currentWorld = &w;
        sendMapInit(w.getMapSend());
    }

const uint8_t& Player::id(){
    return ID;
}

void Player::kick(std::string msg ){
        Server_Packets::Packet_Disconnect k;
        k.set(msg);
        k.send(*sock);
        disconnect();
    }
