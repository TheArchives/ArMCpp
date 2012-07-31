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

#include <map>

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "Server.hpp"
#include "Logging.hpp"
#include "taskScheduler.hpp"

using namespace std;

using boost::posix_time::milliseconds;
using boost::thread;
using boost::lexical_cast;
using boost::asio::ip::tcp;

const char SAFE[256] =
{
    /*      0 1 2 3  4 5 6 7  8 9 A B  C D E F */
    /* 0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* 1 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* 2 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* 3 */ 1,1,1,1, 1,1,1,1, 1,1,0,0, 0,0,0,0,

    /* 4 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
    /* 5 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,
    /* 6 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
    /* 7 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,

    /* 8 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* 9 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* A */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* B */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,

    /* C */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* D */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* E */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* F */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
};

std::string UriEncode(const std::string & sSrc)
{
   const char DEC2HEX[16 + 1] = "0123456789ABCDEF";
   const unsigned char * pSrc = (const unsigned char *)sSrc.c_str();
   const int SRC_LEN = sSrc.length();
   unsigned char * const pStart = new unsigned char[SRC_LEN * 3];
   unsigned char * pEnd = pStart;
   const unsigned char * const SRC_END = pSrc + SRC_LEN;

   for (; pSrc < SRC_END; ++pSrc)
   {
      if (SAFE[*pSrc])
         *pEnd++ = *pSrc;
      else
      {
         // escape this char
         *pEnd++ = '%';
         *pEnd++ = DEC2HEX[*pSrc >> 4];
         *pEnd++ = DEC2HEX[*pSrc & 0x0F];
      }
   }

   std::string sResult((char *)pStart, (char *)pEnd);
   delete [] pStart;
   return sResult;
}

class httpRequest
{
    string host;
    string page;
    bool post;
    map<string,string> args;
public:
    httpRequest(string h,string p, bool po = true): host(h),page(p),post(po),args() {}

    string sendRequest(size_t recvbuflen = 9192)
    {
        tcp::iostream stream(host, "http");
        if(!stream) return "";
        else if(post)
        {
            size_t datalen = 0;
            for(auto it=args.begin(),itend=args.end(); it!=itend; ++it)
                datalen+=UriEncode(it->first).size()+UriEncode(it->second).size()+2;
            --datalen;

            stream << "POST /" << page << " HTTP/1.1\r\n";
            stream << "Host:" << host << "\r\n";
            stream << "Connection: close\r\n";
            stream << "Content-Type:application/x-www-form-urlencoded\r\n";
            stream << "Content-Length:" << datalen << "\r\n\r\n";


            for(auto it=args.begin(),itend=args.end(); it!=itend;)
            {
                stream << UriEncode(it->first) << "=" << UriEncode(it->second);
                if(++it != itend) stream << "&";
            }
        }
        else
        {
            stream << "GET /" << page << "?";
            for(auto it=args.begin(),itend=args.end(); it!=itend;)
            {
                stream << UriEncode(it->first) << "=" << UriEncode(it->second);
                if(++it != itend) stream << "&";
            }
            stream <<  " HTTP/1.1\r\n";;
            stream << "Host:" << host << "\r\n";
            stream << "Connection: close\r\n";
            stream << "Content-Length:0\r\n\r\n";
        }
        string data(recvbuflen,0);
        stream.read((char*)data.data(),recvbuflen);
        data.resize(stream.gcount());
        size_t headerEnd = data.find("\r\n\r\n");
        //cout << headerEnd << "/" << stream.gcount() << endl;
        if(headerEnd != string::npos) data.erase(0,headerEnd+4);
        stream.close();
        return data;
    }
    string& operator[](const string& s)
    {
        return args[s];
    }
};



volatile void doHeartBeat(ServerInfo& i)
{
    httpRequest r("www.minecraft.net","heartbeat.jsp", false);
    r["name"] = i.name;
    r["max"] = lexical_cast<string> (i.max);
    r["port"] = lexical_cast<string> (i.port);
    r["users"] = lexical_cast<string> (i.users);
    r["public"] = lexical_cast<string> ((int)i.Public);
    r["salt"] = i.salt;
    r["version"] = lexical_cast<string> (i.version);
    string hash = r.sendRequest(1024);
    boost::algorithm::trim(hash);
    if(hash.size() == 0) cout << "Unable to send heartbeat: no connection" << endl;
    else Log->info(hash);
}
void beat()
{
    static ServerInfo info;
    info = Server->getInfo();

    static thread hb;

    if(hb.timed_join(milliseconds(0)))
    {
        hb = thread( doHeartBeat,std::ref(info));
        Tasks->callLater(60000,beat);
    }
}
