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

#ifndef LOGGING_HPP_INCLUDED
#define LOGGING_HPP_INCLUDED

#include <string>
#include <fstream>
#include <sstream>

#include <boost/thread/mutex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>


// Thread safe logging class

#define DEFAULT_TIME_FORMAT "%H:%M:%S%F"

#ifdef ERROR // Some constant thats defined somewhere out of my scope
#undef ERROR
#endif

class outputLogger
{
public:

    static const char* INFO;
    static const char* DEBUG;
    static const char* ERROR;

    bool debugging;

    outputLogger();
    outputLogger(const char* time_format );
    outputLogger(const char* time_format, const char* file_name, bool append = true, bool no_cout = false);
    ~outputLogger();

    void setPrefix(const char* time_format);
    void setFile(const char* file_name, bool append = true, bool no_cout = false);
    bool silent();
    void silent(bool newval);

    // Log with log type
    template<class...vt>
    outputLogger& log(const char*, const vt&...);

    // Log info
    template<class...vt>
    outputLogger& info(const vt&...);

    // Log error
    template<class...vt>
    outputLogger& error(const vt&...);

    // Log debug (can be disabled with outputLogger::debugging)
    template<class...vt>
    outputLogger& debug(const vt&...);

    // Log information (same as outputLogger::info)
    template<class...vt>
    outputLogger& operator()(const vt&...);
private:
    boost::mutex m;
    std::stringstream prefix;
    std::ofstream fout;
    bool Silent;

    outputLogger& beginOutput(const char*);
    outputLogger& endOutput();

    outputLogger& write(const char*);
    outputLogger& write(const std::string&);
    template<class t>
    outputLogger& write(const t&);
    template<class t,class...vt>
    outputLogger& write(const t&,const vt&...);

} extern * Log;
// Writing
template<class t>
outputLogger& outputLogger::write(const t& a)
{
    return this->write( boost::lexical_cast<std::string>(a) );
}
template<class t,class...vt>
outputLogger& outputLogger::write(const t& a, const vt&...v)
{
    return this->write(a).write(v...);
}
// Logging
template<class...vt>
outputLogger& outputLogger::log(const char*t,const vt&...v)
{
    return beginOutput(t).write(v...).endOutput();
}
template<class...vt>
outputLogger& outputLogger::operator()(const vt&...v)
{
    return this->log(outputLogger::INFO, v...);
}
template<class...vt>
outputLogger& outputLogger::info(const vt&...v)
{
    return this->log(outputLogger::INFO, v...);
}
template<class...vt>
outputLogger& outputLogger::error(const vt&...v)
{
    return this->log(outputLogger::ERROR, v...);
}
template<class...vt>
outputLogger& outputLogger::debug(const vt&...v)
{
    return ( debugging? this->log(outputLogger::DEBUG, v...) : *this );
}
#endif // LOGGING_HPP_INCLUDED
