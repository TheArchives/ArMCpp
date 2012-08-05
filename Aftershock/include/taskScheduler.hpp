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

#ifndef TASKSCHEDULER_HPP_INCLUDED
#define TASKSCHEDULER_HPP_INCLUDED

#include <queue>
#include <functional>
#include <memory>
#include <csignal>
#include <atomic>

#include <boost/date_time.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>

#include "Logging.hpp"

namespace TaskScheduler {

typedef std::shared_ptr<class _Task>                       Task;

/// @cond NoDocument
namespace Detail {

typedef std::shared_ptr<class binded_function_base> shared_func;
struct binded_function_base
{
    virtual void call() = 0;
    virtual ~binded_function_base() {};
};
template<class callable>
class binded_function : public binded_function_base
{
    callable f;
public:
    binded_function(callable&& c): f(std::forward<callable>(c)) {}
    void call()
    {
        f();
    }
    ~binded_function() = default;
};
template<class t>
std::shared_ptr<binded_function<t>> make_func(t&&c)
{
    return std::make_shared< binded_function<t> >( std::forward<t>(c) );
}
template<class _f,class ..._a>
shared_func make_func(_f&& f, _a&&... a)
{
    return make_func( std::bind(f,a...) );
}
}
/// @endcond NoDocument


class _Task : public std::enable_shared_from_this<_Task>
{
    Detail::shared_func f;
    boost::posix_time::ptime startTime;
    boost::posix_time::ptime callTime;
    bool Canceled,Called;
    friend class TaskScheduler;
    void sleepUntilCallTime();
    void sleepThenCall();
    Task call();
public:
    template<class _f,class ..._a>
    _Task(_f&& __f, _a&&... a): f(Detail::make_func(__f,a...)),startTime(boost::posix_time::microsec_clock::local_time()),callTime(startTime),Canceled(false),Called(false)
    {
    }
    ~_Task();
    _Task(const _Task&) = delete;
    _Task operator=(const _Task&) = delete;

    Task reset(boost::posix_time::ptime t);
    Task reset(boost::posix_time::time_duration t);
    Task reset(const uint32_t& ms);

    Task delay(boost::posix_time::time_duration t);
    Task delay(const uint32_t& ms);
    bool operator>(const Task& t)const;

    Task cancel()
    {
        Canceled = true;
        return shared_from_this();
    }
    bool active()
    {
        return !Canceled || !Called;
    }
    bool called()
    {
        return Called;
    }
};

/// @cond NoDocument
namespace Detail {
struct TaskComp
{
    bool operator() (const Task& lhs, const Task& rhs) const
    {
        return *lhs>rhs;
    }
};
}
/// @endcond NoDocument

template<class _f,class ..._a>
Task make_task(_f&&f,_a&&... a)
{
    return std::make_shared<_Task>(f,a...);
}

class TaskScheduler
{
    std::priority_queue<Task, std::vector<Task>, Detail::TaskComp> tasks;
    std::atomic<bool> sigint_raised;
    static void handle_sigint(int);
    bool run;
    int ret;
public:
    Task addTask(Task&& t);

    template<class _f,class ..._a>
    Task add(_f&&f,_a&&... a)
    {
        return addTask(make_task(f,a...));
    }

    template<class _f,class ..._a>
    Task callLater(uint32_t ms, _f&&f,_a&&... a)
    {
        return addTask(make_task(f,a...)->delay(ms));
    }

    int start();
    bool interupted();
    void setReturn(const int& r);
    void stop();
    void stop(const int& r);
    void clear();

};

}

extern TaskScheduler::TaskScheduler *Tasks;

#endif // TASKSCHEDULER_HPP_INCLUDED
