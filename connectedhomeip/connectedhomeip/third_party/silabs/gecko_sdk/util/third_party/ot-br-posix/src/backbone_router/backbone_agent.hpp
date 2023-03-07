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
 *   This file includes definition for Thread Backbone agent.
 */

#ifndef BACKBONE_ROUTER_BACKBONE_AGENT_HPP_
#define BACKBONE_ROUTER_BACKBONE_AGENT_HPP_

#if OTBR_ENABLE_BACKBONE_ROUTER

#include <openthread/backbone_router_ftd.h>

#include "backbone_router/dua_routing_manager.hpp"
#include "backbone_router/nd_proxy.hpp"
#include "common/code_utils.hpp"
#include "ncp/ncp_openthread.hpp"

namespace otbr {
namespace BackboneRouter {

/**
 * @addtogroup border-router-backbone
 *
 * @brief
 *   This module includes definition for Thread Backbone agent.
 *
 * @{
 */

/**
 * This class implements Thread Backbone agent functionality.
 *
 */
class BackboneAgent : private NonCopyable
{
public:
    static constexpr uint16_t kBackboneUdpPort = 61631; ///< The BBR port.

    /**
     * This constructor intiializes the `BackboneAgent` instance.
     *
     * @param[in] aNcp  The Thread instance.
     *
     */
    BackboneAgent(otbr::Ncp::ControllerOpenThread &aNcp,
                  std::string                      aInterfaceName,
                  std::string                      aBackboneInterfaceName);

    /**
     * This method initializes the Backbone agent.
     *
     */
    void Init(void);

private:
    void        OnBecomePrimary(void);
    void        OnResignPrimary(void);
    bool        IsPrimary(void) const { return mBackboneRouterState == OT_BACKBONE_ROUTER_STATE_PRIMARY; }
    void        HandleThreadStateChanged(otChangedFlags aFlags);
    void        HandleBackboneRouterState(void);
    static void HandleBackboneRouterDomainPrefixEvent(void *                            aContext,
                                                      otBackboneRouterDomainPrefixEvent aEvent,
                                                      const otIp6Prefix *               aDomainPrefix);
    void        HandleBackboneRouterDomainPrefixEvent(otBackboneRouterDomainPrefixEvent aEvent,
                                                      const otIp6Prefix *               aDomainPrefix);
#if OTBR_ENABLE_DUA_ROUTING
    static void HandleBackboneRouterNdProxyEvent(void *                       aContext,
                                                 otBackboneRouterNdProxyEvent aEvent,
                                                 const otIp6Address *         aAddress);
    void        HandleBackboneRouterNdProxyEvent(otBackboneRouterNdProxyEvent aEvent, const otIp6Address *aAddress);
#endif

    static const char *StateToString(otBackboneRouterState aState);

    otbr::Ncp::ControllerOpenThread &mNcp;
    otBackboneRouterState            mBackboneRouterState;
    Ip6Prefix                        mDomainPrefix;
#if OTBR_ENABLE_DUA_ROUTING
    NdProxyManager    mNdProxyManager;
    DuaRoutingManager mDuaRoutingManager;
#endif
};

/**
 * @}
 */

} // namespace BackboneRouter
} // namespace otbr

#endif // OTBR_ENABLE_BACKBONE_ROUTER

#endif // BACKBONE_ROUTER_BACKBONE_AGENT_HPP_
