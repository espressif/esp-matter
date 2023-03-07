/*
 *  Copyright (c) 2019, The OpenThread Authors.
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
 * This file includes definitions for ubus API.
 */

#ifndef OTBR_AGENT_OTUBUS_HPP_
#define OTBR_AGENT_OTUBUS_HPP_

#include "openthread-br/config.h"

#include <stdarg.h>
#include <time.h>

#include <openthread/ip6.h>
#include <openthread/link.h>
#include <openthread/netdiag.h>
#include <openthread/udp.h>

#include "common/code_utils.hpp"
#include "common/mainloop.hpp"
#include "ncp/ncp_openthread.hpp"

extern "C" {
#include <libubox/blobmsg_json.h>
#include <libubox/uloop.h>
#include <libubox/ustream.h>
#include <libubox/utils.h>
#include <libubus.h>
}

namespace otbr {
namespace Ncp {
class ControllerOpenThread;
}

namespace ubus {

/**
 * @namespace otbr::ubus
 *
 * @brief
 *   This namespace contains definitions for ubus related instance.
 *
 */

class UbusServer
{
public:
    /**
     * Constructor
     *
     * @param[in] aController  A pointer to OpenThread Controller structure.
     * @param[in] aMutex       A pointer to mutex.
     */
    static void Initialize(Ncp::ControllerOpenThread *aController, std::mutex *aMutex);

    /**
     * This method return the instance of the global UbusServer.
     *
     * @retval The reference of the UbusServer Instance.
     *
     */
    static UbusServer &GetInstance(void);

    /**
     * This method install ubus object onto OpenWRT.
     *
     */
    void InstallUbusObject(void);

