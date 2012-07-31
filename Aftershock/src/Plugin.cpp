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

#include "Plugin.hpp"
#include "Logging.hpp"
#include "taskScheduler.hpp"
#include <boost/filesystem.hpp>

using namespace std;
using namespace boost::filesystem;

#ifdef _WIN32
#   include <windows.h>
#   define loadDLLFunction(lib, name) GetProcAddress((HINSTANCE) lib , name )
#   define loadError GetLastError()
#   define loadLib(path) LoadLibrary( path )
#   define freeLib(lib) FreeLibrary((HINSTANCE) lib )
#   define PLUGIN_EXTENSION ".dll"
#elif defined __unix
#   include <dlfcn.h>
#   define loadDLLFunction(lib, name) dlsym( lib , name )
#   define loadError dlerror()
#   define loadLib(path) dlopen( path , RTLD_LOCAL | RTLD_LAZY)
#   define freeLib(lib) dlclose( lib )
#   define PLUGIN_EXTENSION ".so"
#endif

PluginInterface::~PluginInterface() throw(){}

_PluginHandle::_PluginHandle(const string& n): handle(0),name(n),pluginInterface(0){
    Log->debug("Loading plugin '", name,"'");
    const string filename = "Plugins/"+name+PLUGIN_EXTENSION;
    if(!exists(filename)) throw PluginNoLib();
    // Load in dynamic library
    handle = loadLib(filename.c_str());
    if(!handle) {
        Log->debug("Dynamic link error: ", loadError);
        throw PluginLibFail();
    }
    // Retrieve pointer to plugin instantate function
    PluginInterface* (*f)(const _AftershockClassPointers&) = (PluginInterface*(*)(const _AftershockClassPointers&)) loadDLLFunction(handle, "InstantatePlugin");
    if(!f) {
        Log->debug("Dynamic link error: ", loadError);
        throw PluginNoInstantate();
    }
    // Call that function and retrieve plugin class
    pluginInterface = (*f)(AftershockClassPointers);
    Log->info("Loaded plugin '",name,"'");
}
_PluginHandle::~_PluginHandle(){
    if(pluginInterface) delete pluginInterface;
    if(handle) freeLib(handle);
    Log->info("Unloaded plugin '",name,"'");
}
PluginHandler::PluginHandler(){
    Log->info("Created Plugin Handler");

}
PluginHandler::~PluginHandler(){
    Log->info("Unloading plugins...");
    loadedPlugins.clear();
    Log->info("Destroyed Plugin Handler");
}
PluginHandler::PluginHandle PluginHandler::GetHandle(const string& name){
    for(auto it: loadedPlugins) if(it->name == name) return it;
    throw PluginNotLoaded();
}
PluginInterface& PluginHandler::Load(const string& name){
    PluginHandle np = make_shared<_PluginHandle>(name);
    loadedPlugins.push_back(np);
    return *np->pluginInterface;
}
void PluginHandler::Unload(const string& name){
    loadedPlugins.remove(GetHandle(name));
}
PluginInterface& PluginHandler::Get(const string& name){
    return *GetHandle(name)->pluginInterface;
}
bool PluginHandler::Has(const string& name){
    try {
        GetHandle(name);
        return true;
    }
    catch(PluginNotLoaded&){
        return false;
    }
}

