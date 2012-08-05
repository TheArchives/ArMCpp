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

#include "taskScheduler.hpp"

namespace TaskScheduler {

void _Task::sleepUntilCallTime()
{
    boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
    if(callTime < now) return;
    else boost::this_thread::sleep(callTime-now);
}
void _Task::sleepThenCall()
{
    if(Canceled || Called) return;
    sleepUntilCallTime();
    call();
}
Task _Task::call()
{
    f->call();
    Called = true;
    return shared_from_this();
}
_Task::~_Task()
{
}

Task _Task::reset(boost::posix_time::ptime t)
{
    if(t<startTime) throw 1;
    callTime = t;
    return shared_from_this();
}

Task _Task::reset(boost::posix_time::time_duration t)
{
    this->reset(startTime+t);
    return shared_from_this();
}
Task _Task::reset(const uint32_t& ms)
{
    return this->reset (boost::posix_time::milliseconds(ms) );
}

Task _Task::delay(boost::posix_time::time_duration t)
{
    this->reset(callTime+t);
    return shared_from_this();
}
Task _Task::delay(const uint32_t& ms)
{
    return this->delay (boost::posix_time::milliseconds(ms) );
}
bool _Task::operator>(const Task& t)const
{
    return callTime > t->callTime;
}

void TaskScheduler::handle_sigint(int)
{
    ::Tasks->sigint_raised = true;
}
Task TaskScheduler::addTask(Task&& t)
{
    tasks.push(t);
    return t;
}
int TaskScheduler::start()
{
    sigint_raised = false;
    signal(SIGINT,handle_sigint);

    run = true;
    while(run && !sigint_raised)
    {
        if(tasks.empty()) break;
        Task t = tasks.top();
        tasks.pop();
        t->sleepThenCall();
    }
    return ret;
}
bool TaskScheduler::interupted()
{
    return sigint_raised;
}
void TaskScheduler::setReturn(const int& r)
{
    ret = r;
}
void TaskScheduler::stop()
{
    run = false;
}
void TaskScheduler::stop(const int& r)
{
    setReturn(r);
    stop();
}
void TaskScheduler::clear()
{
    while(!tasks.empty()) tasks.pop();
}

}
