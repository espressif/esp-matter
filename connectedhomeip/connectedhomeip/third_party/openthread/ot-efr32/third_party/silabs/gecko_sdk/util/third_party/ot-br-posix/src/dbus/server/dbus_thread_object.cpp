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

#include <assert.h>
#include <string.h>

#include <openthread/border_router.h>
#include <openthread/channel_monitor.h>
#include <openthread/dnssd_server.h>
#include <openthread/instance.h>
#include <openthread/joiner.h>
#include <openthread/link_raw.h>
#include <openthread/ncp.h>
#include <openthread/netdata.h>
#include <openthread/srp_server.h>
#include <openthread/thread_ftd.h>
#include <openthread/platform/radio.h>

#include "common/byteswap.hpp"
#include "dbus/common/constants.hpp"
#include "dbus/server/dbus_agent.hpp"
#include "dbus/server/dbus_thread_object.hpp"

#if OTBR_ENABLE_LEGACY
#include <ot-legacy-pairing-ext.h>
#endif

using std::placeholders::_1;
using std::placeholders::_2;

static std::string GetDeviceRoleName(otDeviceRole aRole)
{
    std::string roleName;

    switch (aRole)
    {
    case OT_DEVICE_ROLE_DISABLED:
        roleName = OTBR_ROLE_NAME_DISABLED;
        break;
    case OT_DEVICE_ROLE_DETACHED:
        roleName = OTBR_ROLE_NAME_DETACHED;
        break;
    case OT_DEVICE_ROLE_CHILD:
        roleName = OTBR_ROLE_NAME_CHILD;
        break;
    case OT_DEVICE_ROLE_ROUTER:
        roleName = OTBR_ROLE_NAME_ROUTER;
        break;
    case OT_DEVICE_ROLE_LEADER:
        roleName = OTBR_ROLE_NAME_LEADER;
        break;
    }

    return roleName;
}

static uint64_t ConvertOpenThreadUint64(const uint8_t *aValue)
{
    uint64_t val = 0;

    for (size_t i = 0; i < sizeof(uint64_t); i++)
    {
        val = (val << 8) | aValue[i];
    }
    return val;
}

namespace otbr {
namespace DBus {

DBusThreadObject::DBusThreadObject(DBusConnection *                 aConnection,
                                   const std::string &              aInterfaceName,
                                   otbr::Ncp::ControllerOpenThread *aNcp,
                                   Mdns::Publisher *                aPublisher)
    : DBusObject(aConnection, OTBR_DBUS_OBJECT_PREFIX + aInterfaceName)
    , mNcp(aNcp)
    , mPublisher(aPublisher)
{
}

otbrError DBusThreadObject::Init(void)
{
    otbrError error        = OTBR_ERROR_NONE;
    auto      threadHelper = mNcp->GetThreadHelper();

    SuccessOrExit(error = DBusObject::Init());

    threadHelper->AddDeviceRoleHandler(std::bind(&DBusThreadObject::DeviceRoleHandler, this, _1));
    threadHelper->AddActiveDatasetChangeHandler(std::bind(&DBusThreadObject::ActiveDatasetChangeHandler, this, _1));
    mNcp->RegisterResetHandler(std::bind(&DBusThreadObject::NcpResetHandler, this));

    RegisterMethod(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_SCAN_METHOD,
                   std::bind(&DBusThreadObject::ScanHandler, this, _1));
    RegisterMethod(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_ENERGY_SCAN_METHOD,
                   std::bind(&DBusThreadObject::EnergyScanHandler, this, _1));
    RegisterMethod(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_ATTACH_METHOD,
                   std::bind(&DBusThreadObject::AttachHandler, this, _1));
    RegisterMethod(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_DETACH_METHOD,
                   std::bind(&DBusThreadObject::DetachHandler, this, _1));
    RegisterMethod(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_FACTORY_RESET_METHOD,
                   std::bind(&DBusThreadObject::FactoryResetHandler, this, _1));
    RegisterMethod(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_RESET_METHOD,
                   std::bind(&DBusThreadObject::ResetHandler, this, _1));
    RegisterMethod(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_JOINER_START_METHOD,
                   std::bind(&DBusThreadObject::JoinerStartHandler, this, _1));
    RegisterMethod(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_JOINER_STOP_METHOD,
                   std::bind(&DBusThreadObject::JoinerStopHandler, this, _1));
    RegisterMethod(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PERMIT_UNSECURE_JOIN_METHOD,
                   std::bind(&DBusThreadObject::PermitUnsecureJoinHandler, this, _1));
    RegisterMethod(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_ADD_ON_MESH_PREFIX_METHOD,
                   std::bind(&DBusThreadObject::AddOnMeshPrefixHandler, this, _1));
    RegisterMethod(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_REMOVE_ON_MESH_PREFIX_METHOD,
                   std::bind(&DBusThreadObject::RemoveOnMeshPrefixHandler, this, _1));
    RegisterMethod(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_ADD_EXTERNAL_ROUTE_METHOD,
                   std::bind(&DBusThreadObject::AddExternalRouteHandler, this, _1));
    RegisterMethod(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_REMOVE_EXTERNAL_ROUTE_METHOD,
                   std::bind(&DBusThreadObject::RemoveExternalRouteHandler, this, _1));
    RegisterMethod(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_ATTACH_ALL_NODES_TO_METHOD,
                   std::bind(&DBusThreadObject::AttachAllNodesToHandler, this, _1));
    RegisterMethod(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_UPDATE_VENDOR_MESHCOP_TXT_METHOD,
                   std::bind(&DBusThreadObject::UpdateMeshCopTxtHandler, this, _1));
    RegisterMethod(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_GET_PROPERTIES_METHOD,
                   std::bind(&DBusThreadObject::GetPropertiesHandler, this, _1));

    RegisterMethod(DBUS_INTERFACE_INTROSPECTABLE, DBUS_INTROSPECT_METHOD,
                   std::bind(&DBusThreadObject::IntrospectHandler, this, _1));

    RegisterSetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_MESH_LOCAL_PREFIX,
                               std::bind(&DBusThreadObject::SetMeshLocalPrefixHandler, this, _1));
    RegisterSetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_LEGACY_ULA_PREFIX,
                               std::bind(&DBusThreadObject::SetLegacyUlaPrefixHandler, this, _1));
    RegisterSetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_LINK_MODE,
                               std::bind(&DBusThreadObject::SetLinkModeHandler, this, _1));
    RegisterSetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_ACTIVE_DATASET_TLVS,
                               std::bind(&DBusThreadObject::SetActiveDatasetTlvsHandler, this, _1));
    RegisterSetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_RADIO_REGION,
                               std::bind(&DBusThreadObject::SetRadioRegionHandler, this, _1));

    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_LINK_MODE,
                               std::bind(&DBusThreadObject::GetLinkModeHandler, this, _1));
    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_DEVICE_ROLE,
                               std::bind(&DBusThreadObject::GetDeviceRoleHandler, this, _1));
    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_NETWORK_NAME,
                               std::bind(&DBusThreadObject::GetNetworkNameHandler, this, _1));

    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_PANID,
                               std::bind(&DBusThreadObject::GetPanIdHandler, this, _1));
    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_EXTPANID,
                               std::bind(&DBusThreadObject::GetExtPanIdHandler, this, _1));
    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_EUI64,
                               std::bind(&DBusThreadObject::GetEui64Handler, this, _1));
    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_CHANNEL,
                               std::bind(&DBusThreadObject::GetChannelHandler, this, _1));
    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_NETWORK_KEY,
                               std::bind(&DBusThreadObject::GetNetworkKeyHandler, this, _1));
    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_CCA_FAILURE_RATE,
                               std::bind(&DBusThreadObject::GetCcaFailureRateHandler, this, _1));
    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_LINK_COUNTERS,
                               std::bind(&DBusThreadObject::GetLinkCountersHandler, this, _1));
    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_IP6_COUNTERS,
                               std::bind(&DBusThreadObject::GetIp6CountersHandler, this, _1));
    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_SUPPORTED_CHANNEL_MASK,
                               std::bind(&DBusThreadObject::GetSupportedChannelMaskHandler, this, _1));
    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_RLOC16,
                               std::bind(&DBusThreadObject::GetRloc16Handler, this, _1));
    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_EXTENDED_ADDRESS,
                               std::bind(&DBusThreadObject::GetExtendedAddressHandler, this, _1));
    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_ROUTER_ID,
                               std::bind(&DBusThreadObject::GetRouterIdHandler, this, _1));
    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_LEADER_DATA,
                               std::bind(&DBusThreadObject::GetLeaderDataHandler, this, _1));
    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_NETWORK_DATA_PRPOERTY,
                               std::bind(&DBusThreadObject::GetNetworkDataHandler, this, _1));
    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_STABLE_NETWORK_DATA_PRPOERTY,
                               std::bind(&DBusThreadObject::GetStableNetworkDataHandler, this, _1));
    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_LOCAL_LEADER_WEIGHT,
                               std::bind(&DBusThreadObject::GetLocalLeaderWeightHandler, this, _1));
