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
#include "Plugin.hpp"
#include "version.h"
#include "Server.hpp"
#include <iostream>

using namespace std;

void onConnect(onConnectionEvent& e){
    Log->log("TestPlugin", "IP: ", e.addr.to_string(), " Hostname: ", e.resolved.host_name());
    e.status = true;
}

class TestPlugin : public PluginInterface {
    scopedEventHook onConnectHook;

    public:

    TestPlugin();  // Load
    ~TestPlugin() throw(); // Unload
    void Reload(); // Reload

    // Info
    const char* Name() const throw();
    const char* Description() const throw();
    uint16_t Version() const throw();
};
RegisterAftershockPlugin(TestPlugin)


TestPlugin::TestPlugin(){
    // throw PluginLoadFail on error
    onConnectHook = Server->onConnect + onConnect;
    Log->log("TestPlugin","Loaded");
}
TestPlugin::~TestPlugin() throw(){
    Log->log("TestPlugin","Unloaded");
}
void TestPlugin::Reload(){
    // throw PluginLoadFail on error
    //throw PluginLoadFail();
    Log->log("TestPlugin","Reloaded");
}
const char* TestPlugin::Name() const throw() {
    return "Test Plugin";
}
const char* TestPlugin::Description() const throw() {
    return "Plugin to test Aftershcok plugin system";
}
uint16_t TestPlugin::Version() const throw(){
    return 1;
}
