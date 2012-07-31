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

#ifndef SERVER_HPP_INCLUDED
#define SERVER_HPP_INCLUDED

#include <string>
#include <set>

#include <boost/asio.hpp>

typedef boost::asio::ip::tcp::socket tcpSocket;
typedef std::shared_ptr<tcpSocket> tcpSocketP;
typedef boost::asio::ip::basic_resolver_entry<boost::asio::ip::tcp> resolveInfo;
typedef boost::asio::ip::address ipAddress;

#include "Aftershock.hpp"
#include "eventChain.hpp"

struct ServerInfo
{
    std::string name;
    std::string description;
    std::string salt;
    uint8_t version;
    uint16_t port;
    bool Public;
    uint16_t users;
    uint16_t max;
};

class serverClass
{
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::acceptor acceptor;
    ServerInfo info;
    std::set<tcpSocketP> connections;
public:
    eventChain<onConnectionEvent> onConnect;
    FunctionChain<bool(char, tcpSocketP)> onInitialData;
    FunctionChain<void()> onServerStep;

    serverClass();
    ~serverClass();
    void step();
    void accept();
    void new_connection(std::shared_ptr<tcpSocket> sock, const boost::system::error_code& error);
    void on_connection_data(std::shared_ptr<char> d,tcpSocketP sock,const boost::system::error_code& error, size_t bytes_transferred);
    const ServerInfo& getInfo();

} extern *Server;


#endif // SERVER_HPP_INCLUDED
