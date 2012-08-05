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

#include "Logging.hpp"

#include <iostream>
#include <boost/date_time.hpp>

using namespace std;
using namespace boost::local_time;
using namespace boost::posix_time;

const char* outputLogger::INFO = "Info ";
const char* outputLogger::DEBUG = "Debug";
const char* outputLogger::ERROR = "Error";

outputLogger::outputLogger():
    m(),prefix(),fout(),Silent(false)
{
    prefix.imbue( locale(locale::classic(), new time_facet(DEFAULT_TIME_FORMAT)) );
}
outputLogger::outputLogger(const char* time_format ):
    m(),prefix(),fout(),Silent(false)
{
    prefix.imbue( locale(locale::classic(), new time_facet(time_format)) );
}
outputLogger::outputLogger(const char* time_format, const char* file_name, bool append, bool no_cout):
    m(),prefix(),fout(file_name,(append?std::ios::app:std::ios::out)),Silent(no_cout)
{
    if(!fout) throw 0;
    prefix.imbue( locale(locale::classic(), new time_facet(time_format)) );
}
outputLogger::~outputLogger()
{
    if(fout) fout.close();
}
bool outputLogger::silent()
{
    return Silent;
}
void outputLogger::silent(bool newval)
{
    if(fout) Silent = newval;
    else throw 1;
}
void outputLogger::setPrefix(const char* time_format)
{
    prefix.imbue( locale(locale::classic(), new time_facet(time_format)) );
}
void outputLogger::setFile(const char* file_name, bool append, bool no_cout)
{
    if(fout) fout.close();
    fout.open(file_name,(append?std::ios::app : std::ios::out));
    if(!fout) throw 1;
    Silent = no_cout;
}

outputLogger& outputLogger::beginOutput(const char* t)
{
    prefix.str("");
    prefix << ptime(microsec_clock::local_time());
    m.lock();
    return this->write( (prefix.str() + " - " + t + " - ").c_str() );
}
outputLogger& outputLogger::endOutput()
{
    write("\n");
    m.unlock();
    return *this;
}

outputLogger& outputLogger::write(const char* a)
{
    if(fout)
    {
        fout << a;
        if(!Silent) cout << a;
    }
    else cout << a;
    return *this;
}
outputLogger& outputLogger::write(const std::string& a)
{
    return this->write(a.c_str());
}



