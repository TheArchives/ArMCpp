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


#include "Worlds.hpp"


    worldClass::worldClass()
    {
        boot("Default");
    }
    worldClass::~worldClass()
    {
        Log->debug("Shutting down worlds...");
        for(auto it=Worlds.begin(),itend=Worlds.end(); it!=itend; ++it)
            delete *it;
        Log->debug("Done");
    }

    World& worldClass::get(std::string name)
    {
        for(auto it=Worlds.begin(),itend=Worlds.end(); it!=itend; ++it)
        {
            if((*it)->Name == name)
            {
                return **it;
            }
        }
        throw 1;
    }

    World& worldClass::boot(std::string name)
    {
        Worlds.push_back(new World(name));
        return *Worlds.back();
    }
    void worldClass::shutdown(World& w)
    {
        auto it = std::find(Worlds.begin(),Worlds.end(),&w);
        if(it == Worlds.end()) throw 1;
        delete *it;
        Worlds.erase(it);
    }
    void worldClass::shutdown(std::string name)
    {
        for(auto it=Worlds.begin(),itend=Worlds.end(); it!=itend; ++it)
        {
            if((*it)->Name == name)
            {
                delete *it;
                Worlds.erase(it);
                return;
            }
        }
        throw 1;
    }


