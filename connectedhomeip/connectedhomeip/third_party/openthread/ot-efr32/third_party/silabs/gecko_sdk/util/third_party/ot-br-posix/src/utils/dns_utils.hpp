/*
 *  Copyright (c) 2017-2021, The OpenThread Authors.
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
 * This file includes definition for DNS utilities.
 */

#ifndef OTBR_UTILS_DNS_UTILS_HPP_
#define OTBR_UTILS_DNS_UTILS_HPP_

#include "openthread-br/config.h"

#include <string>

namespace otbr {

namespace DnsUtils {

/**
 * This function unescapes a DNS Service Instance name according to "DNS Name Escaping".
 *
 * @sa  "Notes on DNS Name Escaping" at
 *      https://opensource.apple.com/source/mDNSResponder/mDNSResponder-1310.140.1/mDNSShared/dns_sd.h.auto.html.
 *
 * @param[in] aName  The DNS Service Instance name to unescape.
 *
 * @returns  The unescaped DNS Service Instance name.
 *
 */
std::string UnescapeInstanceName(const std::string &aName);

/**
 * This function checks a given host name for sanity.
 *
 * Check criteria:
 *      The host name must ends with dot.
 *
 * @param[in] aHostName  The host name to check.
 *
 */
void CheckHostnameSanity(const std::string &aHostName);

/**
 * This function checks a given service name for sanity.
 *
 * Check criteria:
 *      The service name must contain exactly one dot.
 *      The service name must not end with dot.
 *
 * @param[in] aServiceName  The service name to check.
 *
 */
void CheckServiceNameSanity(const std::string &aServiceName);

} // namespace DnsUtils
} // namespace otbr
#endif // OTBR_UTILS_DNS_UTILS_HPP_
