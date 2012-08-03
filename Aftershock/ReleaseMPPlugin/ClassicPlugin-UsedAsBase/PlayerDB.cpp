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

#include "PlayerDB.hpp"

PlayerInfoRef::PlayerInfoRef(PlayerDB_Tuple_t& a):
        username ( std::get< pdb::username >( a ) ),
        rank ( std::get< pdb::rank >( a ) ),
        blocksWalked ( std::get< pdb::blocksWalked >( a ) ),
        money ( std::get< pdb::money >( a ) ),
        points ( std::get< pdb::points >( a ) ),
        karma ( std::get< pdb::karma >( a ) )
    {}
PlayerInfoRef::PlayerInfoRef(PlayerInfoCopy& a):
        username(a.username),
        rank(a.rank),
        blocksWalked(a.blocksWalked),
        money(a.money),
        points(a.points),
        karma(a.karma) {}

PlayerDataBase::PlayerDataBase()
{
    std::ifstream f("Players.csv");
    if(f.is_open())
    {
        f >> db;
        f.close();
        Log->debug("Imported player database.");
    }
}
PlayerDataBase::~PlayerDataBase()
{
    std::ofstream f("Players.csv", std::ios::trunc);
    if(f.is_open())
    {
        f << db;
        f.close();
        Log->debug("Saved player database.");
    }
}
bool PlayerDataBase::hasPlayer(std::string name)
{
    return db.hasKey(name);
}

PlayerInfoRef PlayerDataBase::addPlayer( const std::string& name)
{
    auto r = db.makeRow();
    std::get<pdb::username>(r.data) = name;
    return db.insertRow(r).data;
}

PlayerInfoRef PlayerDataBase::getPlayerInfo(const std::string& name)
{
    return db.getRowFromKey(name).data;
}
