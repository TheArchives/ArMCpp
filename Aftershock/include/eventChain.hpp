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

#ifndef EVENTHOOK_HPP_INCLUDED
#define EVENTHOOK_HPP_INCLUDED

#include <list>
#include <boost/signal.hpp>
#include <boost/type_traits.hpp>

#include "Events.hpp"


namespace EventChain {
typedef boost::signals::connection eventHook;
typedef boost::signals::scoped_connection scopedEventHook;

/// @cond NoDocument
namespace Detail {

template<class function_return>
struct Combine_Results {
    typedef function_return result_type;
    template<typename InputIterator>
    function_return operator()(InputIterator first, InputIterator last) const
    {
        return function_return(first, last);
    }
};

template<class returnType, class Function>
struct _signalType{
    typedef std::list<returnType> return_type;
    typedef boost::signal<Function,Combine_Results<return_type>> signal_type;
};

template<class Function>
struct _signalType<void, Function> {
    typedef boost::signal<Function> signal_type;
    typedef void return_type;
};

}
/// @endcond NoDocument

/// Thrown to break out of the eventChain operation
struct chainBreaker {};

/// Helper function
/// @see chainBreaker
inline void breakEventChain(){
    throw chainBreaker();
}




/** EventChain base class.
 *  Provides basic functionality for event chains
 */

template<class Function>
class eventChainBase
{
    public:
    typedef typename boost::function_traits<Function>::result_type function_return;
    typedef typename Detail::_signalType<function_return, Function>::signal_type signal_type;
    typedef typename Detail::_signalType<function_return, Function>::return_type return_type;

    /** Add an event to the front of the chain.
     * @param func Function to add
     * @returns An eventHook to that function
     */
    template<class F>
    eventHook addFront(F&& func){
        return this->hooks.connect(func,boost::signals::at_front);
    }

    /** Add an event to the back of the chain.
     * @param func Function to add
     * @returns An eventHook to that function
     */
    template<class F>
    eventHook addBack(F&& func){
        return this->hooks.connect(func,boost::signals::at_back);
    }

    /** Add an event to the desired position of the chain.
     * @param func Function to add
     * @param ord Position in chain
     * @returns An eventHook to that function
     */
    template<class F>
    eventHook addAt(int ord, F&& func){
        return this->hooks.connect(ord, func);
    }

    /** Remove an event from the chain.
     *  Doesn't work on binded functions
     * @param func Function to remove
     */
    template<class F>
    void remove(F&& func){
        this->hooks.disconnect(func);
    }

    /** Add a function to the front of the chain.
     * Add a binded function/arguments to the chain. Ensure you specify the placeholders.
     * Note that these event hooks cannot be removed
     * @param func Function to add
     * @param args Aguement order
     * @returns An eventHook to that function
     */
    template<class F, class...A>
    eventHook addFront(F&& func, A&&...args){
        return addFront(std::bind(func,args...));
    }
    /** Add a function to the back of the chain.
     * Add a binded function/arguments to the chain. Ensure you specify the placeholders.
     * Note that these event hooks cannot be removed
     * @param func Function to add
     * @param args Aguement order
     * @returns An eventHook to that function
     */
    template<class F, class...A>
    eventHook addBack(F&& func, A&&...args){
        return addBack(std::bind(func,args...));
    }
    /** Add an event to the desired position of the chain.
     * Add a binded function/arguments to the chain. Ensure you specify the placeholders.
     * Note that these event hooks cannot be removed
     * @param ord Position in chain
     * @param func Function to add
     * @param args Aguement order
     * @returns An eventHook to that function
     */
    template<class F, class...A>
    eventHook addAt(int ord, F&& func, A&&...args){
        return addAt(ord,std::bind(func,args...));
    }

    /** Get lastSuccess.
     * @returns a boolean indicating the last call was successful
     */
    bool complete(){
        return lastSuccess;
    }


    /** Add an event to the back of the chain.
     * @see addBack(F&& func)
     */
    template<class F>
    eventHook operator+(F&& func){
        return this->hooks.connect(func,boost::signals::at_back);
    }
    /** Remove an event from the chain.
     * @see remove
     */
    template<class F>
    void operator-(F&& func){
        this->hooks.disconnect(func);
    }

    protected:
    signal_type hooks;
    bool lastSuccess;
};

/// Event chain for functions
template <class Function>
struct FunctionChain : public eventChainBase<Function> {
    // Execute chain
    template<class...T>
    typename eventChainBase<Function>::return_type operator()(T&&... args){
        try {
            this->lastSuccess = true;
            return this->hooks(args...);
        }
        catch(chainBreaker){
            this->lastSuccess = false;
            return typename eventChainBase<Function>::return_type();
        }
    }
};
/// Event chain for functions that take Events::Event as the only parameter
template<class EventType>
struct eventChain : public eventChainBase<void(EventType&)> {
    // Execute chain
    template<class...T>
    EventType operator()(T&&... args){
        EventType e(args...);
        try {
            this->lastSuccess = true;
            this->hooks(std::ref(e));
        }
        catch(chainBreaker){
            this->lastSuccess = false;
        }
        return e;
    }
};
}

#endif // EVENTHOOK_HPP_INCLUDED
