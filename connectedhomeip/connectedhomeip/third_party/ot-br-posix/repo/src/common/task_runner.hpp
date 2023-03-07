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
 * This file defines the Task Runner that executes tasks on the mainloop.
 */

#ifndef OTBR_COMMON_TASK_RUNNER_HPP_
#define OTBR_COMMON_TASK_RUNNER_HPP_

#include <openthread-br/config.h>

#include <chrono>
#include <functional>
#include <future>
#include <mutex>
#include <queue>

#include "common/code_utils.hpp"
#include "common/mainloop.hpp"
#include "common/time.hpp"

namespace otbr {

/**
 * This class implements the Task Runner that executes
 * tasks on the mainloop.
 *
 */
class TaskRunner : public MainloopProcessor, private NonCopyable
{
public:
    /**
     * This type represents the generic executable task.
     *
     */
    template <class T> using Task = std::function<T(void)>;

    /**
     * This constructor initializes the Task Runner instance.
     *
     */
    TaskRunner(void);

    /**
     * This destructor destroys the Task Runner instance.
     *
     */
    ~TaskRunner(void) override;

    /**
     * This method posts a task to the task runner and returns immediately.
     *
     * Tasks are executed sequentially and follow the First-Come-First-Serve rule.
     * It is safe to call this method in different threads concurrently.
     *
     * @param[in] aTask  The task to be executed.
     *
     */
    void Post(const Task<void> aTask);

    /**
     * This method posts a task to the task runner and returns immediately.
     *
     * The task will be executed on the mainloop after `aDelay` milliseconds from now.
     *
     * @param[in] aDelay  The delay before executing the task (in milliseconds).
     * @param[in] aTask   The task to be executed.
     *
     */
    void Post(Milliseconds aDelay, const Task<void> aTask);

    /**
     * This method posts a task and waits for the completion of the task.
     *
     * Tasks are executed sequentially and follow the First-Come-First-Serve rule.
     * This method must be called in a thread other than the mainloop thread. Otherwise,
     * the caller will be blocked forever.
     *
     * @returns The result returned by the task @p aTask.
     *
     */
    template <class T> T PostAndWait(const Task<T> &aTask)
    {
        std::promise<T> pro;

        Post([&pro, &aTask]() { pro.set_value(aTask()); });

        return pro.get_future().get();
    }

    void Update(MainloopContext &aMainloop) override;
    void Process(const MainloopContext &aMainloop) override;

private:
    enum
    {
        kRead  = 0,
        kWrite = 1,
    };

    struct DelayedTask
    {
        friend class Comparator;

        struct Comparator
        {
            bool operator()(const DelayedTask &aLhs, const DelayedTask &aRhs) const { return aRhs < aLhs; }
        };

        DelayedTask(Milliseconds aDelay, Task<void> aTask)
            : mTimeCreated(Clock::now())
            , mDelay(aDelay)
            , mTask(std::move(aTask))
        {
        }

        bool operator<(const DelayedTask &aOther) const
        {
            return GetTimeExecute() <= aOther.GetTimeExecute() ||
                   (GetTimeExecute() == aOther.GetTimeExecute() && mTimeCreated < aOther.mTimeCreated);
        }

        Timepoint GetTimeExecute(void) const { return mTimeCreated + mDelay; }

        Timepoint    mTimeCreated;
        Milliseconds mDelay;
        Task<void>   mTask;
    };

    void PushTask(Milliseconds aDelay, const Task<void> aTask);
    void PopTasks(void);

    // The event fds which are used to wakeup the mainloop
    // when there are pending tasks in the task queue.
    int mEventFd[2];

    std::priority_queue<DelayedTask, std::vector<DelayedTask>, DelayedTask::Comparator> mTaskQueue;

    // The mutex which protects the `mTaskQueue` from being
    // simultaneously accessed by multiple threads.
    std::mutex mTaskQueueMutex;
};

} // namespace otbr

#endif // OTBR_COMMON_TASK_RUNNER_HPP_
