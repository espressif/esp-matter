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

#if __ORDER_BIG_ENDIAN__
#define BYTE_ORDER_BIG_ENDIAN 1
#endif

#define OTBR_LOG_TAG "UBUS"

#include "openwrt/ubus/otubus.hpp"

#include <mutex>

#include <arpa/inet.h>
#include <sys/eventfd.h>

#include <openthread/commissioner.h>
#include <openthread/thread.h>
#include <openthread/thread_ftd.h>

#include "common/logging.hpp"
#include "ncp/ncp_openthread.hpp"

namespace otbr {
namespace ubus {

static UbusServer *sUbusServerInstance = nullptr;
static int         sUbusEfd            = -1;
static void *      sJsonUri            = nullptr;
static int         sBufNum;

const static int PANID_LENGTH      = 10;
const static int XPANID_LENGTH     = 64;
const static int NETWORKKEY_LENGTH = 64;

UbusServer::UbusServer(Ncp::ControllerOpenThread *aController, std::mutex *aMutex)
    : mIfFinishScan(false)
    , mContext(nullptr)
    , mSockPath(nullptr)
    , mController(aController)
    , mNcpThreadMutex(aMutex)
    , mSecond(0)
{
    memset(&mNetworkdataBuf, 0, sizeof(mNetworkdataBuf));
    memset(&mBuf, 0, sizeof(mBuf));

    blob_buf_init(&mBuf, 0);
    blob_buf_init(&mNetworkdataBuf, 0);
}

UbusServer &UbusServer::GetInstance(void)
{
    return *sUbusServerInstance;
}

void UbusServer::Initialize(Ncp::ControllerOpenThread *aController, std::mutex *aMutex)
{
    sUbusServerInstance = new UbusServer(aController, aMutex);
}

enum
{
    SETNETWORK,
    SET_NETWORK_MAX,
};

enum
{
    PSKD,
    EUI64,
    ADD_JOINER_MAX,
};

enum
{
    NETWORKKEY,
    NETWORKNAME,
    EXTPANID,
    PANID,
    CHANNEL,
    PSKC,
    MGMTSET_MAX,
};

static const struct blobmsg_policy setNetworknamePolicy[SET_NETWORK_MAX] = {
    [SETNETWORK] = {.name = "networkname", .type = BLOBMSG_TYPE_STRING},
};

static const struct blobmsg_policy setPanIdPolicy[SET_NETWORK_MAX] = {
    [SETNETWORK] = {.name = "panid", .type = BLOBMSG_TYPE_STRING},
};

static const struct blobmsg_policy setExtPanIdPolicy[SET_NETWORK_MAX] = {
    [SETNETWORK] = {.name = "extpanid", .type = BLOBMSG_TYPE_STRING},
};

static const struct blobmsg_policy setChannelPolicy[SET_NETWORK_MAX] = {
    [SETNETWORK] = {.name = "channel", .type = BLOBMSG_TYPE_INT32},
};

static const struct blobmsg_policy setPskcPolicy[SET_NETWORK_MAX] = {
    [SETNETWORK] = {.name = "pskc", .type = BLOBMSG_TYPE_STRING},
};

static const struct blobmsg_policy setNetworkkeyPolicy[SET_NETWORK_MAX] = {
    [SETNETWORK] = {.name = "networkkey", .type = BLOBMSG_TYPE_STRING},
};

static const struct blobmsg_policy setModePolicy[SET_NETWORK_MAX] = {
    [SETNETWORK] = {.name = "mode", .type = BLOBMSG_TYPE_STRING},
};

static const struct blobmsg_policy macfilterAddPolicy[SET_NETWORK_MAX] = {
    [SETNETWORK] = {.name = "addr", .type = BLOBMSG_TYPE_STRING},
};

static const struct blobmsg_policy macfilterRemovePolicy[SET_NETWORK_MAX] = {
    [SETNETWORK] = {.name = "addr", .type = BLOBMSG_TYPE_STRING},
};

static const struct blobmsg_policy macfilterSetStatePolicy[SET_NETWORK_MAX] = {
    [SETNETWORK] = {.name = "state", .type = BLOBMSG_TYPE_STRING},
};

static const struct blobmsg_policy removeJoinerPolicy[SET_NETWORK_MAX] = {
    [SETNETWORK] = {.name = "eui64", .type = BLOBMSG_TYPE_STRING},
};

static const struct blobmsg_policy addJoinerPolicy[ADD_JOINER_MAX] = {
    [PSKD]  = {.name = "pskd", .type = BLOBMSG_TYPE_STRING},
    [EUI64] = {.name = "eui64", .type = BLOBMSG_TYPE_STRING},
};

static const struct blobmsg_policy mgmtsetPolicy[MGMTSET_MAX] = {
    [NETWORKKEY]  = {.name = "networkkey", .type = BLOBMSG_TYPE_STRING},
    [NETWORKNAME] = {.name = "networkname", .type = BLOBMSG_TYPE_STRING},
    [EXTPANID]    = {.name = "extpanid", .type = BLOBMSG_TYPE_STRING},
    [PANID]       = {.name = "panid", .type = BLOBMSG_TYPE_STRING},
    [CHANNEL]     = {.name = "channel", .type = BLOBMSG_TYPE_STRING},
    [PSKC]        = {.name = "pskc", .type = BLOBMSG_TYPE_STRING},
};

static const struct ubus_method otbrMethods[] = {
    {"scan", &UbusServer::UbusScanHandler, 0, 0, nullptr, 0},
    {"channel", &UbusServer::UbusChannelHandler, 0, 0, nullptr, 0},
    {"setchannel", &UbusServer::UbusSetChannelHandler, 0, 0, setChannelPolicy, ARRAY_SIZE(setChannelPolicy)},
    {"networkname", &UbusServer::UbusNetworknameHandler, 0, 0, nullptr, 0},
    {"setnetworkname", &UbusServer::UbusSetNetworknameHandler, 0, 0, setNetworknamePolicy,
     ARRAY_SIZE(setNetworknamePolicy)},
    {"state", &UbusServer::UbusStateHandler, 0, 0, nullptr, 0},
    {"panid", &UbusServer::UbusPanIdHandler, 0, 0, nullptr, 0},
    {"setpanid", &UbusServer::UbusSetPanIdHandler, 0, 0, setPanIdPolicy, ARRAY_SIZE(setPanIdPolicy)},
    {"rloc16", &UbusServer::UbusRloc16Handler, 0, 0, nullptr, 0},
    {"extpanid", &UbusServer::UbusExtPanIdHandler, 0, 0, nullptr, 0},
    {"setextpanid", &UbusServer::UbusSetExtPanIdHandler, 0, 0, setExtPanIdPolicy, ARRAY_SIZE(setExtPanIdPolicy)},
    {"networkkey", &UbusServer::UbusNetworkkeyHandler, 0, 0, nullptr, 0},
    {"setnetworkkey", &UbusServer::UbusSetNetworkkeyHandler, 0, 0, setNetworkkeyPolicy,
     ARRAY_SIZE(setNetworkkeyPolicy)},
    {"pskc", &UbusServer::UbusPskcHandler, 0, 0, nullptr, 0},
    {"setpskc", &UbusServer::UbusSetPskcHandler, 0, 0, setPskcPolicy, ARRAY_SIZE(setPskcPolicy)},
    {"threadstart", &UbusServer::UbusThreadStartHandler, 0, 0, nullptr, 0},
    {"threadstop", &UbusServer::UbusThreadStopHandler, 0, 0, nullptr, 0},
    {"neighbor", &UbusServer::UbusNeighborHandler, 0, 0, nullptr, 0},
    {"parent", &UbusServer::UbusParentHandler, 0, 0, nullptr, 0},
    {"mode", &UbusServer::UbusModeHandler, 0, 0, nullptr, 0},
    {"setmode", &UbusServer::UbusSetModeHandler, 0, 0, setModePolicy, ARRAY_SIZE(setModePolicy)},
    {"partitionid", &UbusServer::UbusPartitionIdHandler, 0, 0, nullptr, 0},
    {"leave", &UbusServer::UbusLeaveHandler, 0, 0, nullptr, 0},
    {"leaderdata", &UbusServer::UbusLeaderdataHandler, 0, 0, nullptr, 0},
    {"networkdata", &UbusServer::UbusNetworkdataHandler, 0, 0, nullptr, 0},
    {"commissionerstart", &UbusServer::UbusCommissionerStartHandler, 0, 0, nullptr, 0},
    {"joinernum", &UbusServer::UbusJoinerNumHandler, 0, 0, nullptr, 0},
    {"joinerremove", &UbusServer::UbusJoinerRemoveHandler, 0, 0, nullptr, 0},
    {"macfiltersetstate", &UbusServer::UbusMacfilterSetStateHandler, 0, 0, macfilterSetStatePolicy,
     ARRAY_SIZE(macfilterSetStatePolicy)},
    {"macfilteradd", &UbusServer::UbusMacfilterAddHandler, 0, 0, macfilterAddPolicy, ARRAY_SIZE(macfilterAddPolicy)},
    {"macfilterremove", &UbusServer::UbusMacfilterRemoveHandler, 0, 0, macfilterRemovePolicy,
     ARRAY_SIZE(macfilterRemovePolicy)},
    {"macfilterclear", &UbusServer::UbusMacfilterClearHandler, 0, 0, nullptr, 0},
    {"macfilterstate", &UbusServer::UbusMacfilterStateHandler, 0, 0, nullptr, 0},
    {"macfilteraddr", &UbusServer::UbusMacfilterAddrHandler, 0, 0, nullptr, 0},
    {"joineradd", &UbusServer::UbusJoinerAddHandler, 0, 0, addJoinerPolicy, ARRAY_SIZE(addJoinerPolicy)},
    {"mgmtset", &UbusServer::UbusMgmtsetHandler, 0, 0, mgmtsetPolicy, ARRAY_SIZE(mgmtsetPolicy)},
};

static struct ubus_object_type otbrObjType = {"otbr_prog", 0, otbrMethods, ARRAY_SIZE(otbrMethods)};

static struct ubus_object otbr = {
    avl : {},
    name : "otbr",
    id : 0,
    path : nullptr,
    type : &otbrObjType,
    subscribe_cb : nullptr,
    has_subscribers : false,
    methods : otbrMethods,
    n_methods : ARRAY_SIZE(otbrMethods),
};

void UbusServer::ProcessScan(void)
{
    otError  error        = OT_ERROR_NONE;
    uint32_t scanChannels = 0;
    uint16_t scanDuration = 0;

    mNcpThreadMutex->lock();
    SuccessOrExit(error = otLinkActiveScan(mController->GetInstance(), scanChannels, scanDuration,
                                           &UbusServer::HandleActiveScanResult, this));
exit:
    mNcpThreadMutex->unlock();
    return;
}

void UbusServer::HandleActiveScanResult(otActiveScanResult *aResult, void *aContext)
{
    static_cast<UbusServer *>(aContext)->HandleActiveScanResultDetail(aResult);
}

void UbusServer::OutputBytes(const uint8_t *aBytes, uint8_t aLength, char *aOutput)
{
    char byte2char[5] = "";
    for (int i = 0; i < aLength; i++)
    {
        sprintf(byte2char, "%02x", aBytes[i]);
        strcat(aOutput, byte2char);
    }
}

void UbusServer::AppendResult(otError aError, struct ubus_context *aContext, struct ubus_request_data *aRequest)
{
    blobmsg_add_u16(&mBuf, "Error", aError);
    ubus_send_reply(aContext, aRequest, mBuf.head);
}

void UbusServer::HandleActiveScanResultDetail(otActiveScanResult *aResult)
{
    void *jsonList = nullptr;

    char panidstring[PANID_LENGTH];
    char xpanidstring[XPANID_LENGTH] = "";

    if (aResult == nullptr)
    {
        blobmsg_close_array(&mBuf, sJsonUri);
        mIfFinishScan = true;
        goto exit;
    }

    jsonList = blobmsg_open_table(&mBuf, nullptr);

    blobmsg_add_string(&mBuf, "NetworkName", aResult->mNetworkName.m8);

    OutputBytes(aResult->mExtendedPanId.m8, OT_EXT_PAN_ID_SIZE, xpanidstring);
    blobmsg_add_string(&mBuf, "ExtendedPanId", xpanidstring);

    sprintf(panidstring, "0x%04x", aResult->mPanId);
    blobmsg_add_string(&mBuf, "PanId", panidstring);

    blobmsg_add_u32(&mBuf, "Channel", aResult->mChannel);

    blobmsg_add_u32(&mBuf, "Rssi", aResult->mRssi);

    blobmsg_add_u32(&mBuf, "Lqi", aResult->mLqi);

    blobmsg_close_table(&mBuf, jsonList);

exit:
    return;
}

int UbusServer::UbusScanHandler(struct ubus_context *     aContext,
                                struct ubus_object *      aObj,
                                struct ubus_request_data *aRequest,
                                const char *              aMethod,
                                struct blob_attr *        aMsg)
{
    return GetInstance().UbusScanHandlerDetail(aContext, aObj, aRequest, aMethod, aMsg);
}

int UbusServer::UbusScanHandlerDetail(struct ubus_context *     aContext,
                                      struct ubus_object *      aObj,
                                      struct ubus_request_data *aRequest,
                                      const char *              aMethod,
                                      struct blob_attr *        aMsg)
{
    OT_UNUSED_VARIABLE(aObj);
    OT_UNUSED_VARIABLE(aMethod);
    OT_UNUSED_VARIABLE(aMsg);