    /**
     * This method handle ubus scan function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusScanHandler(struct ubus_context *     aContext,
                               struct ubus_object *      aObj,
                               struct ubus_request_data *aRequest,
                               const char *              aMethod,
                               struct blob_attr *        aMsg);

    /**
     * This method handle ubus get channel function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusChannelHandler(struct ubus_context *     aContext,
                                  struct ubus_object *      aObj,
                                  struct ubus_request_data *aRequest,
                                  const char *              aMethod,
                                  struct blob_attr *        aMsg);

    /**
     * This method handle ubus set channel function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusSetChannelHandler(struct ubus_context *     aContext,
                                     struct ubus_object *      aObj,
                                     struct ubus_request_data *aRequest,
                                     const char *              aMethod,
                                     struct blob_attr *        aMsg);

    /**
     * This method handle ubus get networkname function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusNetworknameHandler(struct ubus_context *     aContext,
                                      struct ubus_object *      aObj,
                                      struct ubus_request_data *aRequest,
                                      const char *              aMethod,
                                      struct blob_attr *        aMsg);

    /**
     * This method handle ubus set networkname function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusSetNetworknameHandler(struct ubus_context *     aContext,
                                         struct ubus_object *      aObj,
                                         struct ubus_request_data *aRequest,
                                         const char *              aMethod,
                                         struct blob_attr *        aMsg);

    /**
     * This method handle ubus get state function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusStateHandler(struct ubus_context *     aContext,
                                struct ubus_object *      aObj,
                                struct ubus_request_data *aRequest,
                                const char *              aMethod,
                                struct blob_attr *        aMsg);

    /**
     * This method handle ubus set state function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusMacfilterSetStateHandler(struct ubus_context *     aContext,
                                            struct ubus_object *      aObj,
                                            struct ubus_request_data *aRequest,
                                            const char *              aMethod,
                                            struct blob_attr *        aMsg);

    /**
     * This method handle ubus get panid function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusPanIdHandler(struct ubus_context *     aContext,
                                struct ubus_object *      aObj,
                                struct ubus_request_data *aRequest,
                                const char *              aMethod,
                                struct blob_attr *        aMsg);

    /**
     * This method handle ubus set panid function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusSetPanIdHandler(struct ubus_context *     aContext,
                                   struct ubus_object *      aObj,
                                   struct ubus_request_data *aRequest,
                                   const char *              aMethod,
                                   struct blob_attr *        aMsg);

    /**
     * This method handle ubus get pskc function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusPskcHandler(struct ubus_context *     aContext,
                               struct ubus_object *      aObj,
                               struct ubus_request_data *aRequest,
                               const char *              aMethod,
                               struct blob_attr *        aMsg);

    /**
     * This method handle ubus set pskc function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusSetPskcHandler(struct ubus_context *     aContext,
                                  struct ubus_object *      aObj,
                                  struct ubus_request_data *aRequest,
                                  const char *              aMethod,
                                  struct blob_attr *        aMsg);

    /**
     * This method handle ubus get networkkey function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusNetworkkeyHandler(struct ubus_context *     aContext,
                                     struct ubus_object *      aObj,
                                     struct ubus_request_data *aRequest,
                                     const char *              aMethod,
                                     struct blob_attr *        aMsg);

    /**
     * This method handle ubus set networkkey function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusSetNetworkkeyHandler(struct ubus_context *     aContext,
                                        struct ubus_object *      aObj,
                                        struct ubus_request_data *aRequest,
                                        const char *              aMethod,
                                        struct blob_attr *        aMsg);

    /**
     * This method handle ubus get rloc16 function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusRloc16Handler(struct ubus_context *     aContext,
                                 struct ubus_object *      aObj,
                                 struct ubus_request_data *aRequest,
                                 const char *              aMethod,
                                 struct blob_attr *        aMsg);

    /**
     * This method handle ubus get extpanid function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusExtPanIdHandler(struct ubus_context *     aContext,
                                   struct ubus_object *      aObj,
                                   struct ubus_request_data *aRequest,
                                   const char *              aMethod,
                                   struct blob_attr *        aMsg);

    /**
     * This method handle ubus set extpanid function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusSetExtPanIdHandler(struct ubus_context *     aContext,
                                      struct ubus_object *      aObj,
                                      struct ubus_request_data *aRequest,
                                      const char *              aMethod,
                                      struct blob_attr *        aMsg);

    /**
     * This method handle ubus get mode function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusModeHandler(struct ubus_context *     aContext,
                               struct ubus_object *      aObj,
                               struct ubus_request_data *aRequest,
                               const char *              aMethod,
                               struct blob_attr *        aMsg);

    /**
     * This method handle ubus set mode function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusSetModeHandler(struct ubus_context *     aContext,
                                  struct ubus_object *      aObj,
                                  struct ubus_request_data *aRequest,
                                  const char *              aMethod,
                                  struct blob_attr *        aMsg);

    /**
     * This method handle ubus get partitionid function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusPartitionIdHandler(struct ubus_context *     aContext,
                                      struct ubus_object *      aObj,
                                      struct ubus_request_data *aRequest,
                                      const char *              aMethod,
                                      struct blob_attr *        aMsg);

    /**
     * This method handle ubus get leaderdata function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusLeaderdataHandler(struct ubus_context *     aContext,
                                     struct ubus_object *      aObj,
                                     struct ubus_request_data *aRequest,
                                     const char *              aMethod,
                                     struct blob_attr *        aMsg);

    /**
     * This method handle ubus get networkdata function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusNetworkdataHandler(struct ubus_context *     aContext,
                                      struct ubus_object *      aObj,
                                      struct ubus_request_data *aRequest,
                                      const char *              aMethod,
                                      struct blob_attr *        aMsg);

    /**
     * This method handle ubus get parent function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusParentHandler(struct ubus_context *     aContext,
                                 struct ubus_object *      aObj,
                                 struct ubus_request_data *aRequest,
                                 const char *              aMethod,
                                 struct blob_attr *        aMsg);

    /**
     * This method handle ubus get neighbor function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusNeighborHandler(struct ubus_context *     aContext,
                                   struct ubus_object *      aObj,
                                   struct ubus_request_data *aRequest,
                                   const char *              aMethod,
                                   struct blob_attr *        aMsg);

    /**
     * This method handle ubus start thread function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusThreadStartHandler(struct ubus_context *     aContext,
                                      struct ubus_object *      aObj,
                                      struct ubus_request_data *aRequest,
                                      const char *              aMethod,
                                      struct blob_attr *        aMsg);

    /**
     * This method handle ubus stop thread function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusThreadStopHandler(struct ubus_context *     aContext,
                                     struct ubus_object *      aObj,
                                     struct ubus_request_data *aRequest,
                                     const char *              aMethod,
                                     struct blob_attr *        aMsg);

    /**
     * This method handle ubus leave function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusLeaveHandler(struct ubus_context *     aContext,
                                struct ubus_object *      aObj,
                                struct ubus_request_data *aRequest,
                                const char *              aMethod,
                                struct blob_attr *        aMsg);

    /**
     * This method handle ubus get macfilter address function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusMacfilterAddrHandler(struct ubus_context *     aContext,
                                        struct ubus_object *      aObj,
                                        struct ubus_request_data *aRequest,
                                        const char *              aMethod,
                                        struct blob_attr *        aMsg);

    /**
     * This method handle ubus get macfilter state function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusMacfilterStateHandler(struct ubus_context *     aContext,
                                         struct ubus_object *      aObj,
                                         struct ubus_request_data *aRequest,
                                         const char *              aMethod,
                                         struct blob_attr *        aMsg);

    /**
     * This method handle ubus macfilter address add function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusMacfilterAddHandler(struct ubus_context *     aContext,
                                       struct ubus_object *      aObj,
                                       struct ubus_request_data *aRequest,
                                       const char *              aMethod,
                                       struct blob_attr *        aMsg);

    /**
     * This method handle ubus macfilter address clear function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusMacfilterClearHandler(struct ubus_context *     aContext,
                                         struct ubus_object *      aObj,
                                         struct ubus_request_data *aRequest,
                                         const char *              aMethod,
                                         struct blob_attr *        aMsg);

    /**
     * This method handle ubus macfilter address remove function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusMacfilterRemoveHandler(struct ubus_context *     aContext,
                                          struct ubus_object *      aObj,
                                          struct ubus_request_data *aRequest,
                                          const char *              aMethod,
                                          struct blob_attr *        aMsg);

    /**
     * This method handle ubus start commissioner function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusCommissionerStartHandler(struct ubus_context *     aContext,
                                            struct ubus_object *      aObj,
                                            struct ubus_request_data *aRequest,
                                            const char *              aMethod,
                                            struct blob_attr *        aMsg);

    /**
     * This method handle ubus add joiner function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusJoinerAddHandler(struct ubus_context *     aContext,
                                    struct ubus_object *      aObj,
                                    struct ubus_request_data *aRequest,
                                    const char *              aMethod,
                                    struct blob_attr *        aMsg);

    /**
     * This method handle ubus remove joiner function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusJoinerRemoveHandler(struct ubus_context *     aContext,
                                       struct ubus_object *      aObj,
                                       struct ubus_request_data *aRequest,
                                       const char *              aMethod,
                                       struct blob_attr *        aMsg);

    /**
     * This method handle ubus get joiner information function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusJoinerNumHandler(struct ubus_context *     aContext,
                                    struct ubus_object *      aObj,
                                    struct ubus_request_data *aRequest,
                                    const char *              aMethod,
                                    struct blob_attr *        aMsg);

    /**
     * This method handle ubus mgmtset function request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    static int UbusMgmtsetHandler(struct ubus_context *     aContext,
                                  struct ubus_object *      aObj,
                                  struct ubus_request_data *aRequest,
                                  const char *              aMethod,
                                  struct blob_attr *        aMsg);

    /**
     * This method handle initial diagnostic get response.
     *
     * @param[in] aError        A error of receiving the diagnostic response.
     * @param[in] aMessage      A pointer to the message.
     * @param[in] aMessageInfo  A pointer to the message information.
     * @param[in] aContext      A pointer to the context.
     *
     */
    static void HandleDiagnosticGetResponse(otError              aError,
                                            otMessage *          aMessage,
                                            const otMessageInfo *aMessageInfo,
                                            void *               aContext);

