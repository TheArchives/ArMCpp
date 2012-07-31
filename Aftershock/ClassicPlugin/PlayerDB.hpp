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

#ifndef PDB_HPP_INCLUDED
#define PDB_HPP_INCLUDED

#include "Logging.hpp"
#include <string>
#include <fstream>

#include <CSVDB/DataBase.hpp>

namespace pdb
{
enum:
size_t { username, rank, blocksWalked, money, points, karma};
#define pdbget(a,s) std::get<pdb::s>(a);
#define pdbassign(a,s) s ( std::get< pdb:: s >( a ) )
}

// Note: Not such a good idea to use 'char' in the database
typedef DataBase<pdb::username, std::string, uint16_t, uint32_t, int32_t, uint32_t, uint16_t> PlayerDB_t;
typedef PlayerDB_t::RowType PlayerDB_Row_t;
typedef PlayerDB_t::TupleType PlayerDB_Tuple_t;

// Holds a copy of user data
struct PlayerInfoCopy
{
    std::string username;
    uint16_t rank;
    uint32_t blocksWalked;
    int32_t money;
    uint32_t points;
    uint16_t karma;
} static nullPlayerInfo;

// Holds a collection of references to data in a row
struct PlayerInfoRef
{
    std::string &username;
    uint16_t &rank;
    uint32_t &blocksWalked;
    int32_t &money;
    uint32_t &points;
    uint16_t &karma;
    PlayerInfoRef(PlayerDB_Tuple_t& a);
    PlayerInfoRef(PlayerInfoCopy& a);
};

// Might ditch this class and move code to players class
class PlayerDataBase
{
public:
    PlayerDataBase();
    ~PlayerDataBase();

    bool            hasPlayer(std::string name);
    PlayerInfoRef   addPlayer(const std::string& name);
    PlayerInfoRef   getPlayerInfo(const std::string& name);


    /* Planned:
        promote
        demote
        setRank

    */

protected:
    PlayerDB_t db;
};


#endif // PDB_HPP_INCLUDED
