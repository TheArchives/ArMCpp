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

#ifndef PACKETS_HPP_INCLUDED
#define PACKETS_HPP_INCLUDED

#include <tuple>
#include <string>
#include <ostream>
#include <initializer_list>
#include <boost/asio.hpp>


namespace Packets {
/// @cond NoDocument
namespace TupleDetail
{

template<class t, class f, size_t size>
struct _const_iterate_tuple
{
    static void work(const t&a, f& func)
    {
        _const_iterate_tuple<t,f,size-1>::work(a,func);
        func(std::get<size-1>(a));
    }
};

template <class t,class f>
struct _const_iterate_tuple<t,f,0>
{
    static void work(const t&a,f& func)
    {
    }
};

template <class t, class f>
void const_tuple_for_each(const t &a, f&& func)
{
    _const_iterate_tuple<t,f,std::tuple_size<t>::value>::work(a,func);
}

template<class t, class f, size_t size>
struct _iterate_tuple
{
    static void work(t&a, f& func)
    {
        _iterate_tuple<t,f,size-1>::work(a,func);
        func(std::get<size-1>(a));
    }
};

template <class t,class f>
struct _iterate_tuple<t,f,0>
{
    static void work(t&a,f& func)
    {
    }
};

template <class t, class f>
void tuple_for_each(t &a, f&& func)
{
    _iterate_tuple<t,f,std::tuple_size<t>::value>::work(a,func);
}

struct CountElements
{
    const unsigned * it;
    const unsigned * itend;
    size_t size;
    CountElements(const unsigned * _it, const unsigned * _itend): it(_it),itend(_itend), size(0) {}
    template<class t>
    void operator()(const t&)
    {
        size+=sizeof(t);
    }
};

template<>
void CountElements::operator()<std::string>(const std::string&);

template <class t>
size_t CountTupleElements(const t &a,const std::initializer_list<size_t>& d)
{
    CountElements ce(d.begin(),d.end());
    const_tuple_for_each(a,ce);
    return ce.size;
}
} // End of namespace TupleDetail
/// @endcond NoDocument

namespace ByteOrder
{

template<class t>
t copy(const t& a)
{
    return a;
}

template<>
int16_t copy<int16_t>(const int16_t&);
template<>
int32_t copy<int32_t>(const int32_t&);
template<>
int64_t copy<int64_t>(const int64_t&);

template<>
inline uint16_t copy<uint16_t>(const uint16_t& a)
{
    return copy<int16_t>(a);
}
template<>
inline uint32_t copy<uint32_t>(const uint32_t& a)
{
    return copy<int32_t>(a);
}
template<>
inline uint64_t copy<uint64_t>(const uint64_t& a)
{
    return copy<int64_t>(a);
}

template<class t>
t& swap(t& a)
{
    return a;
}
template<>
int16_t& swap<int16_t>(int16_t&);
template<>
int32_t& swap<int32_t>(int32_t&);
template<>
int64_t& swap<int64_t>(int64_t&);

template<>
inline uint16_t& swap<uint16_t>(uint16_t& a)
{
    return (uint16_t&)swap<int16_t>((int16_t&)a);
}
template<>
inline uint32_t& swap<uint32_t>(uint32_t& a)
{
    return (uint32_t&)swap<int32_t>((int32_t&)a);
}
template<>
inline uint64_t& swap<uint64_t>(uint64_t& a)
{
    return (uint64_t&)swap<int64_t>((int64_t&)a);
}
}



struct Packet_Base
{
    const char id; ///< ID of packet

    Packet_Base(const char& i): id(i) {}

    virtual void send(boost::asio::ip::tcp::socket& s) const = 0; ///< Send data instantly
    virtual void recv(boost::asio::ip::tcp::socket& s) = 0; ///< Receive data instantly
    //virtual void async_send(boost::asio::ip::tcp::socket& s) const = 0; // Send data on next sync

    boost::asio::ip::tcp::socket& operator<<(boost::asio::ip::tcp::socket& s) const;
    boost::asio::ip::tcp::socket& operator>>(boost::asio::ip::tcp::socket& s);

