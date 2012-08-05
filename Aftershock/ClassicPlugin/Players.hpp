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

#ifndef PLAYERS_HPP_INCLUDED
#define PLAYERS_HPP_INCLUDED

#include <string>
#include <iostream>
#include <list>
#include <memory>
#include <bitset>

#include "Player.hpp"
#include "PlayerDB.hpp"

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>

using boost::asio::ip::tcp;

struct serverFull {};

class Players
{
    std::list<Player*> players;
    std::bitset<255> IDS; // Perhaps too conserving?
    uint8_t getFreeID();
    uint16_t Max, Reserved;
    EventChain::scopedEventHook onData,onStep;

public:
    PlayerDataBase db;
    Players(); // Registeres onData hook
    ~Players();

    Player& add(std::shared_ptr<tcp::socket> sock, PlayerInfoRef r); // Adds a player (from onInitialData)

    void step(); // Checks for ping timeouts

    bool onInitialData(char a, std::shared_ptr<tcp::socket> sock); // Checks to see if we got a mc classic handshake

    // undefined
    uint16_t max();
    void setMax(uint16_t);
    uint16_t avaliable();
    uint16_t online();
    uint16_t reserved();
    void setReserved(uint16_t);
} extern *players;



#endif // PLAYERS_HPP_INCLUDED