#if OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE
    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_CHANNEL_MONITOR_SAMPLE_COUNT,
                               std::bind(&DBusThreadObject::GetChannelMonitorSampleCountHandler, this, _1));
    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_CHANNEL_MONITOR_ALL_CHANNEL_QUALITIES,
                               std::bind(&DBusThreadObject::GetChannelMonitorAllChannelQualities, this, _1));
#endif
    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_CHILD_TABLE,
                               std::bind(&DBusThreadObject::GetChildTableHandler, this, _1));
    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_NEIGHBOR_TABLE_PROEPRTY,
                               std::bind(&DBusThreadObject::GetNeighborTableHandler, this, _1));
    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_PARTITION_ID_PROEPRTY,
                               std::bind(&DBusThreadObject::GetPartitionIDHandler, this, _1));
    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_INSTANT_RSSI,
                               std::bind(&DBusThreadObject::GetInstantRssiHandler, this, _1));
    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_RADIO_TX_POWER,
                               std::bind(&DBusThreadObject::GetRadioTxPowerHandler, this, _1));
    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_EXTERNAL_ROUTES,
                               std::bind(&DBusThreadObject::GetExternalRoutesHandler, this, _1));
    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_ON_MESH_PREFIXES,
                               std::bind(&DBusThreadObject::GetOnMeshPrefixesHandler, this, _1));
    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_ACTIVE_DATASET_TLVS,
                               std::bind(&DBusThreadObject::GetActiveDatasetTlvsHandler, this, _1));
    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_RADIO_REGION,
                               std::bind(&DBusThreadObject::GetRadioRegionHandler, this, _1));
    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_SRP_SERVER_INFO,
                               std::bind(&DBusThreadObject::GetSrpServerInfoHandler, this, _1));
    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_MDNS_TELEMETRY_INFO,
                               std::bind(&DBusThreadObject::GetMdnsTelemetryInfoHandler, this, _1));
    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_DNSSD_COUNTERS,
                               std::bind(&DBusThreadObject::GetDnssdCountersHandler, this, _1));
    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_OT_HOST_VERSION,
                               std::bind(&DBusThreadObject::GetOtHostVersionHandler, this, _1));
    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_OT_RCP_VERSION,
                               std::bind(&DBusThreadObject::GetOtRcpVersionHandler, this, _1));
    RegisterGetPropertyHandler(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_THREAD_VERSION,
                               std::bind(&DBusThreadObject::GetThreadVersionHandler, this, _1));

    SuccessOrExit(error = Signal(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_SIGNAL_READY, std::make_tuple()));

exit:
    return error;
}

void DBusThreadObject::DeviceRoleHandler(otDeviceRole aDeviceRole)
{
    SignalPropertyChanged(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_DEVICE_ROLE, GetDeviceRoleName(aDeviceRole));
}

void DBusThreadObject::NcpResetHandler(void)
{
    mNcp->GetThreadHelper()->AddDeviceRoleHandler(std::bind(&DBusThreadObject::DeviceRoleHandler, this, _1));
    mNcp->GetThreadHelper()->AddActiveDatasetChangeHandler(
        std::bind(&DBusThreadObject::ActiveDatasetChangeHandler, this, _1));
    SignalPropertyChanged(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_DEVICE_ROLE,
                          GetDeviceRoleName(OT_DEVICE_ROLE_DISABLED));
}

void DBusThreadObject::ScanHandler(DBusRequest &aRequest)
{
    auto threadHelper = mNcp->GetThreadHelper();
    threadHelper->Scan(std::bind(&DBusThreadObject::ReplyScanResult, this, aRequest, _1, _2));
}

void DBusThreadObject::ReplyScanResult(DBusRequest &                          aRequest,
                                       otError                                aError,
                                       const std::vector<otActiveScanResult> &aResult)
{
    std::vector<ActiveScanResult> results;

    if (aError != OT_ERROR_NONE)
    {
        aRequest.ReplyOtResult(aError);
    }
    else
    {
        for (const auto &r : aResult)
        {
            ActiveScanResult result;

            result.mExtAddress = ConvertOpenThreadUint64(r.mExtAddress.m8);
            result.mPanId      = r.mPanId;
            result.mChannel    = r.mChannel;
            result.mRssi       = r.mRssi;
            result.mLqi        = r.mLqi;

            results.emplace_back(result);
        }

        aRequest.Reply(std::tie(results));
    }
}

void DBusThreadObject::EnergyScanHandler(DBusRequest &aRequest)
{
    otError  error        = OT_ERROR_NONE;
    auto     threadHelper = mNcp->GetThreadHelper();
    uint32_t scanDuration;

    auto args = std::tie(scanDuration);

    VerifyOrExit(DBusMessageToTuple(*aRequest.GetMessage(), args) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);
    threadHelper->EnergyScan(scanDuration, std::bind(&DBusThreadObject::ReplyEnergyScanResult, this, aRequest, _1, _2));

exit:
    if (error != OT_ERROR_NONE)
    {
        aRequest.ReplyOtResult(error);
    }
}

void DBusThreadObject::ReplyEnergyScanResult(DBusRequest &                          aRequest,
                                             otError                                aError,
                                             const std::vector<otEnergyScanResult> &aResult)
{
    std::vector<EnergyScanResult> results;

    if (aError != OT_ERROR_NONE)
    {
        aRequest.ReplyOtResult(aError);
    }
    else
    {
        for (const auto &r : aResult)
        {
            EnergyScanResult result;

            result.mChannel = r.mChannel;
            result.mMaxRssi = r.mMaxRssi;

            results.emplace_back(result);
        }

        aRequest.Reply(std::tie(results));
    }
}

