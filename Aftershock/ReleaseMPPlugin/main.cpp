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
#include "Players.hpp"
#include "Worlds.hpp"
#include <iostream>

Players * players;
worldClass * Worlds;

using namespace std;

class ClassicPlugin : public PluginInterface {
    public:

    ClassicPlugin();  // Load
    ~ClassicPlugin() throw(); // Unload
    void Reload(); // Reload

    // Info
    const char* Name() const throw();
    const char* Description() const throw();
    uint16_t Version() const throw();
};

RegisterAftershockPlugin(ClassicPlugin)


ClassicPlugin::ClassicPlugin(){
    // throw PluginLoadFail on error
    Worlds = new worldClass;
    players = new Players;
    Log->log("ClassicPlugin","Loaded");
}
ClassicPlugin::~ClassicPlugin() throw(){
    delete players;
    delete Worlds;
    Log->log("ClassicPlugin","Unloaded");
}
void ClassicPlugin::Reload(){
    delete players;
    delete Worlds;
    Worlds = new worldClass;
    players = new Players;
    Log->log("ClassicPlugin","Reloaded");
}
const char* ClassicPlugin::Name() const throw() {
    return "Minecraft classic runtime plugin";
}
const char* ClassicPlugin::Description() const throw() {
    return "Adds minecraft classic functionality to Aftershock";
}
uint16_t ClassicPlugin::Version() const throw(){
    return 1;
}
