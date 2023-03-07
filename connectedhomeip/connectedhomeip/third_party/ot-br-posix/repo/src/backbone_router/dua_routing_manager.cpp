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
 *   The file implements DUA routing functionalities.
 */

#include "backbone_router/dua_routing_manager.hpp"

#if OTBR_ENABLE_DUA_ROUTING

#include "common/code_utils.hpp"

namespace otbr {

namespace BackboneRouter {

void DuaRoutingManager::Enable(const Ip6Prefix &aDomainPrefix)
{
    VerifyOrExit(!mEnabled);
    mEnabled = true;

    mDomainPrefix = aDomainPrefix;

    AddDefaultRouteToThread();
    AddPolicyRouteToBackbone();

exit:
    otbrLogResult(OTBR_ERROR_NONE, "DuaRoutingManager: %s", __FUNCTION__);
}

void DuaRoutingManager::Disable(void)
{
    VerifyOrExit(mEnabled);
    mEnabled = false;

    DelDefaultRouteToThread();
    DelPolicyRouteToBackbone();

exit:
    otbrLogResult(OTBR_ERROR_NONE, "DuaRoutingManager: %s", __FUNCTION__);
}

void DuaRoutingManager::AddDefaultRouteToThread(void)
{
    SystemUtils::ExecuteCommand("ip -6 route add %s dev %s proto static metric 1", mDomainPrefix.ToString().c_str(),
                                mInterfaceName.c_str());
}

void DuaRoutingManager::DelDefaultRouteToThread(void)
{
    SystemUtils::ExecuteCommand("ip -6 route del %s dev %s proto static metric 1", mDomainPrefix.ToString().c_str(),
                                mInterfaceName.c_str());
}

void DuaRoutingManager::AddPolicyRouteToBackbone(void)
{
    // Packets from Thread interface use route table "openthread"
    SystemUtils::ExecuteCommand("ip -6 rule add iif %s table openthread", mInterfaceName.c_str());
    SystemUtils::ExecuteCommand("ip -6 route add %s dev %s proto static table openthread",
                                mDomainPrefix.ToString().c_str(), mBackboneInterfaceName.c_str());
}

void DuaRoutingManager::DelPolicyRouteToBackbone(void)
{
    SystemUtils::ExecuteCommand("ip -6 rule del iif %s table openthread", mInterfaceName.c_str());
    SystemUtils::ExecuteCommand("ip -6 route del %s dev %s proto static table openthread",
                                mDomainPrefix.ToString().c_str(), mBackboneInterfaceName.c_str());
}

} // namespace BackboneRouter
} // namespace otbr

#endif // OTBR_ENABLE_DUA_ROUTING