    /**
     * This method handle diagnosticget response.
     *
     * @param[in] aError       A error of receiving the diagnostic response.
     * @param[in] aMessage     A pointer to the message.
     * @param[in] aMessageInfo A pointer to the message information.
     *
     */
    void HandleDiagnosticGetResponse(otError aError, otMessage *aMessage, const otMessageInfo *aMessageInfo);

private:
    bool                       mIfFinishScan;
    struct ubus_context *      mContext;
    const char *               mSockPath;
    struct blob_buf            mBuf;
    struct blob_buf            mNetworkdataBuf;
    Ncp::ControllerOpenThread *mController;
    std::mutex *               mNcpThreadMutex;
    time_t                     mSecond;
    enum
    {
        kDefaultJoinerTimeout = 120,
    };

    /**
     * Constructor
     *
     * @param[in] aController  The pointer to OpenThread Controller structure.
     * @param[in] aMutex       A pointer to mutex.
     */
    UbusServer(Ncp::ControllerOpenThread *aController, std::mutex *aMutex);

    /**
     * This method start scan.
     *
     */
    void ProcessScan(void);

    /**
     * This method detailly start scan.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    int UbusScanHandlerDetail(struct ubus_context *     aContext,
                              struct ubus_object *      aObj,
                              struct ubus_request_data *aRequest,
                              const char *              aMethod,
                              struct blob_attr *        aMsg);

    /**
     * This method handle scan result (callback function).
     *
     * @param[in] aResult   A pointer to result.
     * @param[in] aContext  A pointer to context.
     *
     */
    static void HandleActiveScanResult(otActiveScanResult *aResult, void *aContext);