    otError  error = OT_ERROR_NONE;
    uint64_t eventNum;
    ssize_t  retval;

    blob_buf_init(&mBuf, 0);
    sJsonUri = blobmsg_open_array(&mBuf, "scan_list");

    mIfFinishScan = 0;
    sUbusServerInstance->ProcessScan();

    eventNum = 1;
    retval   = write(sUbusEfd, &eventNum, sizeof(uint64_t));
    if (retval != sizeof(uint64_t))
    {
        error = OT_ERROR_FAILED;
        goto exit;
    }

    while (!mIfFinishScan)
    {
        sleep(1);
    }

exit:
    AppendResult(error, aContext, aRequest);
    return 0;
}

int UbusServer::UbusChannelHandler(struct ubus_context *     aContext,
                                   struct ubus_object *      aObj,
                                   struct ubus_request_data *aRequest,
                                   const char *              aMethod,
                                   struct blob_attr *        aMsg)
{
    return GetInstance().UbusGetInformation(aContext, aObj, aRequest, aMethod, aMsg, "channel");
}

int UbusServer::UbusSetChannelHandler(struct ubus_context *     aContext,
                                      struct ubus_object *      aObj,
                                      struct ubus_request_data *aRequest,
                                      const char *              aMethod,
                                      struct blob_attr *        aMsg)
{
    return GetInstance().UbusSetInformation(aContext, aObj, aRequest, aMethod, aMsg, "channel");
}

int UbusServer::UbusJoinerNumHandler(struct ubus_context *     aContext,
                                     struct ubus_object *      aObj,
                                     struct ubus_request_data *aRequest,
                                     const char *              aMethod,
                                     struct blob_attr *        aMsg)
{
    return GetInstance().UbusGetInformation(aContext, aObj, aRequest, aMethod, aMsg, "joinernum");
}

int UbusServer::UbusNetworknameHandler(struct ubus_context *     aContext,
                                       struct ubus_object *      aObj,
                                       struct ubus_request_data *aRequest,
                                       const char *              aMethod,
                                       struct blob_attr *        aMsg)
{
    return GetInstance().UbusGetInformation(aContext, aObj, aRequest, aMethod, aMsg, "networkname");
}

int UbusServer::UbusSetNetworknameHandler(struct ubus_context *     aContext,
                                          struct ubus_object *      aObj,
                                          struct ubus_request_data *aRequest,
                                          const char *              aMethod,
                                          struct blob_attr *        aMsg)
{
    return GetInstance().UbusSetInformation(aContext, aObj, aRequest, aMethod, aMsg, "networkname");
}

int UbusServer::UbusStateHandler(struct ubus_context *     aContext,
                                 struct ubus_object *      aObj,
                                 struct ubus_request_data *aRequest,
                                 const char *              aMethod,
                                 struct blob_attr *        aMsg)
{
    return GetInstance().UbusGetInformation(aContext, aObj, aRequest, aMethod, aMsg, "state");
}

int UbusServer::UbusRloc16Handler(struct ubus_context *     aContext,
                                  struct ubus_object *      aObj,
                                  struct ubus_request_data *aRequest,
                                  const char *              aMethod,
                                  struct blob_attr *        aMsg)
{
    return GetInstance().UbusGetInformation(aContext, aObj, aRequest, aMethod, aMsg, "rloc16");
}

int UbusServer::UbusPanIdHandler(struct ubus_context *     aContext,
                                 struct ubus_object *      aObj,
                                 struct ubus_request_data *aRequest,
                                 const char *              aMethod,
                                 struct blob_attr *        aMsg)
{
    return GetInstance().UbusGetInformation(aContext, aObj, aRequest, aMethod, aMsg, "panid");
}

int UbusServer::UbusSetPanIdHandler(struct ubus_context *     aContext,
                                    struct ubus_object *      aObj,
                                    struct ubus_request_data *aRequest,
                                    const char *              aMethod,
                                    struct blob_attr *        aMsg)
{
    return GetInstance().UbusSetInformation(aContext, aObj, aRequest, aMethod, aMsg, "panid");
}

int UbusServer::UbusExtPanIdHandler(struct ubus_context *     aContext,
                                    struct ubus_object *      aObj,
                                    struct ubus_request_data *aRequest,
                                    const char *              aMethod,
                                    struct blob_attr *        aMsg)
{
    return GetInstance().UbusGetInformation(aContext, aObj, aRequest, aMethod, aMsg, "extpanid");
}

int UbusServer::UbusSetExtPanIdHandler(struct ubus_context *     aContext,
                                       struct ubus_object *      aObj,
                                       struct ubus_request_data *aRequest,
                                       const char *              aMethod,
                                       struct blob_attr *        aMsg)
{
    return GetInstance().UbusSetInformation(aContext, aObj, aRequest, aMethod, aMsg, "extpanid");
}

int UbusServer::UbusPskcHandler(struct ubus_context *     aContext,
                                struct ubus_object *      aObj,
                                struct ubus_request_data *aRequest,
                                const char *              aMethod,
                                struct blob_attr *        aMsg)
{
    return GetInstance().UbusGetInformation(aContext, aObj, aRequest, aMethod, aMsg, "pskc");
}

int UbusServer::UbusSetPskcHandler(struct ubus_context *     aContext,
                                   struct ubus_object *      aObj,
                                   struct ubus_request_data *aRequest,
                                   const char *              aMethod,
                                   struct blob_attr *        aMsg)
{
    return GetInstance().UbusSetInformation(aContext, aObj, aRequest, aMethod, aMsg, "pskc");
}

int UbusServer::UbusNetworkkeyHandler(struct ubus_context *     aContext,
                                      struct ubus_object *      aObj,
                                      struct ubus_request_data *aRequest,
                                      const char *              aMethod,
                                      struct blob_attr *        aMsg)
{
    return GetInstance().UbusGetInformation(aContext, aObj, aRequest, aMethod, aMsg, "networkkey");
}

int UbusServer::UbusSetNetworkkeyHandler(struct ubus_context *     aContext,
                                         struct ubus_object *      aObj,
                                         struct ubus_request_data *aRequest,
                                         const char *              aMethod,
                                         struct blob_attr *        aMsg)
{
    return GetInstance().UbusSetInformation(aContext, aObj, aRequest, aMethod, aMsg, "networkkey");
}

int UbusServer::UbusThreadStartHandler(struct ubus_context *     aContext,
                                       struct ubus_object *      aObj,
                                       struct ubus_request_data *aRequest,
                                       const char *              aMethod,
                                       struct blob_attr *        aMsg)
{
    return GetInstance().UbusThreadHandler(aContext, aObj, aRequest, aMethod, aMsg, "start");
}

int UbusServer::UbusThreadStopHandler(struct ubus_context *     aContext,
                                      struct ubus_object *      aObj,
                                      struct ubus_request_data *aRequest,
                                      const char *              aMethod,
                                      struct blob_attr *        aMsg)
{
    return GetInstance().UbusThreadHandler(aContext, aObj, aRequest, aMethod, aMsg, "stop");
}

int UbusServer::UbusParentHandler(struct ubus_context *     aContext,
                                  struct ubus_object *      aObj,
                                  struct ubus_request_data *aRequest,
                                  const char *              aMethod,
                                  struct blob_attr *        aMsg)
{
    return GetInstance().UbusParentHandlerDetail(aContext, aObj, aRequest, aMethod, aMsg);
}

int UbusServer::UbusNeighborHandler(struct ubus_context *     aContext,
                                    struct ubus_object *      aObj,
                                    struct ubus_request_data *aRequest,
                                    const char *              aMethod,
                                    struct blob_attr *        aMsg)
{
    return GetInstance().UbusNeighborHandlerDetail(aContext, aObj, aRequest, aMethod, aMsg);
}

int UbusServer::UbusModeHandler(struct ubus_context *     aContext,
                                struct ubus_object *      aObj,
                                struct ubus_request_data *aRequest,
                                const char *              aMethod,
                                struct blob_attr *        aMsg)
{
    return GetInstance().UbusGetInformation(aContext, aObj, aRequest, aMethod, aMsg, "mode");
}

int UbusServer::UbusSetModeHandler(struct ubus_context *     aContext,
                                   struct ubus_object *      aObj,
                                   struct ubus_request_data *aRequest,
                                   const char *              aMethod,
                                   struct blob_attr *        aMsg)
{
    return GetInstance().UbusSetInformation(aContext, aObj, aRequest, aMethod, aMsg, "mode");
}

int UbusServer::UbusPartitionIdHandler(struct ubus_context *     aContext,
                                       struct ubus_object *      aObj,
                                       struct ubus_request_data *aRequest,
                                       const char *              aMethod,
                                       struct blob_attr *        aMsg)
{
    return GetInstance().UbusGetInformation(aContext, aObj, aRequest, aMethod, aMsg, "partitionid");
}

int UbusServer::UbusLeaveHandler(struct ubus_context *     aContext,
                                 struct ubus_object *      aObj,
                                 struct ubus_request_data *aRequest,
                                 const char *              aMethod,
                                 struct blob_attr *        aMsg)
{
    return GetInstance().UbusLeaveHandlerDetail(aContext, aObj, aRequest, aMethod, aMsg);
}

int UbusServer::UbusLeaderdataHandler(struct ubus_context *     aContext,
                                      struct ubus_object *      aObj,
                                      struct ubus_request_data *aRequest,
                                      const char *              aMethod,
                                      struct blob_attr *        aMsg)
{
    return GetInstance().UbusGetInformation(aContext, aObj, aRequest, aMethod, aMsg, "leaderdata");
}

int UbusServer::UbusNetworkdataHandler(struct ubus_context *     aContext,
                                       struct ubus_object *      aObj,
                                       struct ubus_request_data *aRequest,
                                       const char *              aMethod,
                                       struct blob_attr *        aMsg)
{
    return GetInstance().UbusGetInformation(aContext, aObj, aRequest, aMethod, aMsg, "networkdata");
}

int UbusServer::UbusCommissionerStartHandler(struct ubus_context *     aContext,
                                             struct ubus_object *      aObj,
                                             struct ubus_request_data *aRequest,
                                             const char *              aMethod,
                                             struct blob_attr *        aMsg)
{
    return GetInstance().UbusCommissioner(aContext, aObj, aRequest, aMethod, aMsg, "start");
}

int UbusServer::UbusJoinerRemoveHandler(struct ubus_context *     aContext,
                                        struct ubus_object *      aObj,
                                        struct ubus_request_data *aRequest,
                                        const char *              aMethod,
                                        struct blob_attr *        aMsg)
{
    return GetInstance().UbusCommissioner(aContext, aObj, aRequest, aMethod, aMsg, "joinerremove");
}

int UbusServer::UbusMgmtsetHandler(struct ubus_context *     aContext,
                                   struct ubus_object *      aObj,
                                   struct ubus_request_data *aRequest,
                                   const char *              aMethod,
                                   struct blob_attr *        aMsg)
{
    return GetInstance().UbusMgmtset(aContext, aObj, aRequest, aMethod, aMsg);
}

int UbusServer::UbusJoinerAddHandler(struct ubus_context *     aContext,
                                     struct ubus_object *      aObj,
                                     struct ubus_request_data *aRequest,
                                     const char *              aMethod,
                                     struct blob_attr *        aMsg)
{
    return GetInstance().UbusCommissioner(aContext, aObj, aRequest, aMethod, aMsg, "joineradd");
}

int UbusServer::UbusMacfilterAddrHandler(struct ubus_context *     aContext,
                                         struct ubus_object *      aObj,
                                         struct ubus_request_data *aRequest,
                                         const char *              aMethod,
                                         struct blob_attr *        aMsg)
{
    return GetInstance().UbusGetInformation(aContext, aObj, aRequest, aMethod, aMsg, "macfilteraddr");
}

int UbusServer::UbusMacfilterStateHandler(struct ubus_context *     aContext,
                                          struct ubus_object *      aObj,
                                          struct ubus_request_data *aRequest,
                                          const char *              aMethod,
                                          struct blob_attr *        aMsg)
{
    return GetInstance().UbusGetInformation(aContext, aObj, aRequest, aMethod, aMsg, "macfilterstate");
}

int UbusServer::UbusMacfilterAddHandler(struct ubus_context *     aContext,
                                        struct ubus_object *      aObj,
                                        struct ubus_request_data *aRequest,
                                        const char *              aMethod,
                                        struct blob_attr *        aMsg)
{
    return GetInstance().UbusSetInformation(aContext, aObj, aRequest, aMethod, aMsg, "macfilteradd");
}

int UbusServer::UbusMacfilterRemoveHandler(struct ubus_context *     aContext,
                                           struct ubus_object *      aObj,
                                           struct ubus_request_data *aRequest,
                                           const char *              aMethod,
                                           struct blob_attr *        aMsg)
{
    return GetInstance().UbusSetInformation(aContext, aObj, aRequest, aMethod, aMsg, "macfilterremove");
}

int UbusServer::UbusMacfilterSetStateHandler(struct ubus_context *     aContext,
                                             struct ubus_object *      aObj,
                                             struct ubus_request_data *aRequest,
                                             const char *              aMethod,
                                             struct blob_attr *        aMsg)
{
    return GetInstance().UbusSetInformation(aContext, aObj, aRequest, aMethod, aMsg, "macfiltersetstate");
}

int UbusServer::UbusMacfilterClearHandler(struct ubus_context *     aContext,
                                          struct ubus_object *      aObj,
                                          struct ubus_request_data *aRequest,
                                          const char *              aMethod,
                                          struct blob_attr *        aMsg)
{
    return GetInstance().UbusSetInformation(aContext, aObj, aRequest, aMethod, aMsg, "macfilterclear");
}

int UbusServer::UbusLeaveHandlerDetail(struct ubus_context *     aContext,
                                       struct ubus_object *      aObj,
                                       struct ubus_request_data *aRequest,
                                       const char *              aMethod,
                                       struct blob_attr *        aMsg)
{
    OT_UNUSED_VARIABLE(aObj);
    OT_UNUSED_VARIABLE(aMethod);
    OT_UNUSED_VARIABLE(aMsg);

