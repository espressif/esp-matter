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

#include "common/task_runner.hpp"

#include <atomic>
#include <mutex>
#include <thread>

#include <CppUTest/TestHarness.h>

TEST_GROUP(TaskRunner){};

TEST(TaskRunner, TestSingleThread)
{
    int                   rval;
    int                   counter = 0;
    otbr::MainloopContext mainloop;
    otbr::TaskRunner      taskRunner;

    mainloop.mMaxFd   = -1;
    mainloop.mTimeout = {10, 0};

    FD_ZERO(&mainloop.mReadFdSet);
    FD_ZERO(&mainloop.mWriteFdSet);
    FD_ZERO(&mainloop.mErrorFdSet);

    // Increase the `counter` to 3.
    taskRunner.Post([&]() {
        ++counter;
        taskRunner.Post([&]() {
            ++counter;
            taskRunner.Post([&]() { ++counter; });
        });
    });

    taskRunner.Update(mainloop);
    rval = select(mainloop.mMaxFd + 1, &mainloop.mReadFdSet, &mainloop.mWriteFdSet, &mainloop.mErrorFdSet,
                  &mainloop.mTimeout);
    CHECK_EQUAL(1, rval);

    taskRunner.Process(mainloop);
    CHECK_EQUAL(3, counter);
}

TEST(TaskRunner, TestTasksOrder)
{
    std::string           str;
    otbr::TaskRunner      taskRunner;
    int                   rval;
    otbr::MainloopContext mainloop;

    taskRunner.Post([&]() { str.push_back('a'); });
    taskRunner.Post([&]() { str.push_back('b'); });
    taskRunner.Post([&]() { str.push_back('c'); });

    mainloop.mMaxFd   = -1;
    mainloop.mTimeout = {2, 0};

    FD_ZERO(&mainloop.mReadFdSet);
    FD_ZERO(&mainloop.mWriteFdSet);
    FD_ZERO(&mainloop.mErrorFdSet);

    taskRunner.Update(mainloop);
    rval = select(mainloop.mMaxFd + 1, &mainloop.mReadFdSet, &mainloop.mWriteFdSet, &mainloop.mErrorFdSet,
                  &mainloop.mTimeout);
    CHECK_TRUE(rval == 1);

    taskRunner.Process(mainloop);

    // Make sure the tasks are executed in the order of posting.
    STRCMP_EQUAL("abc", str.c_str());
}

TEST(TaskRunner, TestMultipleThreads)
{
    std::atomic<int>         counter{0};
    otbr::TaskRunner         taskRunner;
    std::vector<std::thread> threads;

    // Increase the `counter` to 10 in separate threads.
    for (size_t i = 0; i < 10; ++i)
    {
        threads.emplace_back([&]() { taskRunner.Post([&]() { ++counter; }); });
    }

    while (counter.load() < 10)
    {
        int                   rval;
        otbr::MainloopContext mainloop;

        mainloop.mMaxFd   = -1;
        mainloop.mTimeout = {10, 0};

        FD_ZERO(&mainloop.mReadFdSet);
        FD_ZERO(&mainloop.mWriteFdSet);
        FD_ZERO(&mainloop.mErrorFdSet);

        taskRunner.Update(mainloop);
        rval = select(mainloop.mMaxFd + 1, &mainloop.mReadFdSet, &mainloop.mWriteFdSet, &mainloop.mErrorFdSet,
                      &mainloop.mTimeout);
        CHECK_EQUAL(1, rval);

        taskRunner.Process(mainloop);
    }

    for (auto &th : threads)
    {
        th.join();
    }

    CHECK_EQUAL(10, counter.load());
}

TEST(TaskRunner, TestPostAndWait)
{
    std::atomic<int>         total{0};
    std::atomic<int>         counter{0};
    otbr::TaskRunner         taskRunner;
    std::vector<std::thread> threads;

    // Increase the `counter` to 10 in separate threads and accumulate the total value.
    for (size_t i = 0; i < 10; ++i)
    {
        threads.emplace_back([&]() { total += taskRunner.PostAndWait<int>([&]() { return ++counter; }); });
    }

    while (counter.load() < 10)
    {
        int                   rval;
        otbr::MainloopContext mainloop;

        mainloop.mMaxFd   = -1;
        mainloop.mTimeout = {10, 0};

        FD_ZERO(&mainloop.mReadFdSet);
        FD_ZERO(&mainloop.mWriteFdSet);
        FD_ZERO(&mainloop.mErrorFdSet);

        taskRunner.Update(mainloop);
        rval = select(mainloop.mMaxFd + 1, &mainloop.mReadFdSet, &mainloop.mWriteFdSet, &mainloop.mErrorFdSet,
                      &mainloop.mTimeout);
        CHECK_EQUAL(1, rval);

        taskRunner.Process(mainloop);
    }

    for (auto &th : threads)
    {
        th.join();
    }

    CHECK_EQUAL(55, total);
    CHECK_EQUAL(10, counter.load());
}

