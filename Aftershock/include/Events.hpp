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

#ifndef EVENTS_HPP_INCLUDED
#define EVENTS_HPP_INCLUDED

#include <boost/logic/tribool.hpp>
#include <boost/asio.hpp>

#include <list>

namespace Events {

/** Base Event class.
 *  Base class for events containing basic status tribool
 */

struct Event {
    boost::logic::tribool status;
    operator bool(){
        return status;
    }
    bool operator!(){
        return !status;
    }
};

/// @cond NoDocument
template<class t>
struct dataEvent : Event {
    std::list<t> data;
    typename std::list<t>::iterator currentData;
    t& thisData(){
        return currentData;
    }
};
/// @endcond NoDocument

/** Class for connection events.
 *  This is used when after receives a new connection.
 *  Use Event::status to determine whether to allow the connection:
 *  true:allow, false:disallow
 */

struct onConnectionEvent : Event {
    const boost::asio::ip::address &addr;
    const boost::asio::ip::basic_resolver_entry<boost::asio::ip::tcp> &resolved;
    onConnectionEvent(const boost::asio::ip::address &_addr,  const boost::asio::ip::basic_resolver_entry<boost::asio::ip::tcp> &_resolved):
        addr(_addr),resolved(_resolved){status=true;}
};

}

#endif // EVENTS_HPP_INCLUDED
