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
 *   This file includes type definitions for OTBR-REST.
 */

#ifndef OTBR_REST_TYPES_HPP_
#define OTBR_REST_TYPES_HPP_

#include <chrono>
#include <string>
#include <vector>

#include "openthread/netdiag.h"

using std::chrono::steady_clock;

namespace otbr {
namespace rest {

enum class HttpMethod : std::uint8_t
{
    kDelete  = 0, ///< DELETE
    kGet     = 1, ///< GET
    kHead    = 2, ///< HEAD
    kPost    = 3, ///< POST
    kPut     = 4, ///< PUT
    kOptions = 6, ///< OPTIONS

};

enum class HttpStatusCode : std::uint16_t
{
    kStatusOk                  = 200,
    kStatusResourceNotFound    = 404,
    kStatusMethodNotAllowed    = 405,
    kStatusRequestTimeout      = 408,
    kStatusInternalServerError = 500,
};

enum class PostError : std::uint8_t
{
    kPostErrorNone  = 0, ///< No error
    kPostBadRequest = 1, ///< Bad request for post
    kPostSetFail    = 2, ///< Fail when set value
};

enum class ConnectionState : std::uint8_t
{
    kInit          = 0, ///< Init
    kReadWait      = 1, ///< Wait to read
    kReadTimeout   = 2, ///< Reach read timeout
    kCallbackWait  = 3, ///< Wait for callback
    kWriteWait     = 4, ///< Wait for write
    kWriteTimeout  = 5, ///< Reach write timeout
    kInternalError = 6, ///< Occur internal call error
    kComplete      = 7, ///< No longer need to be processed

};
struct NodeInfo
{
    uint32_t       mRole;
    uint32_t       mNumOfRouter;
    uint16_t       mRloc16;
    const uint8_t *mExtPanId;
    const uint8_t *mExtAddress;
    otIp6Address   mRlocAddress;
    otLeaderData   mLeaderData;
    std::string    mNetworkName;
};

struct DiagInfo
{
    steady_clock::time_point      mStartTime;
    std::vector<otNetworkDiagTlv> mDiagContent;
};

} // namespace rest
} // namespace otbr

#endif // OTBR_REST_TYPES_HPP_