    otError  error = OT_ERROR_NONE;
    uint64_t eventNum;
    ssize_t  retval;

    mNcpThreadMutex->lock();
    otInstanceFactoryReset(mController->GetInstance());

    eventNum = 1;
    retval   = write(sUbusEfd, &eventNum, sizeof(uint64_t));
    if (retval != sizeof(uint64_t))
    {
        error = OT_ERROR_FAILED;
        goto exit;
    }

    blob_buf_init(&mBuf, 0);

exit:
    mNcpThreadMutex->unlock();
    AppendResult(error, aContext, aRequest);
    return 0;
}
int UbusServer::UbusThreadHandler(struct ubus_context *     aContext,
                                  struct ubus_object *      aObj,
                                  struct ubus_request_data *aRequest,
                                  const char *              aMethod,
                                  struct blob_attr *        aMsg,
                                  const char *              aAction)
{
    OT_UNUSED_VARIABLE(aObj);
    OT_UNUSED_VARIABLE(aMethod);
    OT_UNUSED_VARIABLE(aMsg);

    otError error = OT_ERROR_NONE;

    blob_buf_init(&mBuf, 0);

    if (!strcmp(aAction, "start"))
    {
        mNcpThreadMutex->lock();
        SuccessOrExit(error = otIp6SetEnabled(mController->GetInstance(), true));
        SuccessOrExit(error = otThreadSetEnabled(mController->GetInstance(), true));
    }
    else if (!strcmp(aAction, "stop"))
    {
        mNcpThreadMutex->lock();
        SuccessOrExit(error = otThreadSetEnabled(mController->GetInstance(), false));
        SuccessOrExit(error = otIp6SetEnabled(mController->GetInstance(), false));
    }

exit:
    mNcpThreadMutex->unlock();
    AppendResult(error, aContext, aRequest);
    return 0;
}

int UbusServer::UbusParentHandlerDetail(struct ubus_context *     aContext,
                                        struct ubus_object *      aObj,
                                        struct ubus_request_data *aRequest,
                                        const char *              aMethod,
                                        struct blob_attr *        aMsg)
{
    OT_UNUSED_VARIABLE(aObj);
    OT_UNUSED_VARIABLE(aMethod);
    OT_UNUSED_VARIABLE(aMsg);