    /**
     * This method detailly handler the scan result, called by HandleActiveScanResult.
     *
     * @param[in] aResult  A pointer to result.
     *
     */
    void HandleActiveScanResultDetail(otActiveScanResult *aResult);

    /**
     * This method detailly handler get neighbor information.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    int UbusNeighborHandlerDetail(struct ubus_context *     aContext,
                                  struct ubus_object *      aObj,
                                  struct ubus_request_data *aRequest,
                                  const char *              aMethod,
                                  struct blob_attr *        aMsg);

    /**
     * This method detailly handler get parent information.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    int UbusParentHandlerDetail(struct ubus_context *     aContext,
                                struct ubus_object *      aObj,
                                struct ubus_request_data *aRequest,
                                const char *              aMethod,
                                struct blob_attr *        aMsg);

    /**
     * This method handle mgmtset request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    int UbusMgmtset(struct ubus_context *     aContext,
                    struct ubus_object *      aObj,
                    struct ubus_request_data *aRequest,
                    const char *              aMethod,
                    struct blob_attr *        aMsg);

    /**
     * This method handle leave request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    int UbusLeaveHandlerDetail(struct ubus_context *     aContext,
                               struct ubus_object *      aObj,
                               struct ubus_request_data *aRequest,
                               const char *              aMethod,
                               struct blob_attr *        aMsg);

    /**
     * This method handle thread related request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     * @param[in] aAction   A pointer to the action needed.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    int UbusThreadHandler(struct ubus_context *     aContext,
                          struct ubus_object *      aObj,
                          struct ubus_request_data *aRequest,
                          const char *              aMethod,
                          struct blob_attr *        aMsg,
                          const char *              aAction);

    /**
     * This method handle get information request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     * @param[in] aAction   A pointer to the action needed.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    int UbusGetInformation(struct ubus_context *     aContext,
                           struct ubus_object *      aObj,
                           struct ubus_request_data *aRequest,
                           const char *              aMethod,
                           struct blob_attr *        aMsg,
                           const char *              action);

    /**
     * This method handle set information request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     * @param[in] aAction   A pointer to the action needed.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    int UbusSetInformation(struct ubus_context *     aContext,
                           struct ubus_object *      aObj,
                           struct ubus_request_data *aRequest,
                           const char *              aMethod,
                           struct blob_attr *        aMsg,
                           const char *              aAction);

    /**
     * This method handle commissioner related request.
     *
     * @param[in] aContext  A pointer to the ubus context.
     * @param[in] aObj      A pointer to the ubus object.
     * @param[in] aRequest  A pointer to the ubus request.
     * @param[in] aMethod   A pointer to the ubus method.
     * @param[in] aMsg      A pointer to the ubus message.
     * @param[in] aAction   A pointer to the action needed.
     *
     * @retval 0  Successfully handler the request.
     *
     */
    int UbusCommissioner(struct ubus_context *     aContext,
                         struct ubus_object *      aObj,
                         struct ubus_request_data *aRequest,
                         const char *              aMethod,
                         struct blob_attr *        aMsg,
                         const char *              aAction);

    /**
     * This method handle conmmissione state change (callback function).
     *
     * @param[in] aState    The state of commissioner.
     * @param[in] aContext  A pointer to the ubus context.
     *
     */
    static void HandleStateChanged(otCommissionerState aState, void *aContext);

    /**
     * This method handle conmmissione state change.
     *
     * @param[in] aState  The state of commissioner.
     *
     */
    void HandleStateChanged(otCommissionerState aState);

    /**
     * This method handle joiner event (callback function).
     *
     * @param[in] aEvent       The joiner event type.
     * @param[in] aJoinerInfo  A pointer to the Joiner Info.
     * @param[in] aJoinerId    A pointer to the Joiner ID (if not known, it will be NULL).
     * @param[in] aContext     A pointer to application-specific context.
     *
     */
    static void HandleJoinerEvent(otCommissionerJoinerEvent aEvent,
                                  const otJoinerInfo *      aJoinerInfo,
                                  const otExtAddress *      aJoinerId,
                                  void *                    aContext);