void DBusThreadObject::AttachHandler(DBusRequest &aRequest)
{
    auto                 threadHelper = mNcp->GetThreadHelper();
    std::string          name;
    uint16_t             panid;
    uint64_t             extPanId;
    std::vector<uint8_t> networkKey;
    std::vector<uint8_t> pskc;
    uint32_t             channelMask;

    auto args = std::tie(networkKey, panid, name, extPanId, pskc, channelMask);

    if (IsDBusMessageEmpty(*aRequest.GetMessage()))
    {
        threadHelper->Attach([aRequest](otError aError, int64_t aAttachDelayMs) mutable {
            OT_UNUSED_VARIABLE(aAttachDelayMs);

            aRequest.ReplyOtResult(aError);
        });
    }
    else if (DBusMessageToTuple(*aRequest.GetMessage(), args) != OTBR_ERROR_NONE)
    {
        aRequest.ReplyOtResult(OT_ERROR_INVALID_ARGS);
    }
    else
    {
        threadHelper->Attach(name, panid, extPanId, networkKey, pskc, channelMask,
                             [aRequest](otError aError, int64_t aAttachDelayMs) mutable {
                                 OT_UNUSED_VARIABLE(aAttachDelayMs);

                                 aRequest.ReplyOtResult(aError);
                             });
    }
}

void DBusThreadObject::AttachAllNodesToHandler(DBusRequest &aRequest)
{
    std::vector<uint8_t> dataset;
    otError              error = OT_ERROR_NONE;

    auto args = std::tie(dataset);

    VerifyOrExit(DBusMessageToTuple(*aRequest.GetMessage(), args) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);

    mNcp->GetThreadHelper()->AttachAllNodesTo(dataset, [aRequest](otError error, int64_t aAttachDelayMs) mutable {
        aRequest.ReplyOtResult<int64_t>(error, aAttachDelayMs);
    });

exit:
    if (error != OT_ERROR_NONE)
    {
        aRequest.ReplyOtResult(error);
    }
}

void DBusThreadObject::DetachHandler(DBusRequest &aRequest)
{
    aRequest.ReplyOtResult(mNcp->GetThreadHelper()->Detach());
}

void DBusThreadObject::FactoryResetHandler(DBusRequest &aRequest)
{
    otError error = OT_ERROR_NONE;

    SuccessOrExit(error = mNcp->GetThreadHelper()->Detach());
    SuccessOrExit(otInstanceErasePersistentInfo(mNcp->GetThreadHelper()->GetInstance()));
    mNcp->Reset();

exit:
    aRequest.ReplyOtResult(error);
}

void DBusThreadObject::ResetHandler(DBusRequest &aRequest)
{
    mNcp->Reset();
    aRequest.ReplyOtResult(OT_ERROR_NONE);
}

void DBusThreadObject::JoinerStartHandler(DBusRequest &aRequest)
{
    auto        threadHelper = mNcp->GetThreadHelper();
    std::string pskd, provisionUrl, vendorName, vendorModel, vendorSwVersion, vendorData;
    auto        args = std::tie(pskd, provisionUrl, vendorName, vendorModel, vendorSwVersion, vendorData);

    if (DBusMessageToTuple(*aRequest.GetMessage(), args) != OTBR_ERROR_NONE)
    {
        aRequest.ReplyOtResult(OT_ERROR_INVALID_ARGS);
    }
    else
    {
        threadHelper->JoinerStart(pskd, provisionUrl, vendorName, vendorModel, vendorSwVersion, vendorData,
                                  [aRequest](otError aError) mutable { aRequest.ReplyOtResult(aError); });
    }
}

void DBusThreadObject::JoinerStopHandler(DBusRequest &aRequest)
{
    auto threadHelper = mNcp->GetThreadHelper();

    otJoinerStop(threadHelper->GetInstance());
    aRequest.ReplyOtResult(OT_ERROR_NONE);
}

void DBusThreadObject::PermitUnsecureJoinHandler(DBusRequest &aRequest)
{
#ifdef OTBR_ENABLE_UNSECURE_JOIN
    auto     threadHelper = mNcp->GetThreadHelper();
    uint16_t port;
    uint32_t timeout;
    auto     args = std::tie(port, timeout);

    if (DBusMessageToTuple(*aRequest.GetMessage(), args) != OTBR_ERROR_NONE)
    {
        aRequest.ReplyOtResult(OT_ERROR_INVALID_ARGS);
    }
    else
    {
        aRequest.ReplyOtResult(threadHelper->PermitUnsecureJoin(port, timeout));
    }
#else
    aRequest.ReplyOtResult(OT_ERROR_NOT_IMPLEMENTED);
#endif
}

void DBusThreadObject::AddOnMeshPrefixHandler(DBusRequest &aRequest)
{
    auto                 threadHelper = mNcp->GetThreadHelper();
    OnMeshPrefix         onMeshPrefix;
    auto                 args  = std::tie(onMeshPrefix);
    otError              error = OT_ERROR_NONE;
    otBorderRouterConfig config;

    VerifyOrExit(DBusMessageToTuple(*aRequest.GetMessage(), args) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);

    // size is guaranteed by parsing
    std::copy(onMeshPrefix.mPrefix.mPrefix.begin(), onMeshPrefix.mPrefix.mPrefix.end(),
              &config.mPrefix.mPrefix.mFields.m8[0]);
    config.mPrefix.mLength = onMeshPrefix.mPrefix.mLength;
    config.mPreference     = onMeshPrefix.mPreference;
    config.mSlaac          = onMeshPrefix.mSlaac;
    config.mDhcp           = onMeshPrefix.mDhcp;
    config.mConfigure      = onMeshPrefix.mConfigure;
    config.mDefaultRoute   = onMeshPrefix.mDefaultRoute;
    config.mOnMesh         = onMeshPrefix.mOnMesh;
    config.mStable         = onMeshPrefix.mStable;

    SuccessOrExit(error = otBorderRouterAddOnMeshPrefix(threadHelper->GetInstance(), &config));
    SuccessOrExit(error = otBorderRouterRegister(threadHelper->GetInstance()));

exit:
    aRequest.ReplyOtResult(error);
}

void DBusThreadObject::RemoveOnMeshPrefixHandler(DBusRequest &aRequest)
{
    auto        threadHelper = mNcp->GetThreadHelper();
    Ip6Prefix   onMeshPrefix;
    auto        args  = std::tie(onMeshPrefix);
    otError     error = OT_ERROR_NONE;
    otIp6Prefix prefix;

    VerifyOrExit(DBusMessageToTuple(*aRequest.GetMessage(), args) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);
    // size is guaranteed by parsing
    std::copy(onMeshPrefix.mPrefix.begin(), onMeshPrefix.mPrefix.end(), &prefix.mPrefix.mFields.m8[0]);
    prefix.mLength = onMeshPrefix.mLength;

    SuccessOrExit(error = otBorderRouterRemoveOnMeshPrefix(threadHelper->GetInstance(), &prefix));
    SuccessOrExit(error = otBorderRouterRegister(threadHelper->GetInstance()));

exit:
    aRequest.ReplyOtResult(error);
}

void DBusThreadObject::AddExternalRouteHandler(DBusRequest &aRequest)
{
    auto                  threadHelper = mNcp->GetThreadHelper();
    ExternalRoute         route;
    auto                  args  = std::tie(route);
    otError               error = OT_ERROR_NONE;
    otExternalRouteConfig otRoute;
    otIp6Prefix &         prefix = otRoute.mPrefix;

    VerifyOrExit(DBusMessageToTuple(*aRequest.GetMessage(), args) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);

    // size is guaranteed by parsing
    std::copy(route.mPrefix.mPrefix.begin(), route.mPrefix.mPrefix.end(), &prefix.mPrefix.mFields.m8[0]);
    prefix.mLength      = route.mPrefix.mLength;
    otRoute.mPreference = route.mPreference;
    otRoute.mStable     = route.mStable;

    SuccessOrExit(error = otBorderRouterAddRoute(threadHelper->GetInstance(), &otRoute));
    if (route.mStable)
    {
        SuccessOrExit(error = otBorderRouterRegister(threadHelper->GetInstance()));
    }

exit:
    aRequest.ReplyOtResult(error);
}

