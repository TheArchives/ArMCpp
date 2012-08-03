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

#include "Players.hpp"
#include "Server.hpp"
#include <boost/algorithm/string/trim.hpp>

using namespace boost::algorithm;
using namespace std;

Players::Players(): players() {
    onData = Server->onInitialData.addBack(&Players::onInitialData,this,std::placeholders::_1,std::placeholders::_2);
    onStep = Server->onServerStep.addBack(&Players::step,this);
    Log->debug("Added hooks");
}

Players::~Players()
{
    Log->debug("Deleting players...");
    for(auto it = players.begin(), itend=players.end(); it!=itend; ++it)
        delete *it;
    Log->debug("Done");
}
uint8_t Players::getFreeID(){
    for(char it=0, itend=IDS.size(); it!=itend;++it)
        if(!IDS[it]) return it;
    throw serverFull();
}


Player& Players::add(std::shared_ptr<tcp::socket> sock, PlayerInfoRef r)
{
    Player* p = 0;
    try {
        p = new Player(sock,r,getFreeID());
    }
    catch(serverFull){
        Server_Packets::Packet_Disconnect kick;
        kick.set("Server full");
        kick.send(*sock);
        if(p) delete p;
        throw;
    }
    IDS.set(p->id(),true);
    players.push_back(p);
    Log->debug("Added player (", (short)p->id(), ")" );
    return *p;
}
void Players::step()
{
    for(auto it=players.begin(),itend=players.end(); it!=itend; ++it)
    {
        if(!(*it)->step())
        {
            delete *it;
            IDS.reset((*it)->id());
            players.erase(it);
            it=players.begin();
            itend=players.end();
        }
    }
}


bool Players::onInitialData(char a, std::shared_ptr<tcp::socket> sock){
    Log->debug("Checking for player");
    if(a == 0){
        Log->debug("Is player");
        Client_Packets::Packet_Info info;
        info.recv(*sock);
        std::string uname = info.at<info.name>();
        trim_right(uname);
        if(db.hasPlayer(uname))
        {
            Log->debug("'", uname, "' has returned");
        }
        else
        {
            Log->debug("'", uname, "' has joined");
            db.addPlayer(uname);
        }
        auto nfo = Server->getInfo();
        info.set(nfo.version, nfo.name,nfo.description,(char)0);
        info.send(*sock);
        add(sock,db.getPlayerInfo(uname));
        return 1;
    }
    else Log->debug("Isn't a player");
    return 0;
}
