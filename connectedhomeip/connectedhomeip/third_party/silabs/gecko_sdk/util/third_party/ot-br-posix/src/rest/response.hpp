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

/**
 * @file
 *   This file includes response definition for RESTful HTTP server.
 */

#ifndef OTBR_REST_RESPONSE_HPP_
#define OTBR_REST_RESPONSE_HPP_

#include <chrono>
#include <string>
#include <vector>

#include "rest/types.hpp"

using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::seconds;
using std::chrono::steady_clock;

namespace otbr {
namespace rest {

/**
 * This class implements a response class for OTBR_REST, it could be manipulated by connection instance and resource
 * handler.
 *
 */
class Response
{
public:
    /**
     * The constructor to initialize a response instance.
     *
     *
     */
    Response(void);

    /**
     * This method set the response body.
     *
     * @param[in] aBody  A string to be set as response body.
     *
     */
    void SetBody(std::string &aBody);

    /**
     * This method return a string contains the body field of this response.
     *
     * @returns A string containing the body field.
     */
    std::string GetBody(void) const;

    /**
     * This method set the response code.
     *
     * @param[in] aCode  A string representing response code such as "404 not found".
     *
     */
    void SetResponsCode(std::string &aCode);

    /**
     * This method labels the response as need callback.
     *
     *
     */
    void SetCallback(void);

    /**
     * This method checks whether this response need to be processed by callback handler later.
     *
     * @returns A bool value indicates whether this response need to be processed by callback handler later.
     */
    bool NeedCallback(void);

    /**
     * This method labels the response as complete which means all fields has been successfully set.
     *
     */
    void SetComplete();

    /**
     * This method checks whether this response is ready to be written to buffer.
     *
     * @returns A bool value indicates whether this response is ready to be written to buffer..
     */
    bool IsComplete();

    /**
     * This method is used to set a timestamp. when a callback is needed and this field tells callback handler when to
     * collect all the data and form the response.
     *
     * @param[in] aStartTime  A timestamp indicates when the response start to wait for callback.
     */
    void SetStartTime(steady_clock::time_point aStartTime);

    /**
     * This method returns a timestamp of start time.
     *
     * @returns A timepoint object indicates start time.
     */
    steady_clock::time_point GetStartTime() const;

    /**
     * This method serialize a response to a string that could be sent by socket later.
     *
     * @returns A string contains status line, headers and body of a response.
     */
    std::string Serialize(void) const;

private:
    bool                     mCallback;
    std::vector<std::string> mHeaderField;
    std::vector<std::string> mHeaderValue;
    std::string              mCode;
    std::string              mProtocol;
    std::string              mBody;
    bool                     mComplete;
    steady_clock::time_point mStartTime;
};

} // namespace rest
} // namespace otbr

#endif // OTBR_REST_RESPONSE_HPP_
