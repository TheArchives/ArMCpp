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
#include "Aftershock.hpp"
#include "Logging.hpp"
#include "taskScheduler.hpp"
#include "Aftershock.hpp"
#include "../version.h"

#include <cxxabi.h>
#include <boost/random.hpp>
#include <boost/lexical_cast.hpp>
using namespace std;

#if defined AFTERSHOCK_DEBUG_BUILD && AFTERSHOCK_RELEASE_BUILD
#   error There can only be one of either debug or release define
#endif

bool isDLLDebugBuild(){
    #ifdef AFTERSHOCK_DEBUG_BUILD
    return true;
    #   else
    return false;
    #endif
}
bool isDLLReleaseBuild(){
    #ifdef AFTERSHOCK_RELEASE_BUILD
    return true;
    #   else
    return false;
    #endif
}

AftershockException::~AftershockException() throw() {}
const char* AftershockException::what() const throw()
{
    return "Aftershock Exception";
}

AftershockRuntimeException::AftershockRuntimeException(const std::string& error)
    :msg(error)
{}
AftershockRuntimeException::~AftershockRuntimeException() throw() {}
const char* AftershockRuntimeException::what() const throw()
{
    return msg.c_str();
}

boost_exception::boost_exception(const boost::system::error_code& error)
    :AftershockRuntimeException("Boost error code: "+boost::lexical_cast<std::string>(error))
{};
boost_exception::~boost_exception() throw() {}

bad_type_name::bad_type_name(const std::string& name, const int& status)
    :AftershockRuntimeException("Unable to demangle type name (" + boost::lexical_cast<std::string>(status) + "): " + name)
{}

outputLogger * Log;
TaskScheduler * Tasks;
serverClass * Server;
PluginHandler * Plugins;

void assignInternalPointers(const _AftershockClassPointers& a){
    Log = *a.LogP;
    Tasks = *a.TasksP;
    Server = *a.ServerP;
    Plugins = *a.PluginsP;
}

_AftershockClassPointers AftershockClassPointers({&Log,&Tasks,&Server,&Plugins});


string demangle(const string& name)
{
    int status;
    char * cstr = abi::__cxa_demangle(name.c_str(),0,0, &status);
    if(cstr)
    {
        std::string str(cstr);
        free(cstr);
        return str;
    }
    else throw bad_type_name(name, status);
}

int rangeRand(int b, int e)
{
    boost::uniform_int<> dist(b, e);
    static boost::mt19937 gen;
    boost::variate_generator<boost::mt19937&, boost::uniform_int<> > die(gen, dist);
    return die();
}
const char* versionStr()
{
    return AutoVersion::FULLVERSION_STRING;
}
std::array<long,4> version()
{
    std::array<long,4> a = {{ AutoVersion::MAJOR,AutoVersion::MINOR,AutoVersion::BUILD,AutoVersion::REVISION }};
    return a;
}
std::array<const char*,3> versionDate()
{
    std::array<const char*,3> a = {{AutoVersion::DATE,AutoVersion::MONTH, AutoVersion::YEAR}};
    return a;
}

namespace format {
    std::list<std::string> multiLine(std::string msg){
        std::list<std::string> lines;
        while(msg.size()){
            lines.push_back(msg.substr(0,64));
            msg.erase(0,64);
        }
        return lines;
    }
    namespace player {
        std::string singleLine(std::string username, std::string msg){
            return username + ": " + msg;
        }
        std::list<std::string> multiLine(std::string username, std::string msg){
            std::list<std::string> lines;
            while(msg.size()){
                msg = singleLine(username,msg);
                lines.push_back(msg.substr(0,64));
                msg.erase(0,64);
            }
            return lines;
        }
    }
}
