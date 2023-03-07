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

#include "rest/response.hpp"

#include <stdio.h>

#define OT_REST_RESPONSE_CONTENT_TYPE_JSON "application/json"
#define OT_REST_RESPONSE_ACCESS_CONTROL_ALLOW_ORIGIN "*"
#define OT_REST_RESPONSE_ACCESS_CONTROL_ALLOW_HEADERS                                                              \
    "Access-Control-Allow-Headers, Origin,Accept, X-Requested-With, Content-Type, Access-Control-Request-Method, " \
    "Access-Control-Request-Headers"
#define OT_REST_RESPONSE_ACCESS_CONTROL_ALLOW_METHOD "GET"

namespace otbr {
namespace rest {

Response::Response(void)
    : mCallback(false)
    , mComplete(false)
{
    // HTTP protocol
    mProtocol = "HTTP/1.1 ";

    // Pre-defined headers
    mHeaderField.push_back("Content-Type");
    mHeaderValue.push_back(OT_REST_RESPONSE_CONTENT_TYPE_JSON);

    mHeaderField.push_back("Access-Control-Allow-Origin");
    mHeaderValue.push_back(OT_REST_RESPONSE_ACCESS_CONTROL_ALLOW_ORIGIN);

    mHeaderField.push_back("Access-Control-Allow-Methods");
    mHeaderValue.push_back(OT_REST_RESPONSE_ACCESS_CONTROL_ALLOW_METHOD);

    mHeaderField.push_back("Access-Control-Allow-Headers");
    mHeaderValue.push_back(OT_REST_RESPONSE_ACCESS_CONTROL_ALLOW_HEADERS);
}

void Response::SetComplete()
{
    mComplete = true;
}

void Response::SetStartTime(steady_clock::time_point aStartTime)
{
    mStartTime = aStartTime;
}

steady_clock::time_point Response::GetStartTime() const
{
    return mStartTime;
}

bool Response::IsComplete()
{
    return mComplete == true;
}

void Response::SetResponsCode(std::string &aCode)
{
    mCode = aCode;
}

void Response::SetCallback(void)
{
    mCallback = true;
}

void Response::SetBody(std::string &aBody)
{
    mBody = aBody;
}

std::string Response::GetBody(void) const
{
    return mBody;
}

bool Response::NeedCallback(void)
{
    return mCallback;
}

std::string Response::Serialize(void) const
{
    size_t      index;
    std::string spacer = "\r\n";
    std::string ret(mProtocol + " " + mCode);

    for (index = 0; index < mHeaderField.size(); index++)
    {
        ret += (spacer + mHeaderField[index] + ": " + mHeaderValue[index]);
    }
    ret += spacer + "Content-Length: " + std::to_string(mBody.size());
    ret += (spacer + spacer + mBody);

    return ret;
}

} // namespace rest
} // namespace otbr
