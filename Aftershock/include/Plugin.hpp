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

#ifndef PLUGIN_HPP_INCLUDED
#define PLUGIN_HPP_INCLUDED

#include <string>
#include <list>
#include <memory>

#include "Aftershock.hpp"

/** Register Plugin Initialise Function.
 *  When developing a function, call this macro after your plugin class to create the Instantate function to allocate your plugin class for usage with aftershock.
 */
#define RegisterAftershockPlugin( classname, args ... ) \
extern "C" { \
    __declspec(dllexport) PluginInterface* InstantatePlugin(const _AftershockClassPointers& a){ \
        assignInternalPointers(a); \
        return new classname(args); \
    } \
};

namespace Plugin {

/** Class Interface for plugins.
 *  All virtual functions must be overwridden.
 *  The constructor for the derived class acts are the initialise function for the plugin.
 *  Throw PluginLoadFail if your plugin is unable to initialise.
 */
struct PluginInterface {
    virtual void Reload() = 0;
    virtual const char* Name() const throw() = 0;
    virtual const char* Description() const throw() = 0;
    virtual uint16_t Version() const throw() = 0;
    virtual ~PluginInterface() throw() = 0;
};

struct PluginException : Aftershock::AftershockException {
    const char* what() const throw() {return "PluginException";}
};

struct PluginNotLoaded : PluginException {
    const char* what() const throw() {return "Requested plugin is not loaded";}
};
struct PluginNoLib : PluginException {
    const char* what() const throw() {return "Plugin not found in filesystem";}
};
struct PluginLibFail : PluginException {
    const char* what() const throw() {return "Unable to open plugin library";}
};
struct PluginNoInstantate : PluginException {
    const char* what() const throw() {return "Unable to retrieve plugin instantate function";}
};
struct PluginLoadFail : PluginException {
    const char* what() const throw() {return "Plugin was unable to successfully initialise";}
};

/** Plugin Handler.
 *  Used to manage plugins.
 *  Globally instanced as Plugins
 */
class PluginHandler {
public:
    /** Load Plugin.
     *  Load a plugin, throws on failure.
     *  @param name The name of the plugin file excluding the file extension in the ./plugin/ directory
     *  @return A reference to the plugin's class
     */
    PluginInterface& Load(const std::string& name);

    /** Unload Plugin.
     *  Unloads the given plugin, if loaded. Throws PluginNotLoaded if the plugin can not be found.
     *  @param name The name used to load the plugin
     */
    void Unload(const std::string& name);

    /** Retrieve a plugin class by its name.
     *  Throws PluginNotLoaded if the plugin can not be found.
     *  @return A reference to the plugin's class
     */
    PluginInterface& Get(const std::string&);

    /** Check if plugin is loaded.
        @return A boolean indicated whether the plugin is loaded
     */
    bool Has(const std::string&);

    PluginHandler();
    ~PluginHandler();
private:
    struct _PluginHandle {
        void* handle;
        const std::string name;
        PluginInterface* pluginInterface;
        _PluginHandle(const std::string& n);
        ~_PluginHandle();
    };
    typedef std::shared_ptr<_PluginHandle> PluginHandle;
    std::list<PluginHandle> loadedPlugins;
    PluginHandle GetHandle(const std::string&);
};

}

/** Plugins class instance.
 *  Globally available pointer to the PluginHandler class.
 */
extern Plugin::PluginHandler *Plugins;
#endif // PLUGIN_HPP_INCLUDED
