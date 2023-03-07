/*
 *    Copyright (c) 2020, The OpenThread Authors.
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
 *   This file includes definition for ICMPv6 Neighbor Advertisement (ND) proxy management.
 */

#ifndef ND_PROXY_HPP_
#define ND_PROXY_HPP_

#if OTBR_ENABLE_DUA_ROUTING

#ifdef __APPLE__
#define __APPLE_USE_RFC_3542
#endif

#include <inttypes.h>
#include <libnetfilter_queue/libnetfilter_queue.h>
#include <map>
#include <netinet/in.h>
#include <set>
#include <string>
#include <utility>

#include <openthread/backbone_router_ftd.h>

#include "common/code_utils.hpp"
#include "common/mainloop.hpp"
#include "common/types.hpp"
#include "ncp/ncp_openthread.hpp"

namespace otbr {
namespace BackboneRouter {

/**
 * @addtogroup border-router-bbr
 *
 * @brief
 *   This module includes definition for ND Proxy manager.
 *
 * @{
 */

/**
 * This class implements ND Proxy manager.
 *
 */
class NdProxyManager : public MainloopProcessor, private NonCopyable
{
public:
    /**
     * This constructor initializes a NdProxyManager instance.
     *
     */
    explicit NdProxyManager(otbr::Ncp::ControllerOpenThread &aNcp, std::string aBackboneInterfaceName)
        : mNcp(aNcp)
        , mBackboneInterfaceName(std::move(aBackboneInterfaceName))
        , mIcmp6RawSock(-1)
        , mUnicastNsQueueSock(-1)
        , mNfqHandler(nullptr)
        , mNfqQueueHandler(nullptr)
    {
    }

    /**
     * This method initializes a ND Proxy manager instance.
     *
     */
    void Init(void);

    /**
     * This method enables the ND Proxy manager.
     *
     * @param[in] aDomainPrefix  The Domain Prefix.
     *
     */
    void Enable(const Ip6Prefix &aDomainPrefix);

    /**
     * This method disables the ND Proxy manager.
     *
     */
    void Disable(void);

    void Update(MainloopContext &aMainloop) override;
    void Process(const MainloopContext &aMainloop) override;

    /**
     * This method handles a Backbone Router ND Proxy event.
     *
     * @param[in] aEvent  The Backbone Router ND Proxy event type.
     * @param[in] aDua    The Domain Unicast Address of the ND Proxy, or `nullptr` if @p `aEvent` is
     *                    `OT_BACKBONE_ROUTER_NDPROXY_CLEARED`.
     *
     */
    void HandleBackboneRouterNdProxyEvent(otBackboneRouterNdProxyEvent aEvent, const otIp6Address *aDua);

    /**
     * This method returns if the ND Proxy manager is enabled.
     *
     * @returns If the ND Proxy manager is enabled;
     *
     */
    bool IsEnabled(void) const { return mIcmp6RawSock >= 0; }

private:
    enum
    {
        kMaxICMP6PacketSize = 1500, ///< Max size of an ICMP6 packet in bytes.
    };

    void       SendNeighborAdvertisement(const Ip6Address &aTarget, const Ip6Address &aDst);
    otbrError  UpdateMacAddress(void);
    otbrError  InitIcmp6RawSocket(void);
    void       FiniIcmp6RawSocket(void);
    otbrError  InitNetfilterQueue(void);
    void       FiniNetfilterQueue(void);
    void       ProcessMulticastNeighborSolicition(void);
    void       ProcessUnicastNeighborSolicition(void);
    void       JoinSolicitedNodeMulticastGroup(const Ip6Address &aTarget) const;
    void       LeaveSolicitedNodeMulticastGroup(const Ip6Address &aTarget) const;
    static int HandleNetfilterQueue(struct nfq_q_handle *aNfQueueHandler,
                                    struct nfgenmsg *    aNfMsg,
                                    struct nfq_data *    aNfData,
                                    void *               aContext);
    int HandleNetfilterQueue(struct nfq_q_handle *aNfQueueHandler, struct nfgenmsg *aNfMsg, struct nfq_data *aNfData);

    otbr::Ncp::ControllerOpenThread &mNcp;
    std::string                      mBackboneInterfaceName;
    std::set<Ip6Address>             mNdProxySet;
    uint32_t                         mBackboneIfIndex;
    int                              mIcmp6RawSock;
    int                              mUnicastNsQueueSock;
    struct nfq_handle *              mNfqHandler;      ///< A pointer to an NFQUEUE handler.
    struct nfq_q_handle *            mNfqQueueHandler; ///< A pointer to a newly created queue.
    MacAddress                       mMacAddress;
    Ip6Prefix                        mDomainPrefix;
};

/**
 * @}
 */

} // namespace BackboneRouter
} // namespace otbr

#endif // OTBR_ENABLE_DUA_ROUTING
#endif // ND_PROXY_HPP_