void DBusThreadObject::RemoveExternalRouteHandler(DBusRequest &aRequest)
{
    auto        threadHelper = mNcp->GetThreadHelper();
    Ip6Prefix   routePrefix;
    auto        args  = std::tie(routePrefix);
    otError     error = OT_ERROR_NONE;
    otIp6Prefix prefix;

    VerifyOrExit(DBusMessageToTuple(*aRequest.GetMessage(), args) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);

    // size is guaranteed by parsing
    std::copy(routePrefix.mPrefix.begin(), routePrefix.mPrefix.end(), &prefix.mPrefix.mFields.m8[0]);
    prefix.mLength = routePrefix.mLength;

    SuccessOrExit(error = otBorderRouterRemoveRoute(threadHelper->GetInstance(), &prefix));
    SuccessOrExit(error = otBorderRouterRegister(threadHelper->GetInstance()));

exit:
    aRequest.ReplyOtResult(error);
}

void DBusThreadObject::IntrospectHandler(DBusRequest &aRequest)
{
    std::string xmlString(
#include "dbus/server/introspect.hpp"
    );

    aRequest.Reply(std::tie(xmlString));
}

otError DBusThreadObject::SetMeshLocalPrefixHandler(DBusMessageIter &aIter)
{
    auto                                      threadHelper = mNcp->GetThreadHelper();
    otMeshLocalPrefix                         prefix;
    std::array<uint8_t, OTBR_IP6_PREFIX_SIZE> data{};
    otError                                   error = OT_ERROR_NONE;

    VerifyOrExit(DBusMessageExtractFromVariant(&aIter, data) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);
    memcpy(&prefix.m8, &data.front(), sizeof(prefix.m8));
    error = otThreadSetMeshLocalPrefix(threadHelper->GetInstance(), &prefix);

exit:
    return error;
}

otError DBusThreadObject::SetLegacyUlaPrefixHandler(DBusMessageIter &aIter)
{
#if OTBR_ENABLE_LEGACY
    std::array<uint8_t, OTBR_IP6_PREFIX_SIZE> data;
    otError                                   error = OT_ERROR_NONE;

    VerifyOrExit(DBusMessageExtractFromVariant(&aIter, data) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);
    otSetLegacyUlaPrefix(&data[0]);

exit:
    return error;
#else
    OTBR_UNUSED_VARIABLE(aIter);

    return OT_ERROR_NOT_IMPLEMENTED;
#endif // OTBR_ENABLE_LEGACY
}

otError DBusThreadObject::SetLinkModeHandler(DBusMessageIter &aIter)
{
    auto             threadHelper = mNcp->GetThreadHelper();
    LinkModeConfig   cfg;
    otLinkModeConfig otCfg;
    otError          error = OT_ERROR_NONE;

    VerifyOrExit(DBusMessageExtractFromVariant(&aIter, cfg) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);
    otCfg.mDeviceType   = cfg.mDeviceType;
    otCfg.mNetworkData  = cfg.mNetworkData;
    otCfg.mRxOnWhenIdle = cfg.mRxOnWhenIdle;
    error               = otThreadSetLinkMode(threadHelper->GetInstance(), otCfg);

exit:
    return error;
}

otError DBusThreadObject::GetLinkModeHandler(DBusMessageIter &aIter)
{
    auto             threadHelper = mNcp->GetThreadHelper();
    otLinkModeConfig otCfg        = otThreadGetLinkMode(threadHelper->GetInstance());
    LinkModeConfig   cfg;
    otError          error = OT_ERROR_NONE;

    cfg.mDeviceType   = otCfg.mDeviceType;
    cfg.mNetworkData  = otCfg.mNetworkData;
    cfg.mRxOnWhenIdle = otCfg.mRxOnWhenIdle;

    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, cfg) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);

exit:
    return error;
}

otError DBusThreadObject::GetDeviceRoleHandler(DBusMessageIter &aIter)
{
    auto         threadHelper = mNcp->GetThreadHelper();
    otDeviceRole role         = otThreadGetDeviceRole(threadHelper->GetInstance());
    std::string  roleName     = GetDeviceRoleName(role);
    otError      error        = OT_ERROR_NONE;

    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, roleName) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);

exit:
    return error;
}

otError DBusThreadObject::GetNetworkNameHandler(DBusMessageIter &aIter)
{
    auto        threadHelper = mNcp->GetThreadHelper();
    std::string networkName  = otThreadGetNetworkName(threadHelper->GetInstance());
    otError     error        = OT_ERROR_NONE;

    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, networkName) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);

exit:
    return error;
}

otError DBusThreadObject::GetPanIdHandler(DBusMessageIter &aIter)
{
    auto     threadHelper = mNcp->GetThreadHelper();
    uint16_t panId        = otLinkGetPanId(threadHelper->GetInstance());
    otError  error        = OT_ERROR_NONE;

    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, panId) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);

exit:
    return error;
}

otError DBusThreadObject::GetExtPanIdHandler(DBusMessageIter &aIter)
{
    auto                   threadHelper = mNcp->GetThreadHelper();
    const otExtendedPanId *extPanId     = otThreadGetExtendedPanId(threadHelper->GetInstance());
    uint64_t               extPanIdVal;
    otError                error = OT_ERROR_NONE;

    extPanIdVal = ConvertOpenThreadUint64(extPanId->m8);

    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, extPanIdVal) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);

exit:
    return error;
}

otError DBusThreadObject::GetChannelHandler(DBusMessageIter &aIter)
{
    auto     threadHelper = mNcp->GetThreadHelper();
    uint16_t channel      = otLinkGetChannel(threadHelper->GetInstance());
    otError  error        = OT_ERROR_NONE;

    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, channel) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);

exit:
    return error;
}

otError DBusThreadObject::GetNetworkKeyHandler(DBusMessageIter &aIter)
{
    auto         threadHelper = mNcp->GetThreadHelper();
    otNetworkKey networkKey;
    otError      error = OT_ERROR_NONE;

    otThreadGetNetworkKey(threadHelper->GetInstance(), &networkKey);
    std::vector<uint8_t> keyVal(networkKey.m8, networkKey.m8 + sizeof(networkKey.m8));
    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, keyVal) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);

exit:
    return error;
}

otError DBusThreadObject::GetCcaFailureRateHandler(DBusMessageIter &aIter)
{
    auto     threadHelper = mNcp->GetThreadHelper();
    uint16_t failureRate  = otLinkGetCcaFailureRate(threadHelper->GetInstance());
    otError  error        = OT_ERROR_NONE;

    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, failureRate) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);

exit:
    return error;
}