    /**
     * This method handle joiner event.
     *
     * @param[in] aEvent       The joiner event type.
     * @param[in] aJoinerInfo  A pointer to the Joiner Info.
     * @param[in] aJoinerId    A pointer to the Joiner ID (if not known, it will be NULL).
     *
     */
    void HandleJoinerEvent(otCommissionerJoinerEvent aEvent,
                           const otJoinerInfo *      aJoinerInfo,
                           const otExtAddress *      aJoinerId);

    /**
     * This method convert thread network state to string.
     *
     * @param[in]  aInstance  A pointer to the instance.
     * @param[out] aState     A pointer to the string address.
     *
     */
    void GetState(otInstance *aInstance, char *aState);

    /**
     * This method add fd of ubus object.
     *
     */
    void UbusAddFd(void);

    /**
     * This method set ubus reconnect time.
     *
     * @param[in] aTimeout  A pointer to the timeout.
     *
     */
    static void UbusReconnTimer(struct uloop_timeout *aTimeout);

    /**
     * This method detailly handle ubus reconnect time.
     *
     * @param[in] aTimeout  A pointer to the timeout.
     *
     */
    void UbusReconnTimerDetail(struct uloop_timeout *aTimeout);

    /**
     * This method handle ubus connection lost.
     *
     * @param[in] aContext  A pointer to the context.
     *
     */
    static void UbusConnectionLost(struct ubus_context *aContext);

    /**
     * This method connect and display ubus.
     *
     * @param[in] aPath  A pointer to the ubus server path(default is nullptr).
     *
     * @retval 0  Successfully handler the request.
     *
     */
    int DisplayUbusInit(const char *aPath);

    /**
     * This method disconnect and display ubus.
     *
     */
    void DisplayUbusDone(void);

    /**
     * This method parses an ASCII string as a long.
     *
     * @param[in]  aString  A pointer to the ASCII string.
     * @param[out] aLong    A reference to where the parsed long is placed.
     *
     * @retval OT_ERROR_NONE   Successfully parsed the ASCII string.
     * @retval OT_ERROR_PARSE  Could not parse the ASCII string.
     *
     */
    otError ParseLong(char *aString, long &aLong);

    /**
     * This method converts a hex string to binary.
     *
     * @param[in]  aHex        A pointer to the hex string.
     * @param[out] aBin        A pointer to where the binary representation is placed.
     * @param[in]  aBinLength  Maximum length of the binary representation.
     *
     * @returns The number of bytes in the binary representation.
     */
    int Hex2Bin(const char *aHex, uint8_t *aBin, uint16_t aBinLength);

    /**
     * This method output bytes into char*.
     *
     * @param[in]  aBytes   A pointer to the bytes need to be convert.
     * @param[in]  aLength  The length of the bytes.
     * @param[out] aOutput  A pointer to the char* string.
     *
     */
    void OutputBytes(const uint8_t *aBytes, uint8_t aLength, char *aOutput);

    /**
     * This method append result in message passed to ubus.
     *
     * @param[in] aError    The error type of the message.
     * @param[in] aContext  A pointer to the context.
     * @param[in] aRequest  A pointer to the request.
     *
     */
    void AppendResult(otError aError, struct ubus_context *aContext, struct ubus_request_data *aRequest);
};

class UBusAgent : public MainloopProcessor
{
public:
    /**
     * The constructor to initialize the UBus agent.
     *
     * @param[in] aNcp  A reference to the NCP controller.
     *
     */
    UBusAgent(otbr::Ncp::ControllerOpenThread &aNcp)
        : mNcp(aNcp)
        , mThreadMutex()
    {
    }

    /**
     * This method initializes the UBus agent.
     *
     */
    void Init(void);

    void Update(MainloopContext &aMainloop) override;
    void Process(const MainloopContext &aMainloop) override;

private:
    static void UbusServerRun(void) { otbr::ubus::UbusServer::GetInstance().InstallUbusObject(); }

    otbr::Ncp::ControllerOpenThread &mNcp;
    std::mutex                       mThreadMutex;
};
} // namespace ubus
} // namespace otbr

#endif // OTBR_AGENT_OTUBUS_HPP_