TEST(TaskRunner, TestDelayedTasks)
{
    std::atomic<int>         counter{0};
    otbr::TaskRunner         taskRunner;
    std::vector<std::thread> threads;

    // Increase the `counter` to 10 in separate threads.
    for (size_t i = 0; i < 10; ++i)
    {
        threads.emplace_back([&]() { taskRunner.Post(std::chrono::milliseconds(10), [&]() { ++counter; }); });
    }

    while (counter.load() < 10)
    {
        int                   rval;
        otbr::MainloopContext mainloop;

        mainloop.mMaxFd   = -1;
        mainloop.mTimeout = {2, 0};

        FD_ZERO(&mainloop.mReadFdSet);
        FD_ZERO(&mainloop.mWriteFdSet);
        FD_ZERO(&mainloop.mErrorFdSet);

        taskRunner.Update(mainloop);
        rval = select(mainloop.mMaxFd + 1, &mainloop.mReadFdSet, &mainloop.mWriteFdSet, &mainloop.mErrorFdSet,
                      &mainloop.mTimeout);
        CHECK_TRUE(rval >= 0 || errno == EINTR);

        taskRunner.Process(mainloop);
    }

    for (auto &th : threads)
    {
        th.join();
    }

    CHECK_EQUAL(10, counter.load());
}

TEST(TaskRunner, TestDelayedTasksOrder)
{
    std::string      str;
    otbr::TaskRunner taskRunner;

    taskRunner.Post(std::chrono::milliseconds(10), [&]() { str.push_back('a'); });
    taskRunner.Post(std::chrono::milliseconds(9), [&]() { str.push_back('b'); });
    taskRunner.Post(std::chrono::milliseconds(10), [&]() { str.push_back('c'); });

    while (str.size() < 3)
    {
        int                   rval;
        otbr::MainloopContext mainloop;

        mainloop.mMaxFd   = -1;
        mainloop.mTimeout = {2, 0};

        FD_ZERO(&mainloop.mReadFdSet);
        FD_ZERO(&mainloop.mWriteFdSet);
        FD_ZERO(&mainloop.mErrorFdSet);

        taskRunner.Update(mainloop);
        rval = select(mainloop.mMaxFd + 1, &mainloop.mReadFdSet, &mainloop.mWriteFdSet, &mainloop.mErrorFdSet,
                      &mainloop.mTimeout);
        CHECK_TRUE(rval >= 0 || errno == EINTR);

        taskRunner.Process(mainloop);
    }

    // Make sure that tasks with smaller delay are executed earlier.
    STRCMP_EQUAL("bac", str.c_str());
}

TEST(TaskRunner, TestAllAPIs)
{
    std::atomic<int>         counter{0};
    otbr::TaskRunner         taskRunner;
    std::vector<std::thread> threads;

    // Increase the `counter` to 30 in separate threads.
    for (size_t i = 0; i < 10; ++i)
    {
        threads.emplace_back([&]() { taskRunner.Post([&]() { ++counter; }); });
        threads.emplace_back([&]() { taskRunner.Post(std::chrono::milliseconds(10), [&]() { ++counter; }); });
        threads.emplace_back([&]() { taskRunner.PostAndWait<int>([&]() { return ++counter; }); });
    }

    while (counter.load() < 30)
    {
        int                   rval;
        otbr::MainloopContext mainloop;

        mainloop.mMaxFd   = -1;
        mainloop.mTimeout = {2, 0};

        FD_ZERO(&mainloop.mReadFdSet);
        FD_ZERO(&mainloop.mWriteFdSet);
        FD_ZERO(&mainloop.mErrorFdSet);

        taskRunner.Update(mainloop);
        rval = select(mainloop.mMaxFd + 1, &mainloop.mReadFdSet, &mainloop.mWriteFdSet, &mainloop.mErrorFdSet,
                      &mainloop.mTimeout);
        CHECK_TRUE(rval >= 0 || errno == EINTR);

        taskRunner.Process(mainloop);
    }

    for (auto &th : threads)
    {
        th.join();
    }

    CHECK_EQUAL(30, counter.load());
}