otError DBusThreadObject::GetLinkCountersHandler(DBusMessageIter &aIter)
{
    auto                 threadHelper = mNcp->GetThreadHelper();
    const otMacCounters *otCounters   = otLinkGetCounters(threadHelper->GetInstance());
    MacCounters          counters;
    otError              error = OT_ERROR_NONE;

    counters.mTxTotal              = otCounters->mTxTotal;
    counters.mTxUnicast            = otCounters->mTxUnicast;
    counters.mTxBroadcast          = otCounters->mTxBroadcast;
    counters.mTxAckRequested       = otCounters->mTxAckRequested;
    counters.mTxAcked              = otCounters->mTxAcked;
    counters.mTxNoAckRequested     = otCounters->mTxNoAckRequested;
    counters.mTxData               = otCounters->mTxData;
    counters.mTxDataPoll           = otCounters->mTxDataPoll;
    counters.mTxBeacon             = otCounters->mTxBeacon;
    counters.mTxBeaconRequest      = otCounters->mTxBeaconRequest;
    counters.mTxOther              = otCounters->mTxOther;
    counters.mTxRetry              = otCounters->mTxRetry;
    counters.mTxErrCca             = otCounters->mTxErrCca;
    counters.mTxErrAbort           = otCounters->mTxErrAbort;
    counters.mTxErrBusyChannel     = otCounters->mTxErrBusyChannel;
    counters.mRxTotal              = otCounters->mRxTotal;
    counters.mRxUnicast            = otCounters->mTxUnicast;
    counters.mRxBroadcast          = otCounters->mRxBroadcast;
    counters.mRxData               = otCounters->mRxData;
    counters.mRxDataPoll           = otCounters->mTxDataPoll;
    counters.mRxBeacon             = otCounters->mRxBeacon;
    counters.mRxBeaconRequest      = otCounters->mRxBeaconRequest;
    counters.mRxOther              = otCounters->mRxOther;
    counters.mRxAddressFiltered    = otCounters->mRxAddressFiltered;
    counters.mRxDestAddrFiltered   = otCounters->mRxDestAddrFiltered;
    counters.mRxDuplicated         = otCounters->mRxDuplicated;
    counters.mRxErrNoFrame         = otCounters->mRxErrNoFrame;
    counters.mRxErrUnknownNeighbor = otCounters->mRxErrUnknownNeighbor;
    counters.mRxErrInvalidSrcAddr  = otCounters->mRxErrInvalidSrcAddr;
    counters.mRxErrSec             = otCounters->mRxErrSec;
    counters.mRxErrFcs             = otCounters->mRxErrFcs;
    counters.mRxErrOther           = otCounters->mRxErrOther;

    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, counters) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);

exit:
    return error;
}

otError DBusThreadObject::GetIp6CountersHandler(DBusMessageIter &aIter)
{
    auto                threadHelper = mNcp->GetThreadHelper();
    const otIpCounters *otCounters   = otThreadGetIp6Counters(threadHelper->GetInstance());
    IpCounters          counters;
    otError             error = OT_ERROR_NONE;

    counters.mTxSuccess = otCounters->mTxSuccess;
    counters.mTxFailure = otCounters->mTxFailure;
    counters.mRxSuccess = otCounters->mRxSuccess;
    counters.mRxFailure = otCounters->mRxFailure;

    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, counters) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);

exit:
    return error;
}

otError DBusThreadObject::GetSupportedChannelMaskHandler(DBusMessageIter &aIter)
{
    auto     threadHelper = mNcp->GetThreadHelper();
    uint32_t channelMask  = otLinkGetSupportedChannelMask(threadHelper->GetInstance());
    otError  error        = OT_ERROR_NONE;

    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, channelMask) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);

exit:
    return error;
}

otError DBusThreadObject::GetRloc16Handler(DBusMessageIter &aIter)
{
    auto     threadHelper = mNcp->GetThreadHelper();
    otError  error        = OT_ERROR_NONE;
    uint16_t rloc16       = otThreadGetRloc16(threadHelper->GetInstance());

    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, rloc16) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);

exit:
    return error;
}

otError DBusThreadObject::GetExtendedAddressHandler(DBusMessageIter &aIter)
{
    auto                threadHelper    = mNcp->GetThreadHelper();
    otError             error           = OT_ERROR_NONE;
    const otExtAddress *addr            = otLinkGetExtendedAddress(threadHelper->GetInstance());
    uint64_t            extendedAddress = ConvertOpenThreadUint64(addr->m8);

    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, extendedAddress) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);

exit:
    return error;
}

otError DBusThreadObject::GetRouterIdHandler(DBusMessageIter &aIter)
{
    auto         threadHelper = mNcp->GetThreadHelper();
    otError      error        = OT_ERROR_NONE;
    uint16_t     rloc16       = otThreadGetRloc16(threadHelper->GetInstance());
    otRouterInfo info;

    VerifyOrExit(otThreadGetRouterInfo(threadHelper->GetInstance(), rloc16, &info) == OT_ERROR_NONE,
                 error = OT_ERROR_INVALID_STATE);
    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, info.mRouterId) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);

exit:
    return error;
}

otError DBusThreadObject::GetLeaderDataHandler(DBusMessageIter &aIter)
{
    auto                threadHelper = mNcp->GetThreadHelper();
    otError             error        = OT_ERROR_NONE;
    struct otLeaderData data;
    LeaderData          leaderData;

    SuccessOrExit(error = otThreadGetLeaderData(threadHelper->GetInstance(), &data));
    leaderData.mPartitionId       = data.mPartitionId;
    leaderData.mWeighting         = data.mWeighting;
    leaderData.mDataVersion       = data.mDataVersion;
    leaderData.mStableDataVersion = data.mStableDataVersion;
    leaderData.mLeaderRouterId    = data.mLeaderRouterId;
    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, leaderData) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);

exit:
    return error;
}

otError DBusThreadObject::GetNetworkDataHandler(DBusMessageIter &aIter)
{
    static constexpr size_t kNetworkDataMaxSize = 255;
    auto                    threadHelper        = mNcp->GetThreadHelper();
    otError                 error               = OT_ERROR_NONE;
    uint8_t                 data[kNetworkDataMaxSize];
    uint8_t                 len = sizeof(data);
    std::vector<uint8_t>    networkData;

    SuccessOrExit(error = otNetDataGet(threadHelper->GetInstance(), /*stable=*/false, data, &len));
    networkData = std::vector<uint8_t>(&data[0], &data[len]);
    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, networkData) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);

exit:
    return error;
}

otError DBusThreadObject::GetStableNetworkDataHandler(DBusMessageIter &aIter)
{
    static constexpr size_t kNetworkDataMaxSize = 255;
    auto                    threadHelper        = mNcp->GetThreadHelper();
    otError                 error               = OT_ERROR_NONE;
    uint8_t                 data[kNetworkDataMaxSize];
    uint8_t                 len = sizeof(data);
    std::vector<uint8_t>    networkData;

    SuccessOrExit(error = otNetDataGet(threadHelper->GetInstance(), /*stable=*/true, data, &len));
    networkData = std::vector<uint8_t>(&data[0], &data[len]);
    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, networkData) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);

exit:
    return error;
}

otError DBusThreadObject::GetLocalLeaderWeightHandler(DBusMessageIter &aIter)
{
    auto    threadHelper = mNcp->GetThreadHelper();
    otError error        = OT_ERROR_NONE;
    uint8_t weight       = otThreadGetLocalLeaderWeight(threadHelper->GetInstance());

    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, weight) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);

exit:
    return error;
}

otError DBusThreadObject::GetChannelMonitorSampleCountHandler(DBusMessageIter &aIter)
{
#if OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE
    auto     threadHelper = mNcp->GetThreadHelper();
    otError  error        = OT_ERROR_NONE;
    uint32_t cnt          = otChannelMonitorGetSampleCount(threadHelper->GetInstance());

    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, cnt) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);

exit:
    return error;
#else  // OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE
    OTBR_UNUSED_VARIABLE(aIter);
    return OT_ERROR_NOT_IMPLEMENTED;
#endif // OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE
}

otError DBusThreadObject::GetChannelMonitorAllChannelQualities(DBusMessageIter &aIter)
{
#if OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE
    auto                        threadHelper = mNcp->GetThreadHelper();
    otError                     error        = OT_ERROR_NONE;
    uint32_t                    channelMask  = otLinkGetSupportedChannelMask(threadHelper->GetInstance());
    constexpr uint8_t           kNumChannels = sizeof(channelMask) * 8; // 8 bit per byte
    std::vector<ChannelQuality> quality;

    for (uint8_t i = 0; i < kNumChannels; i++)
    {
        if (channelMask & (1U << i))
        {
            uint16_t occupancy = otChannelMonitorGetChannelOccupancy(threadHelper->GetInstance(), i);

            quality.emplace_back(ChannelQuality{i, occupancy});
        }
    }

    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, quality) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);