    virtual ~Packet_Base() {}
};

/// @cond NoDocument
namespace Detail {
struct outputData
{
    std::ostream &s;
    outputData(std::ostream& _s): s(_s) {}
    template<class t>
    void operator()(const t& a)
    {
        s << a << ", ";
    }
};

struct sendStaticLengthData
{
    std::list<size_t>::const_iterator it;
    const std::list<size_t>::const_iterator itend;
    boost::asio::ip::tcp::socket& s;
    sendStaticLengthData(boost::asio::ip::tcp::socket& _s, std::list<size_t>::const_iterator _it, const std::list<size_t>::const_iterator _itend): it(_it),itend(_itend),s(_s) {}
    template<class t>
    void operator()(const t&);
};

template<class t>
void sendStaticLengthData::operator()(const t& a)
{
    const t b = ByteOrder::copy(a);
    s.send(boost::asio::buffer((char*)&b,sizeof(t)));
    std::cout << "Sent: " << (int)a << std::endl;
}

template<>
void sendStaticLengthData::operator()<std::string>(const std::string&);
template<>
void sendStaticLengthData::operator()<std::wstring>(const std::wstring&);

struct recvStaticLengthData
{
    std::list<size_t>::const_iterator it;
    const std::list<size_t>::const_iterator itend;
    boost::asio::ip::tcp::socket& s;
    recvStaticLengthData(boost::asio::ip::tcp::socket& _s, std::list<size_t>::const_iterator _it, const std::list<size_t>::const_iterator _itend): it(_it),itend(_itend),s(_s) {}
    template<class t>
    void operator()(t&);
};

template<class t>
void recvStaticLengthData::operator()(t& a)
{
    s.receive(boost::asio::buffer((char*)&a,sizeof(a)));
    ByteOrder::swap(a);
    //std::cout << "Recieved: " << (int)a << std::endl;
}

template<>
void recvStaticLengthData::operator()<std::string>(std::string&);
template<>
void recvStaticLengthData::operator()<std::wstring>(std::wstring&);


struct sendDynamicLengthData
{
    boost::asio::ip::tcp::socket& s;
    sendDynamicLengthData(boost::asio::ip::tcp::socket& _s): s(_s) {}
    template<class t>
    void operator()(const t&);
};

template<class t>
void sendDynamicLengthData::operator()(const t& a)
{
    const t b = ByteOrder::copy(a);
    s.send(boost::asio::buffer((char*)&b,sizeof(t)));
    std::cout << "Sent: " << (int)a << std::endl;
}

template<>
void sendDynamicLengthData::operator()<std::string>(const std::string&);
template<>
void sendDynamicLengthData::operator()<std::wstring>(const std::wstring&);

struct recvDynamicLengthData
{
    boost::asio::ip::tcp::socket& s;
    recvDynamicLengthData(boost::asio::ip::tcp::socket& _s): s(_s) {}
    template<class t>
    void operator()(t&);
};

template<class t>
void recvDynamicLengthData::operator()(t& a)
{
    s.receive(boost::asio::buffer((char*)&a,sizeof(a)));
    ByteOrder::swap(a);
    //std::cout << "Recieved: " << (int)a << std::endl;
}

template<>
void recvDynamicLengthData::operator()<std::string>(std::string&);
template<>
void recvDynamicLengthData::operator()<std::wstring>(std::wstring&);

/// @endcond NoDocument

template<class...t>
struct PacketData {
    std::tuple<t...> data;
    template<size_t t2>
    auto at() -> decltype( std::get<t2>(data) )
    {
        return std::get<t2>(data);
    }
    template<class...t2>
    void set(t2&&...a)
    {
        data = std::make_tuple(a...);
    }
    template<class...t2>
    PacketData(const t2&... d): data(d...){}
    PacketData(const std::tuple<t...>& d): data(d){}
    PacketData():data(){}
};

} // End of Detail namespace
} // End of Packet namespace

// For using static length packets (static string lengths)
template<char ID, class...t>
struct Packet
{
    template<int...strSizes>
    struct StaticLength : Packets::Packet_Base, Packets::Detail::PacketData<t...> {
        const std::list<size_t> sizes;
        StaticLength(): Packets::Packet_Base(ID), Packets::Detail::PacketData<t...>(),sizes( {strSizes...}){}
        template<class...t2>
        StaticLength(const t2&... d): Packets::Packet_Base(ID), Packets::Detail::PacketData<t...>(d...), sizes( {strSizes...}) {}
        StaticLength(const StaticLength<strSizes...>& a): Packets::Packet_Base(a.id), Packets::Detail::PacketData<t...>(a.data), sizes(a.sizes) {}
        StaticLength<strSizes...>& operator=(const StaticLength<strSizes...>& a)
        {
            this->data = a.data;
            return *this;
        }
        void send(boost::asio::ip::tcp::socket& s) const
        {
            s.send(boost::asio::buffer(&id,1));
            //std::cout << "Sent ID " << (int)id << std::endl;
            Packets::TupleDetail::const_tuple_for_each(this->data,Packets::Detail::sendStaticLengthData(s,sizes.begin(), sizes.end()));
        }
        void recv(boost::asio::ip::tcp::socket& s)
        {
            Packets::Detail::recvStaticLengthData ss(s,sizes.begin(), sizes.end());
            Packets::TupleDetail::tuple_for_each(this->data,ss);
            //std::cout << "Recieved ID " << (int)id << std::endl;
        }

        std::ostream& operator<<(std::ostream& s)const
        {
            Packets::TupleDetail::const_tuple_for_each(this->data,Packets::Detail::outputData(s));
            return s;
        }
    };
    struct DynamicLength : Packets::Packet_Base, Packets::Detail::PacketData<t...> {
        DynamicLength(): Packets::Packet_Base(ID), Packets::Detail::PacketData<t...>(){}
        template<class...t2>
        DynamicLength(const t2&... d): Packets::Packet_Base(ID), Packets::Detail::PacketData<t...>(d...) {}
        DynamicLength(const DynamicLength& a): Packets::Packet_Base(a.id), Packets::Detail::PacketData<t...>(a.data) {}
        DynamicLength& operator=(const DynamicLength& a)
        {
            this->data = a.data;
            return *this;
        }
        void send(boost::asio::ip::tcp::socket& s) const
        {
            s.send(boost::asio::buffer(&id,1));
            //std::cout << "Sent ID " << (int)id << std::endl;
           Packets::TupleDetail::const_tuple_for_each(this->data,Packets::Detail::sendDynamicLengthData(s));
        }
        void recv(boost::asio::ip::tcp::socket& s)
        {
            Packets::Detail::recvDynamicLengthData ss(s);
            Packets::TupleDetail::tuple_for_each(this->data,ss);
            //std::cout << "Recieved ID " << (int)id << std::endl;
        }

        std::ostream& operator<<(std::ostream& s)const
        {
            Packets::TupleDetail::const_tuple_for_each(this->data,Packets::Detail::outputData(s));
            return s;
        }
    };
};



template<char ID, class...t,int...t2>
std::ostream& operator<<(std::ostream&s,const typename Packet<ID,t...>::template StaticLength<t2...>& a )
{
    return a.operator<<(s);
}
template<char ID, class...t>
std::ostream& operator<<(std::ostream&s,const typename Packet<ID,t...>::DynamicLength& a )
{
    return a.operator<<(s);
}


#endif // PACKETS_HPP_INCLUDED