    otError      error = OT_ERROR_NONE;
    otRouterInfo parentInfo;
    char         extAddress[XPANID_LENGTH] = "";
    char         transfer[XPANID_LENGTH]   = "";
    void *       jsonList                  = nullptr;
    void *       jsonArray                 = nullptr;

    blob_buf_init(&mBuf, 0);

    mNcpThreadMutex->lock();
    SuccessOrExit(error = otThreadGetParentInfo(mController->GetInstance(), &parentInfo));

    jsonArray = blobmsg_open_array(&mBuf, "parent_list");
    jsonList  = blobmsg_open_table(&mBuf, "parent");
    blobmsg_add_string(&mBuf, "Role", "R");

    sprintf(transfer, "0x%04x", parentInfo.mRloc16);
    blobmsg_add_string(&mBuf, "Rloc16", transfer);

    sprintf(transfer, "%3d", parentInfo.mAge);
    blobmsg_add_string(&mBuf, "Age", transfer);

    OutputBytes(parentInfo.mExtAddress.m8, sizeof(parentInfo.mExtAddress.m8), extAddress);
    blobmsg_add_string(&mBuf, "ExtAddress", extAddress);

    blobmsg_add_u16(&mBuf, "LinkQualityIn", parentInfo.mLinkQualityIn);

    blobmsg_close_table(&mBuf, jsonList);
    blobmsg_close_array(&mBuf, jsonArray);

exit:
    mNcpThreadMutex->unlock();
    AppendResult(error, aContext, aRequest);
    return error;
}

int UbusServer::UbusNeighborHandlerDetail(struct ubus_context *     aContext,
                                          struct ubus_object *      aObj,
                                          struct ubus_request_data *aRequest,
                                          const char *              aMethod,
                                          struct blob_attr *        aMsg)
{
    OT_UNUSED_VARIABLE(aObj);
    OT_UNUSED_VARIABLE(aMethod);
    OT_UNUSED_VARIABLE(aMsg);

    otError                error = OT_ERROR_NONE;
    otNeighborInfo         neighborInfo;
    otNeighborInfoIterator iterator                  = OT_NEIGHBOR_INFO_ITERATOR_INIT;
    char                   transfer[XPANID_LENGTH]   = "";
    void *                 jsonList                  = nullptr;
    char                   mode[5]                   = "";
    char                   extAddress[XPANID_LENGTH] = "";

    blob_buf_init(&mBuf, 0);

    sJsonUri = blobmsg_open_array(&mBuf, "neighbor_list");

    mNcpThreadMutex->lock();
    while (otThreadGetNextNeighborInfo(mController->GetInstance(), &iterator, &neighborInfo) == OT_ERROR_NONE)
    {
        jsonList = blobmsg_open_table(&mBuf, nullptr);

        blobmsg_add_string(&mBuf, "Role", neighborInfo.mIsChild ? "C" : "R");

        sprintf(transfer, "0x%04x", neighborInfo.mRloc16);
        blobmsg_add_string(&mBuf, "Rloc16", transfer);

        sprintf(transfer, "%3d", neighborInfo.mAge);
        blobmsg_add_string(&mBuf, "Age", transfer);

        sprintf(transfer, "%8d", neighborInfo.mAverageRssi);
        blobmsg_add_string(&mBuf, "AvgRssi", transfer);

        sprintf(transfer, "%9d", neighborInfo.mLastRssi);
        blobmsg_add_string(&mBuf, "LastRssi", transfer);

        if (neighborInfo.mRxOnWhenIdle)
        {
            strcat(mode, "r");
        }

        if (neighborInfo.mFullThreadDevice)
        {
            strcat(mode, "d");
        }

        if (neighborInfo.mFullNetworkData)
        {
            strcat(mode, "n");
        }
        blobmsg_add_string(&mBuf, "Mode", mode);

        OutputBytes(neighborInfo.mExtAddress.m8, sizeof(neighborInfo.mExtAddress.m8), extAddress);
        blobmsg_add_string(&mBuf, "ExtAddress", extAddress);

        blobmsg_add_u16(&mBuf, "LinkQualityIn", neighborInfo.mLinkQualityIn);

        blobmsg_close_table(&mBuf, jsonList);

        memset(mode, 0, sizeof(mode));
        memset(extAddress, 0, sizeof(extAddress));
    }

    blobmsg_close_array(&mBuf, sJsonUri);

    mNcpThreadMutex->unlock();

    AppendResult(error, aContext, aRequest);
    return 0;
}

int UbusServer::UbusMgmtset(struct ubus_context *     aContext,
                            struct ubus_object *      aObj,
                            struct ubus_request_data *aRequest,
                            const char *              aMethod,
                            struct blob_attr *        aMsg)
{
    OT_UNUSED_VARIABLE(aObj);
    OT_UNUSED_VARIABLE(aMethod);
    OT_UNUSED_VARIABLE(aMsg);

    otError              error = OT_ERROR_NONE;
    struct blob_attr *   tb[MGMTSET_MAX];
    otOperationalDataset dataset;
    uint8_t              tlvs[128];
    long                 value;
    int                  length = 0;

    SuccessOrExit(error = otDatasetGetActive(mController->GetInstance(), &dataset));

