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
 *   The file implements the Thread Backbone agent.
 */

#define OTBR_LOG_TAG "BBA"

#include "backbone_router/backbone_agent.hpp"

#if OTBR_ENABLE_BACKBONE_ROUTER

#include <assert.h>
#include <net/if.h>

#include <openthread/backbone_router_ftd.h>

#include "common/code_utils.hpp"

namespace otbr {
namespace BackboneRouter {

BackboneAgent::BackboneAgent(otbr::Ncp::ControllerOpenThread &aNcp,
                             std::string                      aInterfaceName,
                             std::string                      aBackboneInterfaceName)
    : mNcp(aNcp)
    , mBackboneRouterState(OT_BACKBONE_ROUTER_STATE_DISABLED)
#if OTBR_ENABLE_DUA_ROUTING
    , mNdProxyManager(aNcp, aBackboneInterfaceName)
    , mDuaRoutingManager(aInterfaceName, aBackboneInterfaceName)
#endif
{
    OTBR_UNUSED_VARIABLE(aInterfaceName);
    OTBR_UNUSED_VARIABLE(aBackboneInterfaceName);
}

void BackboneAgent::Init(void)
{
    mNcp.AddThreadStateChangedCallback([this](otChangedFlags aFlags) { HandleThreadStateChanged(aFlags); });
    otBackboneRouterSetDomainPrefixCallback(mNcp.GetInstance(), &BackboneAgent::HandleBackboneRouterDomainPrefixEvent,
                                            this);
#if OTBR_ENABLE_DUA_ROUTING
    otBackboneRouterSetNdProxyCallback(mNcp.GetInstance(), &BackboneAgent::HandleBackboneRouterNdProxyEvent, this);
    mNdProxyManager.Init();
#endif

    otBackboneRouterSetEnabled(mNcp.GetInstance(), /* aEnabled */ true);
}

void BackboneAgent::HandleThreadStateChanged(otChangedFlags aFlags)
{
    if (aFlags & OT_CHANGED_THREAD_BACKBONE_ROUTER_STATE)
    {
        HandleBackboneRouterState();
    }
}

void BackboneAgent::HandleBackboneRouterState(void)
{
    otBackboneRouterState state      = otBackboneRouterGetState(mNcp.GetInstance());
    bool                  wasPrimary = (mBackboneRouterState == OT_BACKBONE_ROUTER_STATE_PRIMARY);

    otbrLogDebug("BackboneAgent: HandleBackboneRouterState: state=%d, mBackboneRouterState=%d", state,
                 mBackboneRouterState);
    VerifyOrExit(mBackboneRouterState != state);

    mBackboneRouterState = state;

    if (IsPrimary())
    {
        OnBecomePrimary();
    }
    else if (wasPrimary)
    {
        OnResignPrimary();
    }

exit:
    return;
}

void BackboneAgent::OnBecomePrimary(void)
{
    otbrLogNotice("BackboneAgent: Backbone Router becomes Primary!");

#if OTBR_ENABLE_DUA_ROUTING
    if (mDomainPrefix.IsValid())
    {
        mDuaRoutingManager.Enable(mDomainPrefix);
        mNdProxyManager.Enable(mDomainPrefix);
    }
#endif
}

void BackboneAgent::OnResignPrimary(void)
{
    otbrLogNotice("BackboneAgent: Backbone Router resigns Primary to %s!", StateToString(mBackboneRouterState));

#if OTBR_ENABLE_DUA_ROUTING
    mDuaRoutingManager.Disable();
    mNdProxyManager.Disable();
#endif
}

const char *BackboneAgent::StateToString(otBackboneRouterState aState)
{
    const char *ret = "Unknown";

    switch (aState)
    {
    case OT_BACKBONE_ROUTER_STATE_DISABLED:
        ret = "Disabled";
        break;
    case OT_BACKBONE_ROUTER_STATE_SECONDARY:
        ret = "Secondary";
        break;
    case OT_BACKBONE_ROUTER_STATE_PRIMARY:
        ret = "Primary";
        break;
    }

    return ret;
}

void BackboneAgent::HandleBackboneRouterDomainPrefixEvent(void *                            aContext,
                                                          otBackboneRouterDomainPrefixEvent aEvent,
                                                          const otIp6Prefix *               aDomainPrefix)
{
    static_cast<BackboneAgent *>(aContext)->HandleBackboneRouterDomainPrefixEvent(aEvent, aDomainPrefix);
}

void BackboneAgent::HandleBackboneRouterDomainPrefixEvent(otBackboneRouterDomainPrefixEvent aEvent,
                                                          const otIp6Prefix *               aDomainPrefix)
{
    if (aEvent == OT_BACKBONE_ROUTER_DOMAIN_PREFIX_REMOVED)
    {
        mDomainPrefix.Clear();
    }
    else
    {
        assert(aDomainPrefix != nullptr);
        mDomainPrefix.Set(*aDomainPrefix);
        assert(mDomainPrefix.IsValid());
    }

    VerifyOrExit(IsPrimary() && aEvent != OT_BACKBONE_ROUTER_DOMAIN_PREFIX_REMOVED);

#if OTBR_ENABLE_DUA_ROUTING
    mDuaRoutingManager.Disable();
    mNdProxyManager.Disable();

    mDuaRoutingManager.Enable(mDomainPrefix);
    mNdProxyManager.Enable(mDomainPrefix);
#endif

exit:
    return;
}

#if OTBR_ENABLE_DUA_ROUTING
void BackboneAgent::HandleBackboneRouterNdProxyEvent(void *                       aContext,
                                                     otBackboneRouterNdProxyEvent aEvent,
                                                     const otIp6Address *         aAddress)
{
    static_cast<BackboneAgent *>(aContext)->HandleBackboneRouterNdProxyEvent(aEvent, aAddress);
}

void BackboneAgent::HandleBackboneRouterNdProxyEvent(otBackboneRouterNdProxyEvent aEvent, const otIp6Address *aDua)
{
    mNdProxyManager.HandleBackboneRouterNdProxyEvent(aEvent, aDua);
}
#endif

} // namespace BackboneRouter
} // namespace otbr

#endif // OTBR_ENABLE_BACKBONE_ROUTER
