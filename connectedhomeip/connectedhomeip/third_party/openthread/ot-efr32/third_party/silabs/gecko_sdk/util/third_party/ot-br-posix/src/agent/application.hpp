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
 *   This file includes definition for OTBR Agent.
 */

#ifndef OTBR_AGENT_APPLICATION_HPP_
#define OTBR_AGENT_APPLICATION_HPP_

#include <atomic>
#include <signal.h>
#include <stdint.h>
#include <vector>

#if OTBR_ENABLE_BORDER_AGENT
#include "border_agent/border_agent.hpp"
#endif
#include "ncp/ncp_openthread.hpp"
#if OTBR_ENABLE_BACKBONE_ROUTER
#include "backbone_router/backbone_agent.hpp"
#endif
#if OTBR_ENABLE_REST_SERVER
#include "rest/rest_web_server.hpp"
#endif
#if OTBR_ENABLE_DBUS_SERVER
#include "dbus/server/dbus_agent.hpp"
#endif
#if OTBR_ENABLE_OPENWRT
#include "openwrt/ubus/otubus.hpp"
#endif
#if OTBR_ENABLE_VENDOR_SERVER
#include "agent/vendor.hpp"
#endif

namespace otbr {

/**
 * @addtogroup border-router-agent
 *
 * @brief
 *   This module includes definition for OTBR application.
 *
 * @{
 */

/**
 * This class implements OTBR application management.
 *
 */
class Application : private NonCopyable
{
public:
    /**
     * This constructor initializes the Application instance.
     *
     * @param[in] aInterfaceName         Name of the Thread network interface.
     * @param[in] aBackboneInterfaceName Name of the backbone network interface.
     * @param[in] aRadioUrls             The radio URLs (can be IEEE802.15.4 or TREL radio).
     * @param[in] aEnableAutoAttach      Whether or not to automatically attach to the saved network.
     *
     */
    explicit Application(const std::string &              aInterfaceName,
                         const std::string &              aBackboneInterfaceName,
                         const std::vector<const char *> &aRadioUrls,
                         bool                             aEnableAutoAttach);

    /**
     * This method initializes the Application instance.
     *
     */
    void Init(void);

    /**
     * This method de-initializes the Application instance.
     *
     */
    void Deinit(void);

    /**
     * This method runs the application until exit.
     *
     * @retval OTBR_ERROR_NONE  The application exited without any error.
     * @retval OTBR_ERROR_ERRNO The application exited with some system error.
     *
     */
    otbrError Run(void);

private:
    // Default poll timeout.
    static const struct timeval kPollTimeout;

    static void HandleSignal(int aSignal);

    std::string               mInterfaceName;
    std::string               mBackboneInterfaceName;
    Ncp::ControllerOpenThread mNcp;
#if OTBR_ENABLE_BORDER_AGENT
    BorderAgent mBorderAgent;
#endif
#if OTBR_ENABLE_BACKBONE_ROUTER
    BackboneRouter::BackboneAgent mBackboneAgent;
#endif
#if OTBR_ENABLE_OPENWRT
    ubus::UBusAgent mUbusAgent;
#endif
#if OTBR_ENABLE_REST_SERVER
    rest::RestWebServer mRestWebServer;
#endif
#if OTBR_ENABLE_DBUS_SERVER
    DBus::DBusAgent mDBusAgent;
#endif
#if OTBR_ENABLE_VENDOR_SERVER
    vendor::VendorServer mVendorServer;
#endif

    static std::atomic_bool sShouldTerminate;
};

/**
 * @}
 */

} // namespace otbr

#endif // OTBR_AGENT_APPLICATION_HPP_
