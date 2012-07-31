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

#ifndef AFTERSHOCK_HPP_INCLUDED
#define AFTERSHOCK_HPP_INCLUDED

#include <boost/system/error_code.hpp>
#include <boost/thread.hpp>

#include <typeinfo>
#include <string>
#include <exception>
#include <array>

bool isDLLDebugBuild();
bool isDLLReleaseBuild();

// Base exception class for Aftershock exceptions
struct AftershockException : public std::exception
{
    virtual ~AftershockException() throw();
    virtual const char* what() const throw();
};

// Exceptions with a variable message
class AftershockRuntimeException : public AftershockException
{
    std::string msg;
public:
    AftershockRuntimeException(const std::string& error);
    virtual ~AftershockRuntimeException() throw();
    virtual const char* what() const throw();
};

// An exception containing a boost error code
struct boost_exception : public AftershockRuntimeException
{
    boost_exception(const boost::system::error_code& error);
    virtual ~boost_exception() throw();
};

// Thrown when the demangle function cant demangle a string
struct bad_type_name: AftershockRuntimeException
{
    bad_type_name(const std::string& name, const int& status);
};

// Demangle a string, easily
std::string demangle(const std::string& name);

// Get the demangled name of a type
template<class t>
std::string typeName(t&& _t)
{
    return demangle(typeid(t).name());
}

// Structure containing pointers to the core classes
struct _AftershockClassPointers
{
    class outputLogger **LogP;
    class TaskScheduler **TasksP;
    class serverClass **ServerP;
    class PluginHandler **PluginsP;
} extern AftershockClassPointers;

// Call this from a plugin
void assignInternalPointers(const _AftershockClassPointers&);

// Depreciated, Suspend program for time
inline void msSleep(uint32_t ms)
{
    boost::this_thread::sleep(boost::posix_time::milliseconds(ms));
}

// Depreciated, but generates a random number between b and e
int rangeRand(int b, int e);

// Get version of Aftershock lib
std::array<long,4> version();
// Get the version date of the lib
std::array<const char*,3> versionDate();
// Get the version and a string
const char* versionStr();

void addTasks();

namespace format {
    std::list<std::string> multiLine(std::string msg);
    namespace player {
        std::string singleLine(std::string username, std::string msg);
        std::list<std::string> multiLine(std::string username, std::string msg);
    }

}

#endif // AFTERSHOCK_HPP_INCLUDED
