/*
 *  Copyright (c) 2020, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#include "rest/connection.hpp"

#include <cerrno>

#include <assert.h>

#include <sys/socket.h>
#include <sys/time.h>

using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::seconds;
using std::chrono::steady_clock;

namespace otbr {
namespace rest {

// The timeout (in microseconds) since a connection is in wait callback state
static const uint32_t kCallbackTimeout = 10000000;

// The time interval (in microseconds) for checking again if there is a connection need callback.
static const uint32_t kCallbackCheckInterval = 500000;

// The timeout (in microseconds) since a connection is in wait write state
static const uint32_t kWriteTimeout = 10000000;

// The timeout (in microseconds) since a connection is in wait read state
static const uint32_t kReadTimeout = 1000000;

Connection::Connection(steady_clock::time_point aStartTime, Resource *aResource, int aFd)
    : mTimeStamp(aStartTime)
    , mFd(aFd)
    , mState(ConnectionState::kInit)
    , mParser(&mRequest)
    , mResource(aResource)
{
}

Connection::~Connection(void)
{
    Disconnect();
}

void Connection::Init(void)
{
    mParser.Init();
}

void Connection::UpdateReadFdSet(fd_set &aReadFdSet, int &aMaxFd) const
{
    if (mState == ConnectionState::kReadWait || mState == ConnectionState::kInit)
    {
        FD_SET(mFd, &aReadFdSet);
        aMaxFd = aMaxFd < mFd ? mFd : aMaxFd;
    }
}

void Connection::UpdateWriteFdSet(fd_set &aWriteFdSet, int &aMaxFd) const
{
    if (mState == ConnectionState::kWriteWait)
    {
        FD_SET(mFd, &aWriteFdSet);
        aMaxFd = aMaxFd < mFd ? mFd : aMaxFd;
    }
}

void Connection::UpdateTimeout(timeval &aTimeout) const
{
    struct timeval timeout;
    uint32_t       timeoutLen = kReadTimeout;
    auto           duration   = duration_cast<microseconds>(steady_clock::now() - mTimeStamp).count();

    switch (mState)
    {
    case ConnectionState::kReadWait:
        timeoutLen = kReadTimeout;
        break;
    case ConnectionState::kCallbackWait:
        timeoutLen = kCallbackCheckInterval;
        break;
    case ConnectionState::kWriteWait:
        timeoutLen = kWriteTimeout;
        break;
    case ConnectionState::kComplete:
        timeoutLen = 0;
        break;
    default:
        break;
    }

    if (duration <= timeoutLen)
    {
        timeout.tv_sec  = 0;
        timeout.tv_usec = timeoutLen - duration;
    }
    else
    {
        timeout.tv_sec  = 0;
        timeout.tv_usec = 0;
    }

    if (timercmp(&timeout, &aTimeout, <))
    {
        aTimeout = timeout;
    }
}

void Connection::Update(MainloopContext &aMainloop)
{
    UpdateTimeout(aMainloop.mTimeout);
    UpdateReadFdSet(aMainloop.mReadFdSet, aMainloop.mMaxFd);
    UpdateWriteFdSet(aMainloop.mWriteFdSet, aMainloop.mMaxFd);
}

void Connection::Disconnect(void)
{
    mState = ConnectionState::kComplete;

    if (mFd != -1)
    {
        close(mFd);
        mFd = -1;
    }
}

void Connection::Process(const MainloopContext &aMainloop)
{
    otbrError error = OTBR_ERROR_NONE;

    switch (mState)
    {
    // Initial state, directly read for the first time.
    case ConnectionState::kInit:
    case ConnectionState::kReadWait:
        ProcessWaitRead(aMainloop.mReadFdSet);
        break;
    case ConnectionState::kCallbackWait:
        //  Wait for Callback process.
        ProcessWaitCallback();
        break;
    case ConnectionState::kWriteWait:
        ProcessWaitWrite(aMainloop.mWriteFdSet);
        break;
    default:
        assert(false);
    }

    if (error != OTBR_ERROR_NONE)
    {
        Disconnect();
    }
}

void Connection::ProcessWaitRead(const fd_set &aReadFdSet)
{
    otbrError error    = OTBR_ERROR_NONE;
    int32_t   received = 0, err;
    char      buf[2048];
    auto      duration = duration_cast<microseconds>(steady_clock::now() - mTimeStamp).count();

    // Reach a read timeout, will send response about this timeout later.
    VerifyOrExit(duration <= kReadTimeout, error = OTBR_ERROR_REST);

    // It will succeed either fd is set or it is in kInit state.
    VerifyOrExit(FD_ISSET(mFd, &aReadFdSet) || mState == ConnectionState::kInit);

    do
    {
        mState   = ConnectionState::kReadWait;
        received = read(mFd, buf, sizeof(buf));
        err      = errno;
        if (received > 0)
        {
            mParser.Process(buf, received);
        }
    } while ((received > 0 && !mRequest.IsComplete()) || err == EINTR);

    if (mRequest.IsComplete())
    {
        Handle();
    }

    // Check first failure situation: received == 0 (indicate another side at least has closes its write side )
    // and at the same time, the request has not been parsed completely.
    VerifyOrExit(received != 0 || mRequest.IsComplete(), error = OTBR_ERROR_REST);

    // Check second  failure situation : received = -1 error(indicates that our system call read raise an error )
    // then try to send back a response that there is an internal error.
    VerifyOrExit(received > 0 || (received == -1 && (err == EAGAIN || err == EWOULDBLOCK)), error = OTBR_ERROR_REST);

exit:
    if (error != OTBR_ERROR_NONE)
    {
        if (received < 0)
        {
            mResource->ErrorHandler(mResponse, HttpStatusCode::kStatusInternalServerError);
            Write();
        }
        else
        {
            mResource->ErrorHandler(mResponse, HttpStatusCode::kStatusRequestTimeout);
            Write();
        }
    }
}

void Connection::Handle(void)
{
    otbrError error = OTBR_ERROR_NONE;

    // Try to close server read side here, because we have started to handle the request and no longler read from
    // socket.
    VerifyOrExit((shutdown(mFd, SHUT_RD) == 0), error = OTBR_ERROR_REST);

    mResource->Handle(mRequest, mResponse);

    if (mResponse.NeedCallback())
    {
        mState     = ConnectionState::kCallbackWait;
        mTimeStamp = steady_clock::now();
    }
    else
    {
        // Normal Write back process.
        Write();
    }

exit:

    if (error != OTBR_ERROR_NONE)
    {
        mResource->ErrorHandler(mResponse, HttpStatusCode::kStatusInternalServerError);
        Write();
    }
}

void Connection::ProcessWaitCallback(void)
{
    auto duration = duration_cast<microseconds>(steady_clock::now() - mTimeStamp).count();

    mResource->HandleCallback(mRequest, mResponse);

    if (mResponse.IsComplete())
    {
        Write();
    }
    else
    {
        if (duration >= kCallbackTimeout)
        {
            mResource->ErrorHandler(mResponse, HttpStatusCode::kStatusInternalServerError);
            Write();
        }
    }
}

void Connection::ProcessWaitWrite(const fd_set &aWriteFdSet)
{
    auto duration = duration_cast<microseconds>(steady_clock::now() - mTimeStamp).count();

    if (duration <= kWriteTimeout)
    {
        if (FD_ISSET(mFd, &aWriteFdSet))
        {
            Write();
        }
    }
    else
    {
        Disconnect();
    }
}

void Connection::Write(void)
{
    otbrError   error = OTBR_ERROR_NONE;
    std::string errorCode;
    int32_t     sendLength;
    int32_t     err;

    if (mState != ConnectionState::kWriteWait)
    {
        // Change its state when try write for the first time.
        mState        = ConnectionState::kWriteWait;
        mTimeStamp    = steady_clock::now();
        mWriteContent = mResponse.Serialize();
    }

    // Check we do have something to write.
    VerifyOrExit(mWriteContent.size() > 0, error = OTBR_ERROR_REST);

    sendLength = write(mFd, mWriteContent.c_str(), mWriteContent.size());
    err        = errno;

    // Write successfully
    if (sendLength == static_cast<int32_t>(mWriteContent.size()))
    {
        // Normal Exit
        Disconnect();
    }
    else if (sendLength > 0)
    {
        // Partly write
        mWriteContent = mWriteContent.substr(sendLength);
    }
    else
    {
        if (errno == EINTR)
        {
            // Try again
            Write();
        }
        else
        {
            // There is an error when we write, if this, we directly disconnect this connection.
            VerifyOrExit(err == EAGAIN || err == EWOULDBLOCK, error = OTBR_ERROR_REST);
        }
    }

exit:
    if (error != OTBR_ERROR_NONE)
    {
        Disconnect();
    }
}

bool Connection::IsComplete() const
{
    return mState == ConnectionState::kComplete;
}

} // namespace rest
} // namespace otbr
