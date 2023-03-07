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
 *   This file includes definition for DUA routing functionalities.
 */

#ifndef BACKBONE_ROUTER_DUA_ROUTING_MANAGER
#define BACKBONE_ROUTER_DUA_ROUTING_MANAGER

#if OTBR_ENABLE_DUA_ROUTING

#include <set>
#include <utility>
#include <openthread/backbone_router_ftd.h>

#include "common/code_utils.hpp"
#include "ncp/ncp_openthread.hpp"
#include "utils/system_utils.hpp"

namespace otbr {
namespace BackboneRouter {

/**
 * @addtogroup border-router-backbone
 *
 * @brief
 *   This module includes definition for DUA routing functionalities.
 *
 * @{
 */

/**
 * This class implements the DUA routing manager.
 *
 */
class DuaRoutingManager : private NonCopyable
{
public:
    /**
     * This constructor initializes a DUA routing manager instance.
     *
     */
    explicit DuaRoutingManager(std::string aInterfaceName, std::string aBackboneInterfaceName)
        : mEnabled(false)
        , mInterfaceName(std::move(aInterfaceName))
        , mBackboneInterfaceName(std::move(aBackboneInterfaceName))
    {
    }

    /**
     * This method enables the DUA routing manager.
     *
     */
    void Enable(const Ip6Prefix &aDomainPrefix);

    /**
     * This method disables the DUA routing manager.
     *
     */
    void Disable(void);

private:
    void AddDefaultRouteToThread(void);
    void DelDefaultRouteToThread(void);
    void AddPolicyRouteToBackbone(void);
    void DelPolicyRouteToBackbone(void);

    Ip6Prefix   mDomainPrefix;
    bool        mEnabled : 1;
    std::string mInterfaceName;
    std::string mBackboneInterfaceName;
};

/**
 * @}
 */

} // namespace BackboneRouter
} // namespace otbr

#endif // OTBR_ENABLE_DUA_ROUTING

#endif // BACKBONE_ROUTER_DUA_ROUTING_MANAGER