    blobmsg_parse(mgmtsetPolicy, MGMTSET_MAX, tb, blob_data(aMsg), blob_len(aMsg));
    if (tb[NETWORKKEY] != nullptr)
    {
        dataset.mComponents.mIsNetworkKeyPresent = true;
        VerifyOrExit((length = Hex2Bin(blobmsg_get_string(tb[NETWORKKEY]), dataset.mNetworkKey.m8,
                                       sizeof(dataset.mNetworkKey.m8))) == OT_NETWORK_KEY_SIZE,
                     error = OT_ERROR_PARSE);
        length = 0;
    }
    if (tb[NETWORKNAME] != nullptr)
    {
        dataset.mComponents.mIsNetworkNamePresent = true;
        VerifyOrExit((length = static_cast<int>(strlen(blobmsg_get_string(tb[NETWORKNAME])))) <=
                         OT_NETWORK_NAME_MAX_SIZE,
                     error = OT_ERROR_PARSE);
        memset(&dataset.mNetworkName, 0, sizeof(dataset.mNetworkName));
        memcpy(dataset.mNetworkName.m8, blobmsg_get_string(tb[NETWORKNAME]), static_cast<size_t>(length));
        length = 0;
    }
    if (tb[EXTPANID] != nullptr)
    {
        dataset.mComponents.mIsExtendedPanIdPresent = true;
        VerifyOrExit(Hex2Bin(blobmsg_get_string(tb[EXTPANID]), dataset.mExtendedPanId.m8,
                             sizeof(dataset.mExtendedPanId.m8)) >= 0,
                     error = OT_ERROR_PARSE);
    }
    if (tb[PANID] != nullptr)
    {
        dataset.mComponents.mIsPanIdPresent = true;
        SuccessOrExit(error = ParseLong(blobmsg_get_string(tb[PANID]), value));
        dataset.mPanId = static_cast<otPanId>(value);
    }
    if (tb[CHANNEL] != nullptr)
    {
        dataset.mComponents.mIsChannelPresent = true;
        SuccessOrExit(error = ParseLong(blobmsg_get_string(tb[CHANNEL]), value));
        dataset.mChannel = static_cast<uint16_t>(value);
    }
    if (tb[PSKC] != nullptr)
    {
        dataset.mComponents.mIsPskcPresent = true;
        VerifyOrExit((length = Hex2Bin(blobmsg_get_string(tb[PSKC]), dataset.mPskc.m8, sizeof(dataset.mPskc.m8))) ==
                         OT_PSKC_MAX_SIZE,
                     error = OT_ERROR_PARSE);
        length = 0;
    }
    dataset.mActiveTimestamp.mSeconds++;
    if (otCommissionerGetState(mController->GetInstance()) == OT_COMMISSIONER_STATE_DISABLED)
    {
        otCommissionerStop(mController->GetInstance());
    }
    SuccessOrExit(error = otDatasetSendMgmtActiveSet(mController->GetInstance(), &dataset, tlvs,
                                                     static_cast<uint8_t>(length), /* aCallback */ nullptr,
                                                     /* aContext */ nullptr));
exit:
    AppendResult(error, aContext, aRequest);
    return 0;
}

int UbusServer::UbusCommissioner(struct ubus_context *     aContext,
                                 struct ubus_object *      aObj,
                                 struct ubus_request_data *aRequest,
                                 const char *              aMethod,
                                 struct blob_attr *        aMsg,
                                 const char *              aAction)
{
    OT_UNUSED_VARIABLE(aObj);
    OT_UNUSED_VARIABLE(aMethod);
    OT_UNUSED_VARIABLE(aMsg);

    otError error = OT_ERROR_NONE;

    mNcpThreadMutex->lock();

    if (!strcmp(aAction, "start"))
    {
        if (otCommissionerGetState(mController->GetInstance()) == OT_COMMISSIONER_STATE_DISABLED)
        {
            error = otCommissionerStart(mController->GetInstance(), &UbusServer::HandleStateChanged,
                                        &UbusServer::HandleJoinerEvent, this);
        }
    }
    else if (!strcmp(aAction, "joineradd"))
    {
        struct blob_attr *  tb[ADD_JOINER_MAX];
        otExtAddress        addr;
        const otExtAddress *addrPtr = nullptr;
        char *              pskd    = nullptr;

        blobmsg_parse(addJoinerPolicy, ADD_JOINER_MAX, tb, blob_data(aMsg), blob_len(aMsg));
        if (tb[PSKD] != nullptr)
        {
            pskd = blobmsg_get_string(tb[PSKD]);
        }
        if (tb[EUI64] != nullptr)
        {
            if (!strcmp(blobmsg_get_string(tb[EUI64]), "*"))
            {
                addrPtr = nullptr;
                memset(&addr, 0, sizeof(addr));
            }
            else
            {
                VerifyOrExit(Hex2Bin(blobmsg_get_string(tb[EUI64]), addr.m8, sizeof(addr)) == sizeof(addr),
                             error = OT_ERROR_PARSE);
                addrPtr = &addr;
            }
        }

        unsigned long timeout = kDefaultJoinerTimeout;
        SuccessOrExit(
            error = otCommissionerAddJoiner(mController->GetInstance(), addrPtr, pskd, static_cast<uint32_t>(timeout)));
    }
    else if (!strcmp(aAction, "joinerremove"))
    {
        struct blob_attr *  tb[SET_NETWORK_MAX];
        otExtAddress        addr;
        const otExtAddress *addrPtr = nullptr;

        blobmsg_parse(removeJoinerPolicy, SET_NETWORK_MAX, tb, blob_data(aMsg), blob_len(aMsg));
        if (tb[SETNETWORK] != nullptr)
        {
            if (strcmp(blobmsg_get_string(tb[SETNETWORK]), "*") == 0)
            {
                addrPtr = nullptr;
            }
            else
            {
                VerifyOrExit(Hex2Bin(blobmsg_get_string(tb[SETNETWORK]), addr.m8, sizeof(addr)) == sizeof(addr),
                             error = OT_ERROR_PARSE);
                addrPtr = &addr;
            }
        }

        SuccessOrExit(error = otCommissionerRemoveJoiner(mController->GetInstance(), addrPtr));
    }

exit:
    mNcpThreadMutex->unlock();
    blob_buf_init(&mBuf, 0);
    AppendResult(error, aContext, aRequest);
    return 0;
}

void UbusServer::HandleStateChanged(otCommissionerState aState, void *aContext)
{
    static_cast<UbusServer *>(aContext)->HandleStateChanged(aState);
}

void UbusServer::HandleStateChanged(otCommissionerState aState)
{
    switch (aState)
    {
    case OT_COMMISSIONER_STATE_DISABLED:
        otbrLogInfo("Commissioner state disabled");
        break;
    case OT_COMMISSIONER_STATE_ACTIVE:
        otbrLogInfo("Commissioner state active");
        break;
    case OT_COMMISSIONER_STATE_PETITION:
        otbrLogInfo("Commissioner state petition");
        break;
    }
}

void UbusServer::HandleJoinerEvent(otCommissionerJoinerEvent aEvent,
                                   const otJoinerInfo *      aJoinerInfo,
                                   const otExtAddress *      aJoinerId,
                                   void *                    aContext)
{
    static_cast<UbusServer *>(aContext)->HandleJoinerEvent(aEvent, aJoinerInfo, aJoinerId);
}

void UbusServer::HandleJoinerEvent(otCommissionerJoinerEvent aEvent,
                                   const otJoinerInfo *      aJoinerInfo,
                                   const otExtAddress *      aJoinerId)
{
    OT_UNUSED_VARIABLE(aJoinerInfo);
    OT_UNUSED_VARIABLE(aJoinerId);

    switch (aEvent)
    {
    case OT_COMMISSIONER_JOINER_START:
        otbrLogInfo("Joiner start");
        break;
    case OT_COMMISSIONER_JOINER_CONNECTED:
        otbrLogInfo("Joiner connected");
        break;
    case OT_COMMISSIONER_JOINER_FINALIZE:
        otbrLogInfo("Joiner finalize");
        break;
    case OT_COMMISSIONER_JOINER_END:
        otbrLogInfo("Joiner end");
        break;
    case OT_COMMISSIONER_JOINER_REMOVED:
        otbrLogInfo("Joiner remove");
        break;
    }
}

int UbusServer::UbusGetInformation(struct ubus_context *     aContext,
                                   struct ubus_object *      aObj,
                                   struct ubus_request_data *aRequest,
                                   const char *              aMethod,
                                   struct blob_attr *        aMsg,
                                   const char *              aAction)
{
    OT_UNUSED_VARIABLE(aObj);
    OT_UNUSED_VARIABLE(aMethod);
    OT_UNUSED_VARIABLE(aMsg);

    otError error = OT_ERROR_NONE;

    blob_buf_init(&mBuf, 0);

    mNcpThreadMutex->lock();
    if (!strcmp(aAction, "networkname"))
        blobmsg_add_string(&mBuf, "NetworkName", otThreadGetNetworkName(mController->GetInstance()));
    else if (!strcmp(aAction, "state"))
    {
        char state[10];
        GetState(mController->GetInstance(), state);
        blobmsg_add_string(&mBuf, "State", state);
    }
    else if (!strcmp(aAction, "channel"))
        blobmsg_add_u32(&mBuf, "Channel", otLinkGetChannel(mController->GetInstance()));
    else if (!strcmp(aAction, "panid"))
    {
        char panIdString[PANID_LENGTH];
        sprintf(panIdString, "0x%04x", otLinkGetPanId(mController->GetInstance()));
        blobmsg_add_string(&mBuf, "PanId", panIdString);
    }
    else if (!strcmp(aAction, "rloc16"))
    {
        char rloc[PANID_LENGTH];
        sprintf(rloc, "0x%04x", otThreadGetRloc16(mController->GetInstance()));
        blobmsg_add_string(&mBuf, "rloc16", rloc);
    }
    else if (!strcmp(aAction, "networkkey"))
    {
        char         outputKey[NETWORKKEY_LENGTH] = "";
        otNetworkKey key;

        otThreadGetNetworkKey(mController->GetInstance(), &key);
        OutputBytes(key.m8, OT_NETWORK_KEY_SIZE, outputKey);
        blobmsg_add_string(&mBuf, "Networkkey", outputKey);
    }
    else if (!strcmp(aAction, "pskc"))
    {
        char   outputPskc[NETWORKKEY_LENGTH] = "";
        otPskc pskc;

        otThreadGetPskc(mController->GetInstance(), &pskc);
        OutputBytes(pskc.m8, OT_PSKC_MAX_SIZE, outputPskc);
        blobmsg_add_string(&mBuf, "pskc", outputPskc);
    }
    else if (!strcmp(aAction, "extpanid"))
    {
        char           outputExtPanId[XPANID_LENGTH] = "";
        const uint8_t *extPanId =
            reinterpret_cast<const uint8_t *>(otThreadGetExtendedPanId(mController->GetInstance()));
        OutputBytes(extPanId, OT_EXT_PAN_ID_SIZE, outputExtPanId);
        blobmsg_add_string(&mBuf, "ExtPanId", outputExtPanId);
    }
    else if (!strcmp(aAction, "mode"))
    {
        otLinkModeConfig linkMode;
        char             mode[5] = "";

        memset(&linkMode, 0, sizeof(otLinkModeConfig));

        linkMode = otThreadGetLinkMode(mController->GetInstance());

        if (linkMode.mRxOnWhenIdle)
        {
            strcat(mode, "r");
        }

        if (linkMode.mDeviceType)
        {
            strcat(mode, "d");
        }

        if (linkMode.mNetworkData)
        {
            strcat(mode, "n");
        }
        blobmsg_add_string(&mBuf, "Mode", mode);
    }
    else if (!strcmp(aAction, "partitionid"))
    {
        blobmsg_add_u32(&mBuf, "Partitionid", otThreadGetPartitionId(mController->GetInstance()));
    }
    else if (!strcmp(aAction, "leaderdata"))
    {
        otLeaderData leaderData;

        SuccessOrExit(error = otThreadGetLeaderData(mController->GetInstance(), &leaderData));

        sJsonUri = blobmsg_open_table(&mBuf, "leaderdata");

        blobmsg_add_u32(&mBuf, "PartitionId", leaderData.mPartitionId);
        blobmsg_add_u32(&mBuf, "Weighting", leaderData.mWeighting);
        blobmsg_add_u32(&mBuf, "DataVersion", leaderData.mDataVersion);
        blobmsg_add_u32(&mBuf, "StableDataVersion", leaderData.mStableDataVersion);
        blobmsg_add_u32(&mBuf, "LeaderRouterId", leaderData.mLeaderRouterId);

        blobmsg_close_table(&mBuf, sJsonUri);
    }
    else if (!strcmp(aAction, "networkdata"))
    {
        ubus_send_reply(aContext, aRequest, mNetworkdataBuf.head);
        if (time(nullptr) - mSecond > 10)
        {
            struct otIp6Address address;
            uint8_t             tlvTypes[OT_NETWORK_DIAGNOSTIC_TYPELIST_MAX_ENTRIES];
            uint8_t             count             = 0;
            char                multicastAddr[10] = "ff03::2";

            blob_buf_init(&mNetworkdataBuf, 0);

            SuccessOrExit(error = otIp6AddressFromString(multicastAddr, &address));

            tlvTypes[count++] = static_cast<uint8_t>(OT_NETWORK_DIAGNOSTIC_TLV_ROUTE);
            tlvTypes[count++] = static_cast<uint8_t>(OT_NETWORK_DIAGNOSTIC_TLV_CHILD_TABLE);

            sBufNum = 0;
            otThreadSendDiagnosticGet(mController->GetInstance(), &address, tlvTypes, count,
                                      &UbusServer::HandleDiagnosticGetResponse, this);
            mSecond = time(nullptr);
        }
        goto exit;
    }
    else if (!strcmp(aAction, "joinernum"))
    {
        void *       jsonTable = nullptr;
        void *       jsonArray = nullptr;
        otJoinerInfo joinerInfo;
        uint16_t     iterator        = 0;
        int          joinerNum       = 0;
        char         eui64[EXTPANID] = "";

        blob_buf_init(&mBuf, 0);

        jsonArray = blobmsg_open_array(&mBuf, "joinerList");
        while (otCommissionerGetNextJoinerInfo(mController->GetInstance(), &iterator, &joinerInfo) == OT_ERROR_NONE)
        {
            memset(eui64, 0, sizeof(eui64));

            jsonTable = blobmsg_open_table(&mBuf, nullptr);

            blobmsg_add_string(&mBuf, "pskd", joinerInfo.mPskd.m8);

            switch (joinerInfo.mType)
            {
            case OT_JOINER_INFO_TYPE_ANY:
                blobmsg_add_u16(&mBuf, "isAny", 1);
                break;
            case OT_JOINER_INFO_TYPE_EUI64:
                blobmsg_add_u16(&mBuf, "isAny", 0);
                OutputBytes(joinerInfo.mSharedId.mEui64.m8, sizeof(joinerInfo.mSharedId.mEui64.m8), eui64);
                blobmsg_add_string(&mBuf, "eui64", eui64);
                break;
            case OT_JOINER_INFO_TYPE_DISCERNER:
                blobmsg_add_u16(&mBuf, "isAny", 0);
                blobmsg_add_u64(&mBuf, "discernerValue", joinerInfo.mSharedId.mDiscerner.mValue);
                blobmsg_add_u16(&mBuf, "discernerLength", joinerInfo.mSharedId.mDiscerner.mLength);
                break;
            }

            blobmsg_close_table(&mBuf, jsonTable);

            joinerNum++;
        }
        blobmsg_close_array(&mBuf, jsonArray);

        blobmsg_add_u32(&mBuf, "joinernum", joinerNum);
    }
    else if (!strcmp(aAction, "macfilterstate"))
    {
        otMacFilterAddressMode mode = otLinkFilterGetAddressMode(mController->GetInstance());

        blob_buf_init(&mBuf, 0);

        if (mode == OT_MAC_FILTER_ADDRESS_MODE_DISABLED)
        {
            blobmsg_add_string(&mBuf, "state", "disable");
        }
        else if (mode == OT_MAC_FILTER_ADDRESS_MODE_ALLOWLIST)
        {
            blobmsg_add_string(&mBuf, "state", "allowlist");
        }
        else if (mode == OT_MAC_FILTER_ADDRESS_MODE_DENYLIST)
        {
            blobmsg_add_string(&mBuf, "state", "denylist");
        }
        else
        {
            blobmsg_add_string(&mBuf, "state", "error");
        }
    }
    else if (!strcmp(aAction, "macfilteraddr"))
    {
        otMacFilterEntry    entry;
        otMacFilterIterator iterator = OT_MAC_FILTER_ITERATOR_INIT;

        blob_buf_init(&mBuf, 0);

        sJsonUri = blobmsg_open_array(&mBuf, "addrlist");

        while (otLinkFilterGetNextAddress(mController->GetInstance(), &iterator, &entry) == OT_ERROR_NONE)
        {
            char extAddress[XPANID_LENGTH] = "";
            OutputBytes(entry.mExtAddress.m8, sizeof(entry.mExtAddress.m8), extAddress);
            blobmsg_add_string(&mBuf, "addr", extAddress);
        }

        blobmsg_close_array(&mBuf, sJsonUri);
    }
    else
    {
        perror("invalid argument in get information ubus\n");
    }

    AppendResult(error, aContext, aRequest);
exit:
    mNcpThreadMutex->unlock();
    return 0;
}

void UbusServer::HandleDiagnosticGetResponse(otError              aError,
                                             otMessage *          aMessage,
                                             const otMessageInfo *aMessageInfo,
                                             void *               aContext)
{
    static_cast<UbusServer *>(aContext)->HandleDiagnosticGetResponse(aError, aMessage, aMessageInfo);
}

static bool IsRoutingLocator(const otIp6Address *aAddress)
{
    enum
    {
        kAloc16Mask            = 0xfc, ///< The mask for Aloc16.
        kRloc16ReservedBitMask = 0x02, ///< The mask for the reserved bit of Rloc16.
    };

    return (aAddress->mFields.m32[2] == htonl(0x000000ff) && aAddress->mFields.m16[6] == htons(0xfe00) &&
            aAddress->mFields.m8[14] < kAloc16Mask && (aAddress->mFields.m8[14] & kRloc16ReservedBitMask) == 0);
}

void UbusServer::HandleDiagnosticGetResponse(otError aError, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    uint16_t              rloc16;
    uint16_t              sockRloc16 = 0;
    void *                jsonArray  = nullptr;
    void *                jsonItem   = nullptr;
    char                  xrloc[10];
    otNetworkDiagTlv      diagTlv;
    otNetworkDiagIterator iterator = OT_NETWORK_DIAGNOSTIC_ITERATOR_INIT;

    SuccessOrExit(aError);

    char networkdata[20];
    sprintf(networkdata, "networkdata%d", sBufNum);
    sJsonUri = blobmsg_open_table(&mNetworkdataBuf, networkdata);
    sBufNum++;

    if (IsRoutingLocator(&aMessageInfo->mSockAddr))
    {
        sockRloc16 = ntohs(aMessageInfo->mPeerAddr.mFields.m16[7]);
        sprintf(xrloc, "0x%04x", sockRloc16);
        blobmsg_add_string(&mNetworkdataBuf, "rloc", xrloc);
    }

    while (otThreadGetNextDiagnosticTlv(aMessage, &iterator, &diagTlv) == OT_ERROR_NONE)
    {
        switch (diagTlv.mType)
        {
        case OT_NETWORK_DIAGNOSTIC_TLV_ROUTE:
        {
            const otNetworkDiagRoute &route = diagTlv.mData.mRoute;

            jsonArray = blobmsg_open_array(&mNetworkdataBuf, "routedata");

            for (uint16_t i = 0; i < route.mRouteCount; ++i)
            {
                uint8_t in, out;
                in  = route.mRouteData[i].mLinkQualityIn;
                out = route.mRouteData[i].mLinkQualityOut;
                if (in != 0 && out != 0)
                {
                    jsonItem = blobmsg_open_table(&mNetworkdataBuf, "router");
                    rloc16   = route.mRouteData[i].mRouterId << 10;
                    blobmsg_add_u32(&mNetworkdataBuf, "routerid", route.mRouteData[i].mRouterId);
                    sprintf(xrloc, "0x%04x", rloc16);
                    blobmsg_add_string(&mNetworkdataBuf, "rloc", xrloc);
                    blobmsg_close_table(&mNetworkdataBuf, jsonItem);
                }
            }
            blobmsg_close_array(&mNetworkdataBuf, jsonArray);
            break;
        }

        case OT_NETWORK_DIAGNOSTIC_TLV_CHILD_TABLE:
        {
            jsonArray = blobmsg_open_array(&mNetworkdataBuf, "childdata");
            for (uint16_t i = 0; i < diagTlv.mData.mChildTable.mCount; ++i)
            {
                enum
                {
                    kModeRxOnWhenIdle     = 1 << 3, ///< If the device has its receiver on when not transmitting.
                    kModeFullThreadDevice = 1 << 1, ///< If the device is an FTD.
                    kModeFullNetworkData  = 1 << 0, ///< If the device requires the full Network Data.
                };
                const otNetworkDiagChildEntry &entry = diagTlv.mData.mChildTable.mTable[i];

                uint8_t mode = 0;

                jsonItem = blobmsg_open_table(&mNetworkdataBuf, "child");
                sprintf(xrloc, "0x%04x", (sockRloc16 | entry.mChildId));
                blobmsg_add_string(&mNetworkdataBuf, "rloc", xrloc);

                mode = (entry.mMode.mRxOnWhenIdle ? kModeRxOnWhenIdle : 0) |
                       (entry.mMode.mDeviceType ? kModeFullThreadDevice : 0) |
                       (entry.mMode.mNetworkData ? kModeFullNetworkData : 0);
                blobmsg_add_u16(&mNetworkdataBuf, "mode", mode);
                blobmsg_close_table(&mNetworkdataBuf, jsonItem);
            }
            blobmsg_close_array(&mNetworkdataBuf, jsonArray);
            break;
        }

        default:
            // Ignore other network diagnostics data.
            break;
        }
    }

    blobmsg_close_table(&mNetworkdataBuf, sJsonUri);

exit:
    if (aError != OT_ERROR_NONE)
    {
        otbrLogWarning("Failed to receive diagnostic response: %s", otThreadErrorToString(aError));
    }
}

int UbusServer::UbusSetInformation(struct ubus_context *     aContext,
                                   struct ubus_object *      aObj,
                                   struct ubus_request_data *aRequest,
                                   const char *              aMethod,
                                   struct blob_attr *        aMsg,
                                   const char *              aAction)
{
    OT_UNUSED_VARIABLE(aObj);
    OT_UNUSED_VARIABLE(aMethod);
    OT_UNUSED_VARIABLE(aMsg);

    otError error = OT_ERROR_NONE;

    blob_buf_init(&mBuf, 0);

    mNcpThreadMutex->lock();
    if (!strcmp(aAction, "networkname"))
    {
        struct blob_attr *tb[SET_NETWORK_MAX];

        blobmsg_parse(setNetworknamePolicy, SET_NETWORK_MAX, tb, blob_data(aMsg), blob_len(aMsg));
        if (tb[SETNETWORK] != nullptr)
        {
            char *newName = blobmsg_get_string(tb[SETNETWORK]);
            SuccessOrExit(error = otThreadSetNetworkName(mController->GetInstance(), newName));
        }
    }
    else if (!strcmp(aAction, "channel"))
    {
        struct blob_attr *tb[SET_NETWORK_MAX];

        blobmsg_parse(setChannelPolicy, SET_NETWORK_MAX, tb, blob_data(aMsg), blob_len(aMsg));
        if (tb[SETNETWORK] != nullptr)
        {
            uint32_t channel = blobmsg_get_u32(tb[SETNETWORK]);
            SuccessOrExit(error = otLinkSetChannel(mController->GetInstance(), static_cast<uint8_t>(channel)));
        }
    }
    else if (!strcmp(aAction, "panid"))
    {
        struct blob_attr *tb[SET_NETWORK_MAX];

        blobmsg_parse(setPanIdPolicy, SET_NETWORK_MAX, tb, blob_data(aMsg), blob_len(aMsg));
        if (tb[SETNETWORK] != nullptr)
        {
            long  value;
            char *panid = blobmsg_get_string(tb[SETNETWORK]);
            SuccessOrExit(error = ParseLong(panid, value));
            error = otLinkSetPanId(mController->GetInstance(), static_cast<otPanId>(value));
        }
    }
    else if (!strcmp(aAction, "networkkey"))
    {
        struct blob_attr *tb[SET_NETWORK_MAX];

        blobmsg_parse(setNetworkkeyPolicy, SET_NETWORK_MAX, tb, blob_data(aMsg), blob_len(aMsg));
        if (tb[SETNETWORK] != nullptr)
        {
            otNetworkKey key;
            char *       networkkey = blobmsg_get_string(tb[SETNETWORK]);

            VerifyOrExit(Hex2Bin(networkkey, key.m8, sizeof(key.m8)) == OT_NETWORK_KEY_SIZE, error = OT_ERROR_PARSE);
            SuccessOrExit(error = otThreadSetNetworkKey(mController->GetInstance(), &key));
        }
    }
    else if (!strcmp(aAction, "pskc"))
    {
        struct blob_attr *tb[SET_NETWORK_MAX];

        blobmsg_parse(setPskcPolicy, SET_NETWORK_MAX, tb, blob_data(aMsg), blob_len(aMsg));
        if (tb[SETNETWORK] != nullptr)
        {
            otPskc pskc;

            VerifyOrExit(Hex2Bin(blobmsg_get_string(tb[SETNETWORK]), pskc.m8, sizeof(pskc)) == OT_PSKC_MAX_SIZE,
                         error = OT_ERROR_PARSE);
            SuccessOrExit(error = otThreadSetPskc(mController->GetInstance(), &pskc));
        }
    }
    else if (!strcmp(aAction, "extpanid"))
    {
        struct blob_attr *tb[SET_NETWORK_MAX];

        blobmsg_parse(setExtPanIdPolicy, SET_NETWORK_MAX, tb, blob_data(aMsg), blob_len(aMsg));
        if (tb[SETNETWORK] != nullptr)
        {
            otExtendedPanId extPanId;
            char *          input = blobmsg_get_string(tb[SETNETWORK]);
            VerifyOrExit(Hex2Bin(input, extPanId.m8, sizeof(extPanId)) >= 0, error = OT_ERROR_PARSE);
            error = otThreadSetExtendedPanId(mController->GetInstance(), &extPanId);
        }
    }
    else if (!strcmp(aAction, "mode"))
    {
        otLinkModeConfig  linkMode;
        struct blob_attr *tb[SET_NETWORK_MAX];

        blobmsg_parse(setModePolicy, SET_NETWORK_MAX, tb, blob_data(aMsg), blob_len(aMsg));
        if (tb[SETNETWORK] != nullptr)
        {
            char *inputMode = blobmsg_get_string(tb[SETNETWORK]);
            for (char *ch = inputMode; *ch != '\0'; ch++)
            {
                switch (*ch)
                {
                case 'r':
                    linkMode.mRxOnWhenIdle = 1;
                    break;

                case 'd':
                    linkMode.mDeviceType = 1;
                    break;

                case 'n':
                    linkMode.mNetworkData = 1;
                    break;

                default:
                    ExitNow(error = OT_ERROR_PARSE);
                }
            }

            SuccessOrExit(error = otThreadSetLinkMode(mController->GetInstance(), linkMode));
        }
    }
    else if (!strcmp(aAction, "macfilteradd"))
    {
        struct blob_attr *tb[SET_NETWORK_MAX];
        otExtAddress      extAddr;

        blobmsg_parse(macfilterAddPolicy, SET_NETWORK_MAX, tb, blob_data(aMsg), blob_len(aMsg));
        if (tb[SETNETWORK] != nullptr)
        {
            char *addr = blobmsg_get_string(tb[SETNETWORK]);

            VerifyOrExit(Hex2Bin(addr, extAddr.m8, OT_EXT_ADDRESS_SIZE) == OT_EXT_ADDRESS_SIZE, error = OT_ERROR_PARSE);

            error = otLinkFilterAddAddress(mController->GetInstance(), &extAddr);

            VerifyOrExit(error == OT_ERROR_NONE || error == OT_ERROR_ALREADY);
        }
    }
    else if (!strcmp(aAction, "macfilterremove"))
    {
        struct blob_attr *tb[SET_NETWORK_MAX];
        otExtAddress      extAddr;

        blobmsg_parse(macfilterRemovePolicy, SET_NETWORK_MAX, tb, blob_data(aMsg), blob_len(aMsg));
        if (tb[SETNETWORK] != nullptr)
        {
            char *addr = blobmsg_get_string(tb[SETNETWORK]);
            VerifyOrExit(Hex2Bin(addr, extAddr.m8, OT_EXT_ADDRESS_SIZE) == OT_EXT_ADDRESS_SIZE, error = OT_ERROR_PARSE);

            otLinkFilterRemoveAddress(mController->GetInstance(), &extAddr);
        }
    }
    else if (!strcmp(aAction, "macfiltersetstate"))
    {
        struct blob_attr *tb[SET_NETWORK_MAX];

        blobmsg_parse(macfilterSetStatePolicy, SET_NETWORK_MAX, tb, blob_data(aMsg), blob_len(aMsg));
        if (tb[SETNETWORK] != nullptr)
        {
            char *state = blobmsg_get_string(tb[SETNETWORK]);

            if (strcmp(state, "disable") == 0)
            {
                otLinkFilterSetAddressMode(mController->GetInstance(), OT_MAC_FILTER_ADDRESS_MODE_DISABLED);
            }
            else if (strcmp(state, "allowlist") == 0)
            {
                otLinkFilterSetAddressMode(mController->GetInstance(), OT_MAC_FILTER_ADDRESS_MODE_ALLOWLIST);
            }
            else if (strcmp(state, "denylist") == 0)
            {
                otLinkFilterSetAddressMode(mController->GetInstance(), OT_MAC_FILTER_ADDRESS_MODE_DENYLIST);
            }
        }
    }
    else if (!strcmp(aAction, "macfilterclear"))
    {
        otLinkFilterClearAddresses(mController->GetInstance());
    }
    else
    {
        perror("invalid argument in get information ubus\n");
    }

exit:
    mNcpThreadMutex->unlock();
    AppendResult(error, aContext, aRequest);
    return 0;
}

void UbusServer::GetState(otInstance *aInstance, char *aState)
{
    switch (otThreadGetDeviceRole(aInstance))
    {
    case OT_DEVICE_ROLE_DISABLED:
        strcpy(aState, "disabled");
        break;

    case OT_DEVICE_ROLE_DETACHED:
        strcpy(aState, "detached");
        break;

    case OT_DEVICE_ROLE_CHILD:
        strcpy(aState, "child");
        break;

    case OT_DEVICE_ROLE_ROUTER:
        strcpy(aState, "router");
        break;

    case OT_DEVICE_ROLE_LEADER:
        strcpy(aState, "leader");
        break;
    default:
        strcpy(aState, "invalid aState");
        break;
    }
}

void UbusServer::UbusAddFd()
{
    // ubus library function
    ubus_add_uloop(mContext);

#ifdef FD_CLOEXEC
    fcntl(mContext->sock.fd, F_SETFD, fcntl(mContext->sock.fd, F_GETFD) | FD_CLOEXEC);
#endif
}

void UbusServer::UbusReconnTimer(struct uloop_timeout *aTimeout)
{
    GetInstance().UbusReconnTimerDetail(aTimeout);
}

void UbusServer::UbusReconnTimerDetail(struct uloop_timeout *aTimeout)
{
    OT_UNUSED_VARIABLE(aTimeout);

    static struct uloop_timeout retry = {
        list : {},
        pending : false,
        cb : UbusReconnTimer,
        time : {},
    };
    int time = 2;

    if (ubus_reconnect(mContext, mSockPath) != 0)
    {
        uloop_timeout_set(&retry, time * 1000);
        return;
    }

    UbusAddFd();
}

void UbusServer::UbusConnectionLost(struct ubus_context *aContext)
{
    OT_UNUSED_VARIABLE(aContext);

    UbusReconnTimer(nullptr);
}

int UbusServer::DisplayUbusInit(const char *aPath)
{
    uloop_init();
    signal(SIGPIPE, SIG_IGN);

    mSockPath = aPath;

    mContext = ubus_connect(aPath);
    if (!mContext)
    {
        otbrLogErr("Ubus connect failed");
        return -1;
    }

    otbrLogInfo("Connected as %08x\n", mContext->local_id);
    mContext->connection_lost = UbusConnectionLost;

    /* file description */
    UbusAddFd();

    /* Add a object */
    if (ubus_add_object(mContext, &otbr) != 0)
    {
        otbrLogErr("Ubus add obj failed");
        return -1;
    }

    return 0;
}

void UbusServer::DisplayUbusDone(void)
{
    if (mContext)
    {
        ubus_free(mContext);
        mContext = nullptr;
    }
}

void UbusServer::InstallUbusObject(void)
{
    char *path = nullptr;

    if (-1 == DisplayUbusInit(path))
    {
        otbrLogErr("Ubus connect failed");
        return;
    }

    otbrLogInfo("Uloop run");
    uloop_run();

    DisplayUbusDone();

    uloop_done();
}

otError UbusServer::ParseLong(char *aString, long &aLong)
{
    char *endptr;
    aLong = strtol(aString, &endptr, 0);
    return (*endptr == '\0') ? OT_ERROR_NONE : OT_ERROR_PARSE;
}

int UbusServer::Hex2Bin(const char *aHex, uint8_t *aBin, uint16_t aBinLength)
{
    size_t      hexLength = strlen(aHex);
    const char *hexEnd    = aHex + hexLength;
    uint8_t *   cur       = aBin;
    uint8_t     numChars  = hexLength & 1;
    uint8_t     byte      = 0;
    int         rval;

    VerifyOrExit((hexLength + 1) / 2 <= aBinLength, rval = -1);

    while (aHex < hexEnd)
    {
        if ('A' <= *aHex && *aHex <= 'F')
        {
            byte |= 10 + (*aHex - 'A');
        }
        else if ('a' <= *aHex && *aHex <= 'f')
        {
            byte |= 10 + (*aHex - 'a');
        }
        else if ('0' <= *aHex && *aHex <= '9')
        {
            byte |= *aHex - '0';
        }
        else
        {
            ExitNow(rval = -1);
        }

        aHex++;
        numChars++;

        if (numChars >= 2)
        {
            numChars = 0;
            *cur++   = byte;
            byte     = 0;
        }
        else
        {
            byte <<= 4;
        }
    }

    rval = static_cast<int>(cur - aBin);

exit:
    return rval;
}

void UBusAgent::Init(void)
{
    otbr::ubus::sUbusEfd = eventfd(0, 0);

    otbr::ubus::UbusServer::Initialize(&mNcp, &mThreadMutex);

    if (otbr::ubus::sUbusEfd == -1)
    {
        perror("Failed to create eventfd for ubus");
        exit(EXIT_FAILURE);
    }

    std::thread(UbusServerRun).detach();
}

void UBusAgent::Update(MainloopContext &aMainloop)
{
    VerifyOrExit(otbr::ubus::sUbusEfd != -1);

    FD_SET(otbr::ubus::sUbusEfd, &aMainloop.mReadFdSet);

    if (aMainloop.mMaxFd < otbr::ubus::sUbusEfd)
    {
        aMainloop.mMaxFd = otbr::ubus::sUbusEfd;
    }

exit:
    mThreadMutex.unlock();
    return;
}

void UBusAgent::Process(const MainloopContext &aMainloop)
{
    ssize_t  retval;
    uint64_t num;

    mThreadMutex.lock();

    VerifyOrExit(otbr::ubus::sUbusEfd != -1);

    if (FD_ISSET(otbr::ubus::sUbusEfd, &aMainloop.mReadFdSet))
    {
        retval = read(otbr::ubus::sUbusEfd, &num, sizeof(uint64_t));
        if (retval != sizeof(uint64_t))
        {
            perror("read ubus eventfd failed\n");
            exit(EXIT_FAILURE);
        }
    }

exit:
    return;
}

} // namespace ubus
} // namespace otbr
