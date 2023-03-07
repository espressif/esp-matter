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
 *   This file includes JSON formatter definition for RESTful HTTP server.
 */

#ifndef OTBR_REST_JSON_HPP_
#define OTBR_REST_JSON_HPP_

#include "openthread/link.h"
#include "openthread/thread_ftd.h"

#include "rest/types.hpp"
#include "utils/hex.hpp"

namespace otbr {
namespace rest {

/**
 * The functions within this namespace provides a tranformation from an object/string/number to a serialized Json
 * string.
 *
 */
namespace Json {

/**
 * This method formats an integer to a Json number and serialize it to a string.
 *
 * @param[in] aNumber  An integer need to be format.
 *
 * @returns A string of serialized Json number.
 *
 */
std::string Number2JsonString(const uint32_t &aNumber);

/**
 * This method formats a Bytes array to a Json string and serialize it to a string.
 *
 * @param[in] aBytes  A Bytes array representing a hex number.
 *
 * @returns A string of serialized Json string.
 *
 */
std::string Bytes2HexJsonString(const uint8_t *aBytes, uint8_t aLength);

/**
 * This method formats a C string to a Json string and serialize it to a string.
 *
 * @param[in] aCString  A char pointer pointing to a C string.
 *
 * @returns A string of serialized Json string.
 *
 */
std::string CString2JsonString(const char *aCString);

/**
 * This method formats a string to a Json string and serialize it to a string.
 *
 * @param[in] aString  A string.
 *
 * @returns A string of serialized Json string.
 *
 */
std::string String2JsonString(const std::string &aString);

/**
 * This method formats a Node object to a Json object and serialize it to a string.
 *
 * @param[in] aNode  A Node object.
 *
 * @returns A string of serialized Json object.
 *
 */
std::string Node2JsonString(const NodeInfo &aNode);

/**
 * This method formats a vector of diagnostic objects to a Json array and serialize it to a string.
 *
 * @param[in] aDiagSet  A vector of diagnostic objects.
 *
 * @returns A string of serialized Json array.
 *
 */
std::string Diag2JsonString(const std::vector<std::vector<otNetworkDiagTlv>> &aDiagSet);

/**
 * This method formats an Ipv6Address to a Json string and serialize it to a string.
 *
 * @param[in] aAddress  An Ip6Address object.
 *
 * @returns A string of serialized Json string.
 *
 */
std::string IpAddr2JsonString(const otIp6Address &aAddress);

/**
 * This method formats a LinkModeConfig object to a Json object and serialize it to a string.
 *
 * @param[in] aMode  A LinkModeConfig object.
 *
 * @returns A string of serialized Json object.
 *
 */
std::string Mode2JsonString(const otLinkModeConfig &aMode);

/**
 * This method formats a Connectivity object to a Json object and serialize it to a string.
 *
 * @param[in] aConnectivity  A Connectivity object.
 *
 * @returns A string of serialized Json object.
 *
 */
std::string Connectivity2JsonString(const otNetworkDiagConnectivity &aConnectivity);

/**
 * This method formats a Route object to a Json object and serialize it to a string.
 *
 * @param[in] aRoute  A Route object.
 *
 * @returns A string of serialized Json object.
 *
 */
std::string Route2JsonString(const otNetworkDiagRoute &aRoute);

/**
 * This method formats a RouteData object to a Json object and serialize it to a string.
 *
 * @param[in] aRouteData  A RouteData object.
 *
 * @returns A string of serialized Json object.
 *
 */
std::string RouteData2JsonString(const otNetworkDiagRouteData &aRouteData);

/**
 * This method formats a LeaderData object to a Json object and serialize it to a string.
 *
 * @param[in] aLeaderData  A LeaderData object.
 *
 * @returns A string of serialized Json object.
 *
 */
std::string LeaderData2JsonString(const otLeaderData &aLeaderData);

/**
 * This method formats a MacCounters object to a Json object and serialize it to a string.
 *
 * @param[in] aMacCounters  A MacCounters object.
 *
 * @returns A string of serialized Json object.
 *
 */
std::string MacCounters2JsonString(const otNetworkDiagMacCounters &aMacCounters);

/**
 * This method formats a ChildEntry object to a Json object and serialize it to a string.
 *
 * @param[in] aChildEntry  A ChildEntry object.
 *
 * @returns A string of serialized Json object.
 *
 */
std::string ChildTableEntry2JsonString(const otNetworkDiagChildEntry &aChildEntry);

/**
 * This method formats an error code and an error message to a Json object and serialize it to a string.
 *
 * @param[in] aErrorCode     An enum HttpStatusCode  such as '404'.
 * @param[in] aErrorMessage  Error message such as '404 Not Found'.
 *
 * @returns A string of serialized Json object.
 *
 */
std::string Error2JsonString(HttpStatusCode aErrorCode, std::string aErrorMessage);

}; // namespace Json

} // namespace rest
} // namespace otbr

#endif // OTBR_REST_JSON_HPP_
