/*
 *    Copyright (c) 2017, The OpenThread Authors.
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
 * This file includes definitions for time functions.
 */

#ifndef OTBR_COMMON_TIME_HPP_
#define OTBR_COMMON_TIME_HPP_

#include "openthread-br/config.h"

#include <chrono>

#include <stdint.h>

#include <sys/time.h>

namespace otbr {

using Seconds      = std::chrono::seconds;
using Milliseconds = std::chrono::milliseconds;
using Microseconds = std::chrono::microseconds;
using Clock        = std::chrono::steady_clock;
using Timepoint    = Clock::time_point;

template <class D> D FromTimeval(const timeval &aTime)
{
    return std::chrono::duration_cast<D>(Microseconds{aTime.tv_usec}) +
           std::chrono::duration_cast<D>(Seconds{aTime.tv_sec});
}

template <class D> timeval ToTimeval(const D &aDuration)
{
    timeval      ret;
    const size_t kMicrosecondsPeriod = 1000000;
    auto         microseconds        = std::chrono::duration_cast<Microseconds>(aDuration).count();

    ret.tv_sec  = microseconds / kMicrosecondsPeriod;
    ret.tv_usec = microseconds % kMicrosecondsPeriod;

    return ret;
}

} // namespace otbr

#endif // OTBR_COMMON_TIME_HPP_
