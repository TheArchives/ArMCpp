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

#include "Packets.hpp"

namespace Packets {

namespace TupleDetail
{
template<>
void CountElements::operator()<std::string>(const std::string&)
{
    assert(it!=itend);
    size+=*it++;
}

}

namespace ByteOrder
{

template<>
int16_t copy<int16_t>(const int16_t& a)
{
    return htons(a);
}
template<>
int32_t copy<int32_t>(const int32_t& a)
{
    return htonl(a);
}
template<>
int64_t copy<int64_t>(const int64_t& value)
{
    return ( htons(0xff00) == 0xff00) ? value : ((value << 56) | ((value << 40) & 0x00FF000000000000LL) |
            ((value << 24) & 0x0000FF0000000000LL)  | ((value << 8 ) & 0x000000FF00000000LL) |
            ((value >> 8 ) & 0x00000000FF000000LL)  | ((value >> 24) & 0x0000000000FF0000LL) |
            ((value >> 40) & 0x000000000000FF00LL)  | ((value >> 56) & 0x00000000000000FFLL) );
}

template<>
int16_t& swap<int16_t>(int16_t& a)
{
    return a = htons(a);
}
template<>
int32_t& swap<int32_t>(int32_t& a)
{
    return a = htonl(a);
}
template<>
int64_t& swap<int64_t>(int64_t& value)
{
    return ( htons(0xff00) == 0xff00) ? value : value = ((value << 56) | ((value << 40) & 0x00FF000000000000LL) |
            ((value << 24) & 0x0000FF0000000000LL)  | ((value << 8 ) & 0x000000FF00000000LL) |
            ((value >> 8 ) & 0x00000000FF000000LL)  | ((value >> 24) & 0x0000000000FF0000LL) |
            ((value >> 40) & 0x000000000000FF00LL)  | ((value >> 56) & 0x00000000000000FFLL) );
}

}

namespace Detail
{

template<class t>
void sendStaticLengthString(const t& str, size_t size, boost::asio::ip::tcp::socket& sock){
    t b(str);
    b.resize(size);
    sock.send(boost::asio::buffer((char*)b.data(),b.length()));
    std::cout << "Sent string: (" << b.length() << ")" << b.c_str() << std::endl;
    //for(unsigned i =0; i<b.size(); ++i) std::cout << (int)b[i] << " ";
    //std::cout << std::endl;
}

template<class t>
void recvStaticLengthString(t& str, size_t size, boost::asio::ip::tcp::socket& sock){
    str.resize(size);
    sock.receive(boost::asio::buffer((char*)str.data(),str.length()));
    std::cout << "Recieved string: " << str.c_str() << std::endl;
}

template<>
void sendStaticLengthData::operator()<std::string>(const std::string& a)
{
    assert(it!=itend);
    sendStaticLengthString<std::string>(a,*it++,s);
}

template<>
void recvStaticLengthData::operator()<std::string>(std::string& a)
{
    assert(it!=itend);
    recvStaticLengthString<std::string>(a,*it++,s);
}
template<>
void sendStaticLengthData::operator()<std::wstring>(const std::wstring& a)
{
    assert(it!=itend);
    sendStaticLengthString<std::wstring>(a,*it++,s);
}

template<>
void recvStaticLengthData::operator()<std::wstring>(std::wstring& a)
{
    assert(it!=itend);
    recvStaticLengthString<std::wstring>(a,*it++,s);
}


template<>
void sendDynamicLengthData::operator()<std::string>(const std::string& a)
{
    (*this)((short)a.length());
    sendStaticLengthString<std::string>(a,a.length(),s);
}

template<>
void recvDynamicLengthData::operator()<std::string>(std::string& a)
{
    short l;
    (*this)(l);
    recvStaticLengthString<std::string>(a,l,s);
}

template<>
void sendDynamicLengthData::operator()<std::wstring>(const std::wstring& a)
{
    (*this)((short)a.length());
    sendStaticLengthString<std::wstring>(a,a.length(),s);
}

template<>
void recvDynamicLengthData::operator()<std::wstring>(std::wstring& a)
{
    short l;
    (*this)(l);
    recvStaticLengthString<std::wstring>(a,l,s);
}

} // Detail ns

boost::asio::ip::tcp::socket& Packet_Base::operator<<(boost::asio::ip::tcp::socket& s) const
{
    this->send(s);
    return s;
}
boost::asio::ip::tcp::socket& Packet_Base::operator>>(boost::asio::ip::tcp::socket& s)
{
    this->recv(s);
    return s;
}

} // Packets ns