exit:
    return error;
#else  // OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE
    OTBR_UNUSED_VARIABLE(aIter);
    return OT_ERROR_NOT_IMPLEMENTED;
#endif // OPENTHREAD_CONFIG_CHANNEL_MONITOR_ENABLE
}

otError DBusThreadObject::GetChildTableHandler(DBusMessageIter &aIter)
{
    auto                   threadHelper = mNcp->GetThreadHelper();
    otError                error        = OT_ERROR_NONE;
    uint16_t               childIndex   = 0;
    otChildInfo            childInfo;
    std::vector<ChildInfo> childTable;

    while (otThreadGetChildInfoByIndex(threadHelper->GetInstance(), childIndex, &childInfo) == OT_ERROR_NONE)
    {
        ChildInfo info;

        info.mExtAddress         = ConvertOpenThreadUint64(childInfo.mExtAddress.m8);
        info.mTimeout            = childInfo.mTimeout;
        info.mAge                = childInfo.mAge;
        info.mChildId            = childInfo.mChildId;
        info.mNetworkDataVersion = childInfo.mNetworkDataVersion;
        info.mLinkQualityIn      = childInfo.mLinkQualityIn;
        info.mAverageRssi        = childInfo.mAverageRssi;
        info.mLastRssi           = childInfo.mLastRssi;
        info.mFrameErrorRate     = childInfo.mFrameErrorRate;
        info.mMessageErrorRate   = childInfo.mMessageErrorRate;
        info.mRxOnWhenIdle       = childInfo.mRxOnWhenIdle;
        info.mFullThreadDevice   = childInfo.mFullThreadDevice;
        info.mFullNetworkData    = childInfo.mFullNetworkData;
        info.mIsStateRestoring   = childInfo.mIsStateRestoring;
        childTable.push_back(info);
        childIndex++;
    }

    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, childTable) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);

exit:
    return error;
}

otError DBusThreadObject::GetNeighborTableHandler(DBusMessageIter &aIter)
{
    auto                      threadHelper = mNcp->GetThreadHelper();
    otError                   error        = OT_ERROR_NONE;
    otNeighborInfoIterator    iter         = OT_NEIGHBOR_INFO_ITERATOR_INIT;
    otNeighborInfo            neighborInfo;
    std::vector<NeighborInfo> neighborTable;

    while (otThreadGetNextNeighborInfo(threadHelper->GetInstance(), &iter, &neighborInfo) == OT_ERROR_NONE)
    {
        NeighborInfo info;

        info.mExtAddress       = ConvertOpenThreadUint64(neighborInfo.mExtAddress.m8);
        info.mAge              = neighborInfo.mAge;
        info.mRloc16           = neighborInfo.mRloc16;
        info.mLinkFrameCounter = neighborInfo.mLinkFrameCounter;
        info.mMleFrameCounter  = neighborInfo.mMleFrameCounter;
        info.mLinkQualityIn    = neighborInfo.mLinkQualityIn;
        info.mAverageRssi      = neighborInfo.mAverageRssi;
        info.mLastRssi         = neighborInfo.mLastRssi;
        info.mFrameErrorRate   = neighborInfo.mFrameErrorRate;
        info.mMessageErrorRate = neighborInfo.mMessageErrorRate;
        info.mRxOnWhenIdle     = neighborInfo.mRxOnWhenIdle;
        info.mFullThreadDevice = neighborInfo.mFullThreadDevice;
        info.mFullNetworkData  = neighborInfo.mFullNetworkData;
        info.mIsChild          = neighborInfo.mIsChild;
        neighborTable.push_back(info);
    }

    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, neighborTable) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);

exit:
    return error;
}

otError DBusThreadObject::GetPartitionIDHandler(DBusMessageIter &aIter)
{
    auto     threadHelper = mNcp->GetThreadHelper();
    otError  error        = OT_ERROR_NONE;
    uint32_t partitionId  = otThreadGetPartitionId(threadHelper->GetInstance());

    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, partitionId) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);

exit:
    return error;
}

otError DBusThreadObject::GetInstantRssiHandler(DBusMessageIter &aIter)
{
    auto    threadHelper = mNcp->GetThreadHelper();
    otError error        = OT_ERROR_NONE;
    int8_t  rssi         = otPlatRadioGetRssi(threadHelper->GetInstance());

    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, rssi) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);

exit:
    return error;
}

otError DBusThreadObject::GetRadioTxPowerHandler(DBusMessageIter &aIter)
{
    auto    threadHelper = mNcp->GetThreadHelper();
    otError error        = OT_ERROR_NONE;
    int8_t  txPower;

    SuccessOrExit(error = otPlatRadioGetTransmitPower(threadHelper->GetInstance(), &txPower));

    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, txPower) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);

exit:
    return error;
}

otError DBusThreadObject::GetExternalRoutesHandler(DBusMessageIter &aIter)
{
    auto                       threadHelper = mNcp->GetThreadHelper();
    otError                    error        = OT_ERROR_NONE;
    otNetworkDataIterator      iter         = OT_NETWORK_DATA_ITERATOR_INIT;
    otExternalRouteConfig      config;
    std::vector<ExternalRoute> externalRouteTable;

    while (otNetDataGetNextRoute(threadHelper->GetInstance(), &iter, &config) == OT_ERROR_NONE)
    {
        ExternalRoute route;

        route.mPrefix.mPrefix      = std::vector<uint8_t>(&config.mPrefix.mPrefix.mFields.m8[0],
                                                     &config.mPrefix.mPrefix.mFields.m8[OTBR_IP6_PREFIX_SIZE]);
        route.mPrefix.mLength      = config.mPrefix.mLength;
        route.mRloc16              = config.mRloc16;
        route.mPreference          = config.mPreference;
        route.mStable              = config.mStable;
        route.mNextHopIsThisDevice = config.mNextHopIsThisDevice;
        externalRouteTable.push_back(route);
    }

    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, externalRouteTable) == OTBR_ERROR_NONE,
                 error = OT_ERROR_INVALID_ARGS);

exit:
    return error;
}

otError DBusThreadObject::GetOnMeshPrefixesHandler(DBusMessageIter &aIter)
{
    auto                      threadHelper = mNcp->GetThreadHelper();
    otError                   error        = OT_ERROR_NONE;
    otNetworkDataIterator     iter         = OT_NETWORK_DATA_ITERATOR_INIT;
    otBorderRouterConfig      config;
    std::vector<OnMeshPrefix> onMeshPrefixes;

    while (otNetDataGetNextOnMeshPrefix(threadHelper->GetInstance(), &iter, &config) == OT_ERROR_NONE)
    {
        OnMeshPrefix prefix;

        prefix.mPrefix.mPrefix = std::vector<uint8_t>(&config.mPrefix.mPrefix.mFields.m8[0],
                                                      &config.mPrefix.mPrefix.mFields.m8[OTBR_IP6_PREFIX_SIZE]);
        prefix.mPrefix.mLength = config.mPrefix.mLength;
        prefix.mRloc16         = config.mRloc16;
        prefix.mPreference     = config.mPreference;
        prefix.mPreferred      = config.mPreferred;
        prefix.mSlaac          = config.mSlaac;
        prefix.mDhcp           = config.mDhcp;
        prefix.mConfigure      = config.mConfigure;
        prefix.mDefaultRoute   = config.mDefaultRoute;
        prefix.mOnMesh         = config.mOnMesh;
        prefix.mStable         = config.mStable;
        prefix.mNdDns          = config.mNdDns;
        prefix.mDp             = config.mDp;
        onMeshPrefixes.push_back(prefix);
    }
    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, onMeshPrefixes) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);

