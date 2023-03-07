/*
 *    Copyright (c) 2021, The OpenThread Authors.
 *    All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *    POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 * This file implements the Task Runner that executes tasks on the mainloop.
 */

#include "common/task_runner.hpp"

#include <algorithm>

#include <fcntl.h>
#include <unistd.h>

#include "common/code_utils.hpp"

namespace otbr {

TaskRunner::TaskRunner(void)
    : mTaskQueue(DelayedTask::Comparator{})
{
    int flags;

    // We do not handle failures when creating a pipe, simply die.
    VerifyOrDie(pipe(mEventFd) != -1, strerror(errno));

    flags = fcntl(mEventFd[kRead], F_GETFL, 0);
    VerifyOrDie(fcntl(mEventFd[kRead], F_SETFL, flags | O_NONBLOCK) != -1, strerror(errno));
    flags = fcntl(mEventFd[kWrite], F_GETFL, 0);
    VerifyOrDie(fcntl(mEventFd[kWrite], F_SETFL, flags | O_NONBLOCK) != -1, strerror(errno));
}

TaskRunner::~TaskRunner(void)
{
    if (mEventFd[kRead] != -1)
    {
        close(mEventFd[kRead]);
        mEventFd[kRead] = -1;
    }
    if (mEventFd[kWrite] != -1)
    {
        close(mEventFd[kWrite]);
        mEventFd[kWrite] = -1;
    }
}

void TaskRunner::Post(const Task<void> aTask)
{
    Post(Milliseconds::zero(), std::move(aTask));
}

void TaskRunner::Post(Milliseconds aDelay, const Task<void> aTask)
{
    PushTask(aDelay, std::move(aTask));
}

void TaskRunner::Update(MainloopContext &aMainloop)
{
    FD_SET(mEventFd[kRead], &aMainloop.mReadFdSet);
    aMainloop.mMaxFd = std::max(mEventFd[kRead], aMainloop.mMaxFd);

    {
        std::lock_guard<std::mutex> _(mTaskQueueMutex);

        if (!mTaskQueue.empty())
        {
            auto  now     = Clock::now();
            auto &task    = mTaskQueue.top();
            auto  delay   = std::chrono::duration_cast<Microseconds>(task.GetTimeExecute() - now);
            auto  timeout = FromTimeval<Microseconds>(aMainloop.mTimeout);

            if (task.GetTimeExecute() < now)
            {
                delay = Microseconds::zero();
            }

            if (delay <= timeout)
            {
                aMainloop.mTimeout.tv_sec  = delay.count() / 1000000;
                aMainloop.mTimeout.tv_usec = delay.count() % 1000000;
            }
        }
    }
}

void TaskRunner::Process(const MainloopContext &aMainloop)
{
    OTBR_UNUSED_VARIABLE(aMainloop);

    ssize_t rval;

    // Read any data in the pipe.
    do
    {
        uint8_t n;

        rval = read(mEventFd[kRead], &n, sizeof(n));
    } while (rval > 0 || (rval == -1 && errno == EINTR));

    // Critical error happens, simply die.
    VerifyOrDie(errno == EAGAIN || errno == EWOULDBLOCK, strerror(errno));

    PopTasks();
}

void TaskRunner::PushTask(Milliseconds aDelay, const Task<void> aTask)
{
    ssize_t                     rval;
    const uint8_t               kOne = 1;
    std::lock_guard<std::mutex> _(mTaskQueueMutex);

    mTaskQueue.emplace(aDelay, std::move(aTask));
    do
    {
        rval = write(mEventFd[kWrite], &kOne, sizeof(kOne));
    } while (rval == -1 && errno == EINTR);

    VerifyOrExit(rval == -1);

    // Critical error happens, simply die.
    VerifyOrDie(errno == EAGAIN || errno == EWOULDBLOCK, strerror(errno));

    // We are blocked because there are already data (written by other concurrent callers in
    // different threads) in the pipe, and the mEventFd[kRead] should be readable now.
    otbrLogWarning("Failed to write fd %d: %s", mEventFd[kWrite], strerror(errno));

exit:
    return;
}

void TaskRunner::PopTasks(void)
{
    while (true)
    {
        Task<void> task;

        // The braces here are necessary for auto-releasing of the mutex.
        {
            std::lock_guard<std::mutex> _(mTaskQueueMutex);

            if (!mTaskQueue.empty() && mTaskQueue.top().GetTimeExecute() <= Clock::now())
            {
                task = std::move(mTaskQueue.top().mTask);
                mTaskQueue.pop();
            }
            else
            {
                break;
            }
        }

        task();
    }
}

} // namespace otbr
