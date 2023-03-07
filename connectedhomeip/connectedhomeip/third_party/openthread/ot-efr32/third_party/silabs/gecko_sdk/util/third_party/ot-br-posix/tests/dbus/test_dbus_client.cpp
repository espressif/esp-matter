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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <memory>

#include <dbus/dbus.h>
#include <unistd.h>

#include "common/code_utils.hpp"
#include "dbus/client/thread_api_dbus.hpp"
#include "dbus/common/constants.hpp"

using otbr::DBus::ActiveScanResult;
using otbr::DBus::ClientError;
using otbr::DBus::DeviceRole;
using otbr::DBus::EnergyScanResult;
using otbr::DBus::ExternalRoute;
using otbr::DBus::Ip6Prefix;
using otbr::DBus::LinkModeConfig;
using otbr::DBus::OnMeshPrefix;
using otbr::DBus::SrpServerInfo;
using otbr::DBus::ThreadApiDBus;
using otbr::DBus::TxtEntry;

#if OTBR_ENABLE_DNSSD_DISCOVERY_PROXY
using otbr::DBus::DnssdCounters;
#endif

#define TEST_ASSERT(x)                                              \
    do                                                              \
    {                                                               \
        if (!(x))                                                   \
        {                                                           \
            printf("Assert failed at %s:%d\n", __FILE__, __LINE__); \
            exit(EXIT_FAILURE);                                     \
        }                                                           \
    } while (false)

struct DBusConnectionDeleter
{
    void operator()(DBusConnection *aConnection) { dbus_connection_unref(aConnection); }
};

using UniqueDBusConnection = std::unique_ptr<DBusConnection, DBusConnectionDeleter>;

static bool operator==(const otbr::DBus::Ip6Prefix &aLhs, const otbr::DBus::Ip6Prefix &aRhs)
{
    bool prefixDataEquality = (aLhs.mPrefix.size() == aRhs.mPrefix.size()) &&
                              (memcmp(&aLhs.mPrefix[0], &aRhs.mPrefix[0], aLhs.mPrefix.size()) == 0);

    return prefixDataEquality && aLhs.mLength == aRhs.mLength;
}

static void CheckExternalRoute(ThreadApiDBus *aApi, const Ip6Prefix &aPrefix)
{
    ExternalRoute              route = {};
    std::vector<ExternalRoute> externalRouteTable;

    route.mPrefix     = aPrefix;
    route.mStable     = true;
    route.mPreference = 0;

    TEST_ASSERT(aApi->AddExternalRoute(route) == OTBR_ERROR_NONE);
    sleep(10);
    TEST_ASSERT(aApi->GetExternalRoutes(externalRouteTable) == OTBR_ERROR_NONE);
    TEST_ASSERT(externalRouteTable.size() == 1);
    TEST_ASSERT(externalRouteTable[0].mPrefix == aPrefix);
    TEST_ASSERT(externalRouteTable[0].mPreference == 0);
    TEST_ASSERT(externalRouteTable[0].mStable);
    TEST_ASSERT(externalRouteTable[0].mNextHopIsThisDevice);

    TEST_ASSERT(aApi->RemoveExternalRoute(aPrefix) == OTBR_ERROR_NONE);
    sleep(10);
    TEST_ASSERT(aApi->GetExternalRoutes(externalRouteTable) == OTBR_ERROR_NONE);
    TEST_ASSERT(externalRouteTable.empty());
}