exit:
    return error;
}

otError DBusThreadObject::SetActiveDatasetTlvsHandler(DBusMessageIter &aIter)
{
    auto                     threadHelper = mNcp->GetThreadHelper();
    std::vector<uint8_t>     data;
    otOperationalDatasetTlvs datasetTlvs;
    otError                  error = OT_ERROR_NONE;

    VerifyOrExit(DBusMessageExtractFromVariant(&aIter, data) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);
    VerifyOrExit(data.size() <= sizeof(datasetTlvs.mTlvs));
    std::copy(std::begin(data), std::end(data), std::begin(datasetTlvs.mTlvs));
    datasetTlvs.mLength = data.size();
    error               = otDatasetSetActiveTlvs(threadHelper->GetInstance(), &datasetTlvs);

exit:
    return error;
}

otError DBusThreadObject::GetActiveDatasetTlvsHandler(DBusMessageIter &aIter)
{
    auto                     threadHelper = mNcp->GetThreadHelper();
    otError                  error        = OT_ERROR_NONE;
    std::vector<uint8_t>     data;
    otOperationalDatasetTlvs datasetTlvs;

    SuccessOrExit(error = otDatasetGetActiveTlvs(threadHelper->GetInstance(), &datasetTlvs));
    data = std::vector<uint8_t>{std::begin(datasetTlvs.mTlvs), std::begin(datasetTlvs.mTlvs) + datasetTlvs.mLength};

    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, data) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);

exit:
    return error;
}

otError DBusThreadObject::SetRadioRegionHandler(DBusMessageIter &aIter)
{
    auto        threadHelper = mNcp->GetThreadHelper();
    std::string radioRegion;
    uint16_t    regionCode;
    otError     error = OT_ERROR_NONE;

    VerifyOrExit(DBusMessageExtractFromVariant(&aIter, radioRegion) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);
    VerifyOrExit(radioRegion.size() == sizeof(uint16_t), error = OT_ERROR_INVALID_ARGS);
    regionCode = radioRegion[0] << 8 | radioRegion[1];

    error = otPlatRadioSetRegion(threadHelper->GetInstance(), regionCode);

exit:
    return error;
}

void DBusThreadObject::UpdateMeshCopTxtHandler(DBusRequest &aRequest)
{
    auto                                        threadHelper = mNcp->GetThreadHelper();
    otError                                     error        = OT_ERROR_NONE;
    std::map<std::string, std::vector<uint8_t>> update;
    std::vector<TxtEntry>                       updatedTxtEntries;
    auto                                        args = std::tie(updatedTxtEntries);

    VerifyOrExit(DBusMessageToTuple(*aRequest.GetMessage(), args) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);
    for (const auto &entry : updatedTxtEntries)
    {
        update[entry.mKey] = entry.mValue;
    }
    for (const auto reservedKey : {"rv", "tv", "sb", "nn", "xp", "at", "pt", "dn", "sq", "bb", "omr"})
    {
        VerifyOrExit(!update.count(reservedKey), error = OT_ERROR_INVALID_ARGS);
    }
    threadHelper->OnUpdateMeshCopTxt(std::move(update));

exit:
    aRequest.ReplyOtResult(error);
}

otError DBusThreadObject::GetRadioRegionHandler(DBusMessageIter &aIter)
{
    auto        threadHelper = mNcp->GetThreadHelper();
    otError     error        = OT_ERROR_NONE;
    std::string radioRegion;
    uint16_t    regionCode;

    SuccessOrExit(error = otPlatRadioGetRegion(threadHelper->GetInstance(), &regionCode));
    radioRegion.resize(sizeof(uint16_t), '\0');
    radioRegion[0] = static_cast<char>((regionCode >> 8) & 0xff);
    radioRegion[1] = static_cast<char>(regionCode & 0xff);

    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, radioRegion) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);

exit:
    return error;
}

otError DBusThreadObject::GetSrpServerInfoHandler(DBusMessageIter &aIter)
{
#if OTBR_ENABLE_SRP_ADVERTISING_PROXY
    auto                               threadHelper = mNcp->GetThreadHelper();
    auto                               instance     = threadHelper->GetInstance();
    otError                            error        = OT_ERROR_NONE;
    SrpServerInfo                      srpServerInfo{};
    otSrpServerLeaseInfo               leaseInfo;
    const otSrpServerHost *            host             = nullptr;
    const otSrpServerResponseCounters *responseCounters = otSrpServerGetResponseCounters(instance);

    srpServerInfo.mState       = SrpServerState(static_cast<uint8_t>(otSrpServerGetState(instance)));
    srpServerInfo.mPort        = otSrpServerGetPort(instance);
    srpServerInfo.mAddressMode = SrpServerAddressMode(static_cast<uint8_t>(otSrpServerGetAddressMode(instance)));

    while ((host = otSrpServerGetNextHost(instance, host)))
    {
        const otSrpServerService *service = nullptr;

        if (otSrpServerHostIsDeleted(host))
        {
            ++srpServerInfo.mHosts.mDeletedCount;
        }
        else
        {
            ++srpServerInfo.mHosts.mFreshCount;
            otSrpServerHostGetLeaseInfo(host, &leaseInfo);
            srpServerInfo.mHosts.mLeaseTimeTotal += leaseInfo.mLease;
            srpServerInfo.mHosts.mKeyLeaseTimeTotal += leaseInfo.mKeyLease;
            srpServerInfo.mHosts.mRemainingLeaseTimeTotal += leaseInfo.mRemainingLease;
            srpServerInfo.mHosts.mRemainingKeyLeaseTimeTotal += leaseInfo.mRemainingKeyLease;
        }

        while ((service = otSrpServerHostGetNextService(host, service)))
        {
            if (otSrpServerServiceIsDeleted(service))
            {
                ++srpServerInfo.mServices.mDeletedCount;
            }
            else
            {
                ++srpServerInfo.mServices.mFreshCount;
                otSrpServerServiceGetLeaseInfo(service, &leaseInfo);
                srpServerInfo.mServices.mLeaseTimeTotal += leaseInfo.mLease;
                srpServerInfo.mServices.mKeyLeaseTimeTotal += leaseInfo.mKeyLease;
                srpServerInfo.mServices.mRemainingLeaseTimeTotal += leaseInfo.mRemainingLease;
                srpServerInfo.mServices.mRemainingKeyLeaseTimeTotal += leaseInfo.mRemainingKeyLease;
            }
        }
    }

    srpServerInfo.mResponseCounters.mSuccess       = responseCounters->mSuccess;
    srpServerInfo.mResponseCounters.mServerFailure = responseCounters->mServerFailure;
    srpServerInfo.mResponseCounters.mFormatError   = responseCounters->mFormatError;
    srpServerInfo.mResponseCounters.mNameExists    = responseCounters->mNameExists;
    srpServerInfo.mResponseCounters.mRefused       = responseCounters->mRefused;
    srpServerInfo.mResponseCounters.mOther         = responseCounters->mOther;

    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, srpServerInfo) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);

exit:
    return error;
#else  // OTBR_ENABLE_SRP_ADVERTISING_PROXY
    OTBR_UNUSED_VARIABLE(aIter);

    return OT_ERROR_NOT_IMPLEMENTED;
#endif // OTBR_ENABLE_SRP_ADVERTISING_PROXY
}

