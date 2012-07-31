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

#include "Server.hpp"
#include <iostream>

#include "TaskScheduler.hpp"

using boost::asio::ip::tcp;

using namespace std;
void beat();

serverClass::serverClass(): io_service(), acceptor(io_service,tcp::endpoint(tcp::v4(),42042))
{
    accept();

    info =
    {
        "Aftershock Dev",
        "Note: This server is in development",
        "591f849746d65341494dfa35887d20ad",
        7,
        42042,
        true,
        0,
        42
    };

    Tasks->add(&serverClass::step,Server);
    Tasks->add(beat);
}
serverClass::~serverClass()
{
    Log->debug("Deleting server...");
    Log->debug(connections.size(), " open connections");
    for(auto it=connections.begin(),itend=connections.end();it!=itend;++it)
    {
        Log->debug("Connection: ", it->get(), " ", it->use_count(), " existances" );
    }
    acceptor.cancel();
    acceptor.close();
    Log->debug("Done");
}
void serverClass::step()
{
    io_service.poll();
    onServerStep();
    Tasks->callLater(50,&serverClass::step,this);
}
void serverClass::accept()
{
    tcpSocketP sock = std::make_shared<tcp::socket>(io_service);
    acceptor.async_accept(*sock,boost::bind(&serverClass::new_connection, this, sock, boost::asio::placeholders::error));
}

void serverClass::new_connection(std::shared_ptr<tcp::socket> sock, const boost::system::error_code& error)
{
    if(error) throw boost_exception(error);
    onConnectionEvent event = onConnect(sock->remote_endpoint().address(), *(boost::asio::ip::tcp::resolver(io_service).resolve(sock->remote_endpoint())));
    if(!event) return;
    connections.insert(sock);
    std::shared_ptr<char> d = std::make_shared<char>(0);
    sock->async_receive(boost::asio::buffer(d.get(),1),boost::bind(&serverClass::on_connection_data,this,d,sock,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred));
    accept();
}
void serverClass::on_connection_data(std::shared_ptr<char> d,tcpSocketP sock, const boost::system::error_code& error, size_t bytes_transferred)
{
    if(!error) {
        Log->debug("Got new connection");
        onInitialData(*d,sock);
    } else Log->error("New Connection error");
    connections.erase(sock);
}
const ServerInfo& serverClass::getInfo()
{
    return const_cast<const ServerInfo&>(info);
}
