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
#include "Plugin.hpp"


#include "version.h"

#include <boost/program_options.hpp>
#include <boost/exception/all.hpp>

#include <cstdio>
#include <conio.h>
#include <cstdlib>

using namespace std;
using namespace boost;
namespace po = boost::program_options;


template<class t>
void memAlloc(t*&a)
{
    a = (t*) malloc(sizeof(t));
}
template<class t>
void memConstruct(t*&a)
{
    new (a) t();
}
template<class t>
void memDestruct(t*&a)
{
    a->~t();
}

struct ClassManager
{
    ClassManager()
    {
        Log = new outputLogger();
        memAlloc(Tasks);
        memAlloc(Server);
        memAlloc(Plugins);
    }
    ~ClassManager()
    {
        free(Plugins);
        free(Server);
        free(Tasks);
        delete Log;
    }
    void Construct()
    {
        memConstruct(Tasks);
        memConstruct(Server);
        memConstruct(Plugins);
    }
    void Destruct()
    {
        memDestruct(Plugins);
        memDestruct(Server);
        memDestruct(Tasks);
    }
} static cm;

int main(int argc, char*argv[])
{
    cout << "AfterShock: " << versionStr() << endl;
    cout << "Launcher: " << AutoVersion::FULLVERSION_STRING << endl << endl;


    po::options_description opDesc("Parameters");
    po::variables_map opVars;
    opDesc.add_options()
    ("help","Display help")
    ("autoRestart,r", "Automatic restart on runtime error")
    ("showNanoSec,n", "Display nanoseconds in logging")
    ("debugLogging,d", po::value<bool>(&Log->debugging)->default_value(isDLLDebugBuild()), "Display debug output")
    ;

    try
    {
        po::store(po::parse_command_line(argc, argv, opDesc), opVars);
        po::notify(opVars);
    }
    catch(std::exception & e)
    {
        cout << "Parameter error: " << e.what() << endl;
        return 0;
    }


    if(opVars.count("help"))
    {
        cout << opDesc << endl;
        return 0;
    }

    Log->setPrefix(opVars.count("showNanoSec")? "%H:%M:%S%F" : "%H:%M:%S");
    Log->setFile("out.txt");

    int ret = 0;

    Log->info("Server launched");

    while(true)
    {
        // Construct classes
        cm.Construct();
        Plugins->Load("ClassicPlugin");
        try
        {
            // Start Tha Serva
            ret = Tasks->start();
        }

        // Debug stuff
        catch(std::exception& e)
        {
            Log->error(typeName(e), ": ",e.what());
        }
        catch(boost::exception & e )
        {
            cout << endl << diagnostic_information(e) << endl << endl;
        }
        catch(...)
        {
            Log->error("Caught unhanded exception");
        }
        bool sigint =  Tasks->interupted();

        // Destroy classes
        cm.Destruct();

        Log->debug("Server engine returned: ", ret);

        // Automatic restart
        if( opVars.count("autoRestart") && !sigint )
        {
            Log->info("Automaticly restarting server");
            continue;
        }

        // Manual restart
        cout << "Restart? (y/n): ";
        char s = getche();
        if( tolower(s) == 'y' )
        {
            cout << endl;
            Log->info("Restarting server on user request");
            continue;
        }
        break;
    }


    return ret;
}
