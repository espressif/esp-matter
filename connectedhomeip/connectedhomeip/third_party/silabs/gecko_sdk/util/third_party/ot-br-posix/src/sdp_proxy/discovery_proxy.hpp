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
 *   This file includes definition for DNS-SD Discovery Proxy.
 */

#ifndef OTBR_AGENT_DISCOVERY_PROXY_HPP_
#define OTBR_AGENT_DISCOVERY_PROXY_HPP_

#if OTBR_ENABLE_DNSSD_DISCOVERY_PROXY

#include <set>
#include <utility>

#include <stdint.h>

#include <openthread/dnssd_server.h>
#include <openthread/instance.h>

#include "common/dns_utils.hpp"
#include "mdns/mdns.hpp"
#include "ncp/ncp_openthread.hpp"

namespace otbr {
namespace Dnssd {

/**
 * This class implements the DNS-SD Discovery Proxy.
 *
 */
class DiscoveryProxy : private NonCopyable
{
public:
    /**
     * This constructor initializes the Discovery Proxy instance.
     *
     * @param[in] aNcp        A reference to the OpenThread Controller instance.
     * @param[in] aPublisher  A reference to the mDNS Publisher.
     *
     */
    explicit DiscoveryProxy(Ncp::ControllerOpenThread &aNcp, Mdns::Publisher &aPublisher);

    /**
     * This method starts the Discovery Proxy.
     *
     */
    void Start(void);

    /**
     * This method stops the Discovery Proxy.
     *
     */
    void Stop(void);

private:
    enum : uint32_t
    {
        kServiceTtlCapLimit = 10, // TTL cap limit for Discovery Proxy (in seconds).
    };

    static void        OnDiscoveryProxySubscribe(void *aContext, const char *aFullName);
    void               OnDiscoveryProxySubscribe(const char *aSubscription);
    static void        OnDiscoveryProxyUnsubscribe(void *aContext, const char *aFullName);
    void               OnDiscoveryProxyUnsubscribe(const char *aSubscription);
    int                GetServiceSubscriptionCount(const DnsNameInfo &aNameInfo) const;
    static std::string TranslateDomain(const std::string &aName, const std::string &aTargetDomain);
    void               OnServiceDiscovered(const std::string &                            aSubscription,
                                           const Mdns::Publisher::DiscoveredInstanceInfo &aInstanceInfo);
    void OnHostDiscovered(const std::string &aHostName, const Mdns::Publisher::DiscoveredHostInfo &aHostInfo);
    static uint32_t CapTtl(uint32_t aTtl);

    Ncp::ControllerOpenThread &mNcp;
    Mdns::Publisher &          mMdnsPublisher;
    uint64_t                   mSubscriberId = 0;
};

} // namespace Dnssd
} // namespace otbr

#endif // OTBR_ENABLE_DNSSD_DISCOVERY_PROXY

#endif // OTBR_AGENT_DISCOVERY_PROXY_HPP_