otError DBusThreadObject::GetMdnsTelemetryInfoHandler(DBusMessageIter &aIter)
{
    otError error = OT_ERROR_NONE;

    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, mPublisher->GetMdnsTelemetryInfo()) == OTBR_ERROR_NONE,
                 error = OT_ERROR_INVALID_ARGS);
exit:
    return error;
}

otError DBusThreadObject::GetDnssdCountersHandler(DBusMessageIter &aIter)
{
#if OTBR_ENABLE_DNSSD_DISCOVERY_PROXY
    auto            threadHelper = mNcp->GetThreadHelper();
    auto            instance     = threadHelper->GetInstance();
    otError         error        = OT_ERROR_NONE;
    DnssdCounters   dnssdCounters;
    otDnssdCounters otDnssdCounters = *otDnssdGetCounters(instance);

    dnssdCounters.mSuccessResponse        = otDnssdCounters.mSuccessResponse;
    dnssdCounters.mServerFailureResponse  = otDnssdCounters.mServerFailureResponse;
    dnssdCounters.mFormatErrorResponse    = otDnssdCounters.mFormatErrorResponse;
    dnssdCounters.mNameErrorResponse      = otDnssdCounters.mNameErrorResponse;
    dnssdCounters.mNotImplementedResponse = otDnssdCounters.mNotImplementedResponse;
    dnssdCounters.mOtherResponse          = otDnssdCounters.mOtherResponse;

    dnssdCounters.mResolvedBySrp = otDnssdCounters.mResolvedBySrp;

    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, dnssdCounters) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);

exit:
    return error;
#else  // OTBR_ENABLE_DNSSD_DISCOVERY_PROXY
    OTBR_UNUSED_VARIABLE(aIter);

    return OT_ERROR_NOT_IMPLEMENTED;
#endif // OTBR_ENABLE_DNSSD_DISCOVERY_PROXY
}

void DBusThreadObject::GetPropertiesHandler(DBusRequest &aRequest)
{
    UniqueDBusMessage        reply(dbus_message_new_method_return(aRequest.GetMessage()));
    DBusMessageIter          iter;
    DBusMessageIter          replyIter;
    DBusMessageIter          replySubIter;
    std::vector<std::string> propertyNames;
    otError                  error = OT_ERROR_NONE;

    VerifyOrExit(reply != nullptr, error = OT_ERROR_NO_BUFS);
    VerifyOrExit(dbus_message_iter_init(aRequest.GetMessage(), &iter), error = OT_ERROR_FAILED);
    VerifyOrExit(DBusMessageExtract(&iter, propertyNames) == OTBR_ERROR_NONE, error = OT_ERROR_PARSE);

    dbus_message_iter_init_append(reply.get(), &replyIter);
    VerifyOrExit(
        dbus_message_iter_open_container(&replyIter, DBUS_TYPE_ARRAY, DBUS_TYPE_VARIANT_AS_STRING, &replySubIter),
        error = OT_ERROR_NO_BUFS);

    for (const std::string &propertyName : propertyNames)
    {
        auto handlerIter = mGetPropertyHandlers.find(propertyName);

        otbrLogInfo("GetPropertiesHandler getting property: %s", propertyName.c_str());
        VerifyOrExit(handlerIter != mGetPropertyHandlers.end(), error = OT_ERROR_NOT_FOUND);

        SuccessOrExit(error = handlerIter->second(replySubIter));
    }

    VerifyOrExit(dbus_message_iter_close_container(&replyIter, &replySubIter), error = OT_ERROR_NO_BUFS);

exit:
    if (error == OT_ERROR_NONE)
    {
        dbus_connection_send(aRequest.GetConnection(), reply.get(), nullptr);
    }
    else
    {
        aRequest.ReplyOtResult(error);
    }
}

void DBusThreadObject::RegisterGetPropertyHandler(const std::string &        aInterfaceName,
                                                  const std::string &        aPropertyName,
                                                  const PropertyHandlerType &aHandler)
{
    DBusObject::RegisterGetPropertyHandler(aInterfaceName, aPropertyName, aHandler);
    mGetPropertyHandlers[aPropertyName] = aHandler;
}

otError DBusThreadObject::GetOtHostVersionHandler(DBusMessageIter &aIter)
{
    otError     error   = OT_ERROR_NONE;
    std::string version = otGetVersionString();

    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, version) == OTBR_ERROR_NONE, error = OT_ERROR_FAILED);

exit:
    return error;
}

otError DBusThreadObject::GetEui64Handler(DBusMessageIter &aIter)
{
    auto         threadHelper = mNcp->GetThreadHelper();
    otError      error        = OT_ERROR_NONE;
    otExtAddress extAddr;
    uint64_t     eui64;

    otLinkGetFactoryAssignedIeeeEui64(threadHelper->GetInstance(), &extAddr);

    eui64 = ConvertOpenThreadUint64(extAddr.m8);

    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, eui64) == OTBR_ERROR_NONE, error = OT_ERROR_INVALID_ARGS);

exit:
    return error;
}

otError DBusThreadObject::GetOtRcpVersionHandler(DBusMessageIter &aIter)
{
    auto        threadHelper = mNcp->GetThreadHelper();
    otError     error        = OT_ERROR_NONE;
    std::string version      = otGetRadioVersionString(threadHelper->GetInstance());

    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, version) == OTBR_ERROR_NONE, error = OT_ERROR_FAILED);

exit:
    return error;
}

otError DBusThreadObject::GetThreadVersionHandler(DBusMessageIter &aIter)
{
    otError error = OT_ERROR_NONE;

    VerifyOrExit(DBusMessageEncodeToVariant(&aIter, otThreadGetVersion()) == OTBR_ERROR_NONE, error = OT_ERROR_FAILED);

exit:
    return error;
}

void DBusThreadObject::ActiveDatasetChangeHandler(const otOperationalDatasetTlvs &aDatasetTlvs)
{
    std::vector<uint8_t> value(aDatasetTlvs.mLength);
    std::copy(aDatasetTlvs.mTlvs, aDatasetTlvs.mTlvs + aDatasetTlvs.mLength, value.begin());
    SignalPropertyChanged(OTBR_DBUS_THREAD_INTERFACE, OTBR_DBUS_PROPERTY_ACTIVE_DATASET_TLVS, value);
}

static_assert(OTBR_SRP_SERVER_STATE_DISABLED == static_cast<uint8_t>(OT_SRP_SERVER_STATE_DISABLED),
              "OTBR_SRP_SERVER_STATE_DISABLED value is incorrect");
static_assert(OTBR_SRP_SERVER_STATE_RUNNING == static_cast<uint8_t>(OT_SRP_SERVER_STATE_RUNNING),
              "OTBR_SRP_SERVER_STATE_RUNNING value is incorrect");
static_assert(OTBR_SRP_SERVER_STATE_STOPPED == static_cast<uint8_t>(OT_SRP_SERVER_STATE_STOPPED),
              "OTBR_SRP_SERVER_STATE_STOPPED value is incorrect");

static_assert(OTBR_SRP_SERVER_ADDRESS_MODE_UNICAST == static_cast<uint8_t>(OT_SRP_SERVER_ADDRESS_MODE_UNICAST),
              "OTBR_SRP_SERVER_ADDRESS_MODE_UNICAST value is incorrect");
static_assert(OTBR_SRP_SERVER_ADDRESS_MODE_ANYCAST == static_cast<uint8_t>(OT_SRP_SERVER_ADDRESS_MODE_ANYCAST),
              "OTBR_SRP_SERVER_ADDRESS_MODE_ANYCAST value is incorrect");

} // namespace DBus
} // namespace otbr