static void CheckOnMeshPrefix(ThreadApiDBus *aApi)
{
    OnMeshPrefix              prefix = {};
    std::vector<OnMeshPrefix> onMeshPrefixes;

    prefix.mPrefix.mPrefix = {0xfd, 0xee, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    prefix.mPrefix.mLength = 64;

    prefix.mPreference = 0;
    prefix.mStable     = true;

    TEST_ASSERT(aApi->AddOnMeshPrefix(prefix) == OTBR_ERROR_NONE);
    sleep(10);
    TEST_ASSERT(aApi->GetOnMeshPrefixes(onMeshPrefixes) == OTBR_ERROR_NONE);
    TEST_ASSERT(onMeshPrefixes.size() == 1);
    TEST_ASSERT(onMeshPrefixes[0].mPrefix == prefix.mPrefix);
    TEST_ASSERT(onMeshPrefixes[0].mPreference == 0);
    TEST_ASSERT(onMeshPrefixes[0].mStable);

    TEST_ASSERT(aApi->RemoveOnMeshPrefix(prefix.mPrefix) == OTBR_ERROR_NONE);
    sleep(10);
    TEST_ASSERT(aApi->GetOnMeshPrefixes(onMeshPrefixes) == OTBR_ERROR_NONE);
    TEST_ASSERT(onMeshPrefixes.empty());
}

void CheckSrpServerInfo(ThreadApiDBus *aApi)
{
    SrpServerInfo srpServerInfo;

    TEST_ASSERT(aApi->GetSrpServerInfo(srpServerInfo) == OTBR_ERROR_NONE);
    TEST_ASSERT(srpServerInfo.mState == otbr::DBus::OTBR_SRP_SERVER_STATE_RUNNING);
    TEST_ASSERT(srpServerInfo.mPort != 0);
    TEST_ASSERT(srpServerInfo.mHosts.mFreshCount == 0);
    TEST_ASSERT(srpServerInfo.mHosts.mDeletedCount == 0);
    TEST_ASSERT(srpServerInfo.mHosts.mLeaseTimeTotal == 0);
    TEST_ASSERT(srpServerInfo.mHosts.mKeyLeaseTimeTotal == 0);
    TEST_ASSERT(srpServerInfo.mHosts.mRemainingLeaseTimeTotal == 0);
    TEST_ASSERT(srpServerInfo.mHosts.mRemainingKeyLeaseTimeTotal == 0);
    TEST_ASSERT(srpServerInfo.mServices.mFreshCount == 0);
    TEST_ASSERT(srpServerInfo.mServices.mDeletedCount == 0);
    TEST_ASSERT(srpServerInfo.mServices.mLeaseTimeTotal == 0);
    TEST_ASSERT(srpServerInfo.mServices.mKeyLeaseTimeTotal == 0);
    TEST_ASSERT(srpServerInfo.mServices.mRemainingLeaseTimeTotal == 0);
    TEST_ASSERT(srpServerInfo.mServices.mRemainingKeyLeaseTimeTotal == 0);
    TEST_ASSERT(srpServerInfo.mResponseCounters.mSuccess == 0);
    TEST_ASSERT(srpServerInfo.mResponseCounters.mServerFailure == 0);
    TEST_ASSERT(srpServerInfo.mResponseCounters.mFormatError == 0);
    TEST_ASSERT(srpServerInfo.mResponseCounters.mNameExists == 0);
    TEST_ASSERT(srpServerInfo.mResponseCounters.mRefused == 0);
    TEST_ASSERT(srpServerInfo.mResponseCounters.mOther == 0);
}

void CheckDnssdCounters(ThreadApiDBus *aApi)
{
    OTBR_UNUSED_VARIABLE(aApi);
#if OTBR_ENABLE_DNSSD_DISCOVERY_PROXY
    otbr::DBus::DnssdCounters dnssdCounters;

    TEST_ASSERT(aApi->GetDnssdCounters(dnssdCounters) == OTBR_ERROR_NONE);
    TEST_ASSERT(dnssdCounters.mSuccessResponse == 0);
    TEST_ASSERT(dnssdCounters.mServerFailureResponse == 0);
    TEST_ASSERT(dnssdCounters.mFormatErrorResponse == 0);
    TEST_ASSERT(dnssdCounters.mNameErrorResponse == 0);
    TEST_ASSERT(dnssdCounters.mNotImplementedResponse == 0);
    TEST_ASSERT(dnssdCounters.mOtherResponse == 0);
    TEST_ASSERT(dnssdCounters.mResolvedBySrp == 0);
#endif
}

void CheckMdnsInfo(ThreadApiDBus *aApi)
{
    otbr::MdnsTelemetryInfo mdnsInfo;

    TEST_ASSERT(aApi->GetMdnsTelemetryInfo(mdnsInfo) == OTBR_ERROR_NONE);

    TEST_ASSERT(mdnsInfo.mServiceRegistrations.mSuccess > 0);
    TEST_ASSERT(mdnsInfo.mServiceRegistrationEmaLatency > 0);
}

int main()
{
    DBusError                      error;
    UniqueDBusConnection           connection;
    std::unique_ptr<ThreadApiDBus> api;
    uint64_t                       extpanid = 0xdead00beaf00cafe;
    std::string                    region;
    uint32_t                       scanDuration = 1000; // 1s for each channel
    bool                           stepDone     = false;

    dbus_error_init(&error);
    connection = UniqueDBusConnection(dbus_bus_get(DBUS_BUS_SYSTEM, &error));

    VerifyOrExit(connection != nullptr);

    VerifyOrExit(dbus_bus_register(connection.get(), &error) == true);

    api = std::unique_ptr<ThreadApiDBus>(new ThreadApiDBus(connection.get()));

    api->AddDeviceRoleHandler(
        [](DeviceRole aRole) { printf("Device role changed to %d\n", static_cast<uint8_t>(aRole)); });

    TEST_ASSERT(api->SetRadioRegion("US") == ClientError::ERROR_NONE);
    TEST_ASSERT(api->GetRadioRegion(region) == ClientError::ERROR_NONE);
    TEST_ASSERT(region == "US");

    api->EnergyScan(scanDuration, [&stepDone](const std::vector<EnergyScanResult> &aResult) {
        TEST_ASSERT(!aResult.empty());
        printf("Energy Scan:\n");
        for (auto &result : aResult)
        {
            printf("channel %d rssi %d\n", result.mChannel, result.mMaxRssi);
        }

        stepDone = true;
    });

    while (!stepDone)
    {
        dbus_connection_read_write_dispatch(connection.get(), 0);
    }

    stepDone = false;

    api->Scan([&api, extpanid, &stepDone](const std::vector<ActiveScanResult> &aResult) {
        LinkModeConfig       cfg        = {true, false, true};
        std::vector<uint8_t> networkKey = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                                           0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
        uint16_t             channel    = 11;

        for (auto &&result : aResult)
        {
            printf("channel %d rssi %d\n", result.mChannel, result.mRssi);
        }

        api->SetLinkMode(cfg);
        api->GetLinkMode(cfg);
        printf("LinkMode %d %d %d\n", cfg.mRxOnWhenIdle, cfg.mDeviceType, cfg.mNetworkData);

        cfg.mDeviceType = true;
        api->SetLinkMode(cfg);

        api->Attach("Test", 0x3456, extpanid, networkKey, {}, 1 << channel,
                    [&api, channel, extpanid, &stepDone](ClientError aError) {
                        printf("Attach result %d\n", static_cast<int>(aError));
                        sleep(20);
                        uint64_t             extpanidCheck;
                        std::vector<uint8_t> activeDataset;

                        if (aError == OTBR_ERROR_NONE)
                        {
                            std::string                           name;
                            uint64_t                              extAddress = 0;
                            uint16_t                              rloc16     = 0xffff;
                            std::vector<uint8_t>                  networkData;
                            std::vector<uint8_t>                  stableNetworkData;
                            int8_t                                rssi;
                            int8_t                                txPower;
                            std::vector<otbr::DBus::ChildInfo>    childTable;
                            std::vector<otbr::DBus::NeighborInfo> neighborTable;
                            uint32_t                              partitionId;
                            uint16_t                              channelResult;

                            TEST_ASSERT(api->GetChannel(channelResult) == OTBR_ERROR_NONE);
                            TEST_ASSERT(channelResult == channel);
                            TEST_ASSERT(api->GetNetworkName(name) == OTBR_ERROR_NONE);
                            TEST_ASSERT(api->GetExtPanId(extpanidCheck) == OTBR_ERROR_NONE);
                            TEST_ASSERT(api->GetRloc16(rloc16) == OTBR_ERROR_NONE);
                            TEST_ASSERT(api->GetExtendedAddress(extAddress) == OTBR_ERROR_NONE);
                            TEST_ASSERT(api->GetNetworkData(networkData) == OTBR_ERROR_NONE);
                            TEST_ASSERT(api->GetStableNetworkData(stableNetworkData) == OTBR_ERROR_NONE);
                            TEST_ASSERT(api->GetChildTable(childTable) == OTBR_ERROR_NONE);
                            TEST_ASSERT(api->GetNeighborTable(neighborTable) == OTBR_ERROR_NONE);
                            printf("neighborTable size %zu\n", neighborTable.size());
                            printf("childTable size %zu\n", childTable.size());
                            TEST_ASSERT(neighborTable.size() == 1);
                            TEST_ASSERT(childTable.size() == 1);
                            TEST_ASSERT(api->GetPartitionId(partitionId) == OTBR_ERROR_NONE);
                            TEST_ASSERT(api->GetInstantRssi(rssi) == OTBR_ERROR_NONE);
                            TEST_ASSERT(api->GetRadioTxPower(txPower) == OTBR_ERROR_NONE);
                            TEST_ASSERT(api->GetActiveDatasetTlvs(activeDataset) == OTBR_ERROR_NONE);
                            CheckSrpServerInfo(api.get());
                            CheckMdnsInfo(api.get());
                            CheckDnssdCounters(api.get());
                            api->FactoryReset(nullptr);
                            TEST_ASSERT(api->GetNetworkName(name) == OTBR_ERROR_NONE);
                            TEST_ASSERT(rloc16 != 0xffff);
                            TEST_ASSERT(extAddress != 0);
                            TEST_ASSERT(!networkData.empty());
                            TEST_ASSERT(api->GetNeighborTable(neighborTable) == OTBR_ERROR_NONE);
                            TEST_ASSERT(neighborTable.empty());
                        }
                        if (aError != OTBR_ERROR_NONE || extpanidCheck != extpanid)
                        {
                            exit(-1);
                        }
                        TEST_ASSERT(api->SetActiveDatasetTlvs(activeDataset) == OTBR_ERROR_NONE);
                        api->Attach([&api, channel, extpanid, &stepDone](ClientError aErr) {
                            uint8_t                routerId;
                            otbr::DBus::LeaderData leaderData;
                            uint8_t                leaderWeight;
                            uint16_t               channelResult;
                            uint64_t               extpanidCheck;
                            Ip6Prefix              prefix;
                            std::vector<TxtEntry>  updatedTxtEntries{TxtEntry{"B", {97, 98, 99}}};

                            prefix.mPrefix = {0xfd, 0xcd, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
                            prefix.mLength = 64;

                            TEST_ASSERT(aErr == ClientError::ERROR_NONE);
                            TEST_ASSERT(api->GetChannel(channelResult) == OTBR_ERROR_NONE);
                            TEST_ASSERT(channelResult == channel);
                            TEST_ASSERT(api->GetExtPanId(extpanidCheck) == OTBR_ERROR_NONE);
                            TEST_ASSERT(extpanidCheck == extpanid);

                            TEST_ASSERT(api->GetLocalLeaderWeight(leaderWeight) == OTBR_ERROR_NONE);
                            TEST_ASSERT(api->GetLeaderData(leaderData) == OTBR_ERROR_NONE);
                            TEST_ASSERT(api->GetRouterId(routerId) == OTBR_ERROR_NONE);
                            TEST_ASSERT(routerId == leaderData.mLeaderRouterId);

                            TEST_ASSERT(api->UpdateVendorMeshCopTxtEntries(updatedTxtEntries) == OTBR_ERROR_NONE);

                            CheckExternalRoute(api.get(), prefix);
                            CheckOnMeshPrefix(api.get());

                            api->FactoryReset(nullptr);
                            TEST_ASSERT(api->JoinerStart("ABCDEF", "", "", "", "", "", nullptr) ==
                                        ClientError::OT_ERROR_NOT_FOUND);
                            TEST_ASSERT(api->JoinerStart(
                                            "ABCDEF", "", "", "", "", "", [&api, &stepDone](ClientError aJoinError) {
                                                DeviceRole deviceRole;

                                                TEST_ASSERT(aJoinError == ClientError::OT_ERROR_NOT_FOUND);

                                                api->FactoryReset(nullptr);
                                                api->GetDeviceRole(deviceRole);
                                                TEST_ASSERT(deviceRole == otbr::DBus::OTBR_DEVICE_ROLE_DISABLED);

                                                stepDone = true;
                                            }) == ClientError::ERROR_NONE);
                        });
                    });
    });

    while (!stepDone)
    {
        dbus_connection_read_write_dispatch(connection.get(), 0);
    }

exit:
    dbus_error_free(&error);
    return 0;
};
