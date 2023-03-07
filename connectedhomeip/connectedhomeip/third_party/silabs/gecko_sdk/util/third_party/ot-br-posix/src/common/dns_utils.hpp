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
 * This file includes DNS utilities.
 *
 */
#ifndef OTBR_COMMON_DNS_UTILS_HPP_
#define OTBR_COMMON_DNS_UTILS_HPP_

#include "common/types.hpp"

/**
 * This structure represents DNS Name information.
 *
 * @sa SplitFullDnsName
 *
 */
struct DnsNameInfo
{
    std::string mInstanceName; ///< Instance name, or empty if the DNS name is not a service instance.
    std::string mServiceName;  ///< Service name, or empty if the DNS name is not a service or service instance.
    std::string mHostName;     ///< Host name, or empty if the DNS name is not a host name.
    std::string mDomain;       ///< Domain name.

    /**
     * This method returns if the DNS name is a service instance.
     *
     * @returns Whether the DNS name is a service instance.
     *
     */
    bool IsServiceInstance(void) const { return !mInstanceName.empty(); };

    /**
     * This method returns if the DNS name is a service.
     *
     * @returns Whether the DNS name is a service.
     *
     */
    bool IsService(void) const { return !mServiceName.empty() && mInstanceName.empty(); }

    /**
     * This method returns if the DNS name is a host.
     *
     * @returns Whether the DNS name is a host.
     *
     */
    bool IsHost(void) const { return mServiceName.empty(); }
};

/**
 * This method splits a full DNS name into name components.
 *
 * @param[in] aName  The full DNS name to dissect.
 *
 * @returns A `DnsNameInfo` structure containing DNS name information.
 *
 * @sa DnsNameInfo
 *
 */
DnsNameInfo SplitFullDnsName(const std::string &aName);

/**
 * This function splits a full service name into components.
 *
 * @param[in]  aFullName  The full service name to split.
 * @param[out] aType      A reference to a string to receive the service type.
 * @param[out] aDomain    A reference to a string to receive the domain.
 *
 * @retval OTBR_ERROR_NONE          Successfully split the full service name.
 * @retval OTBR_ERROR_INVALID_ARGS  If the full service name is not valid.
 *
 */
otbrError SplitFullServiceName(const std::string &aFullName, std::string &aType, std::string &aDomain);

/**
 * This function splits a full service instance name into components.
 *
 * @param[in]  aFullName      The full service instance name to split.
 * @param[out] aInstanceName  A reference to a string to receive the instance name.
 * @param[out] aType          A reference to a string to receive the service type.
 * @param[out] aDomain        A reference to a string to receive the domain.
 *
 * @retval OTBR_ERROR_NONE          Successfully split the full service instance name.
 * @retval OTBR_ERROR_INVALID_ARGS  If the full service instance name is not valid.
 *
 */
otbrError SplitFullServiceInstanceName(const std::string &aFullName,
                                       std::string &      aInstanceName,
                                       std::string &      aType,
                                       std::string &      aDomain);

/**
 * This function splits a full host name into components.
 *
 * @param[in]  aFullName  The full host name to split.
 * @param[out] aHostName  A reference to a string to receive the host name.
 * @param[out] aDomain    A reference to a string to receive the domain.
 *
 * @retval OTBR_ERROR_NONE          Successfully split the full host name.
 * @retval OTBR_ERROR_INVALID_ARGS  If the full host name is not valid.
 *
 */
otbrError SplitFullHostName(const std::string &aFullName, std::string &aHostName, std::string &aDomain);

#endif // OTBR_COMMON_DNS_UTILS_HPP_
