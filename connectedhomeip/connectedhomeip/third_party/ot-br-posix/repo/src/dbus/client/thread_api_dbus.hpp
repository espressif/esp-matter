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
 * This file includes definitions for d-bus client API.
 */

#ifndef OTBR_THREAD_API_DBUS_HPP_
#define OTBR_THREAD_API_DBUS_HPP_

#include <functional>

#include <dbus/dbus.h>

#include "common/types.hpp"
#include "dbus/common/constants.hpp"
#include "dbus/common/error.hpp"
#include "dbus/common/types.hpp"

namespace otbr {
namespace DBus {

bool IsThreadActive(DeviceRole aRole);

class ThreadApiDBus
{
public:
    using DeviceRoleHandler = std::function<void(DeviceRole)>;
    using ScanHandler       = std::function<void(const std::vector<ActiveScanResult> &)>;
    using EnergyScanHandler = std::function<void(const std::vector<EnergyScanResult> &)>;
    using OtResultHandler   = std::function<void(ClientError)>;

    /**
     * The constructor of a d-bus object.
     *
     * Will use the default interfacename
     *
     * @param[in] aConnection  The dbus connection.
     *
     */
    ThreadApiDBus(DBusConnection *aConnection);

    /**
     * The constructor of a d-bus object.
     *
     * @param[in] aConnection     The dbus connection.
     * @param[in] aInterfaceName  The network interface name.
     *
     */
    ThreadApiDBus(DBusConnection *aConnection, const std::string &aInterfaceName);

    /**
     * This method adds a callback for device role change.
     *
     * @param[in] aHandler  The device role handler.
     *
     */
    void AddDeviceRoleHandler(const DeviceRoleHandler &aHandler);

    /**
     * This method permits unsecure join on port.
     *
     * @param[in] aPort     The port number.
     * @param[in] aSeconds  The timeout to close the port, 0 for never close.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError PermitUnsecureJoin(uint16_t aPort, uint32_t aSeconds);

    /**
     * This method performs a Thread network scan.
     *
     * @param[in] aHandler  The scan result handler.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError Scan(const ScanHandler &aHandler);

    /**
     * This method performs an IEEE 802.15.4 Energy Scan.
     *
     * @param[in] aScanDuration  The duration for the scan for each channel, in milliseconds. Note that maximum value
     *                           for the duration is currently 65535. If it's the duration is larger than that, the
     *                           handler will get an INVALID_ARGS error code.
     * @param[in] aHandler       The energy scan result handler.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError EnergyScan(uint32_t aScanDuration, const EnergyScanHandler &aHandler);

    /**
     * This method attaches the device to the Thread network.
     * @param[in] aNetworkName  The network name.
     * @param[in] aPanId        The pan id, UINT16_MAX for random.
     * @param[in] aExtPanId     The extended pan id, UINT64_MAX for random.
     * @param[in] aNetworkKey   The network key, empty for random.
     * @param[in] aPSKc         The pre-shared commissioner key, empty for random.
     * @param[in] aChannelMask  A bitmask for valid channels, will random select one.
     * @param[in] aHandler      The attach result handler.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError Attach(const std::string &         aNetworkName,
                       uint16_t                    aPanId,
                       uint64_t                    aExtPanId,
                       const std::vector<uint8_t> &aNetworkKey,
                       const std::vector<uint8_t> &aPSKc,
                       uint32_t                    aChannelMask,
                       const OtResultHandler &     aHandler);

    /**
     * This method attaches the device to the Thread network.
     *
     * The network parameters will be set with the active dataset under this
     * circumstance.
     *
     * @param[in] aHandler  The attach result handler.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError Attach(const OtResultHandler &aHandler);

    /**
     * This method detaches the device from the Thread network.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError Detach(const OtResultHandler &aHandler);

    /**
     * This method attaches the device to the specified Thread network.
     *
     * If the device has already attached to a network, send a request to migrate the existing network.
     *
     * @param[in] aDataset  The Operational Dataset that contains parameter values of the Thread network to attach
     *                      to. It must have a valid Delay Timer and Pending Timestamp.
     *
     * @retval ERROR_NONE              Successfully requested the Thread network migration.
     * @retval ERROR_DBUS              D-Bus encode/decode error.
     * @retval OT_ERROR_REJECTED       The request was rejected by the leader.
     * @retval OT_ERROR_FAILED         Failed to send the request.
     * @retval OT_ERROR_INVALID_STATE  The device is attaching.
     * @retval OT_ERROR_INVALID_ARGS   Arguments are invalid.
     * @retval OT_ERROR_BUSY           There is an ongoing request.
     *
     */
    ClientError AttachAllNodesTo(const std::vector<uint8_t> &aDataset);

    /**
     * This method performs a factory reset.
     *
     * @param[in] aHandler  The reset result handler.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError FactoryReset(const OtResultHandler &aHandler);

    /**
     * This method performs a soft reset.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError Reset(void);

    /**
     * This method triggers a thread join process.
     *
     * @note The joiner start and the attach proccesses are exclusive
     *
     * @param[in] aPskd             The pre-shared key for device.
     * @param[in] aProvisioningUrl  The provision url.
     * @param[in] aVendorName       The vendor name.
     * @param[in] aVendorModel      The vendor model.
     * @param[in] aVendorSwVersion  The vendor software version.
     * @param[in] aVendorData       The vendor custom data.
     * @param[in] aHandler          The join result handler.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError JoinerStart(const std::string &    aPskd,
                            const std::string &    aProvisioningUrl,
                            const std::string &    aVendorName,
                            const std::string &    aVendorModel,
                            const std::string &    aVendorSwVersion,
                            const std::string &    aVendorData,
                            const OtResultHandler &aHandler);

    /**
     * This method stops the joiner process
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError JoinerStop(void);

    /**
     * This method adds a on-mesh address prefix.
     *
     * @param[in] aPrefix  The address prefix.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError AddOnMeshPrefix(const OnMeshPrefix &aPrefix);

    /**
     * This method removes a on-mesh address prefix.
     *
     * @param[in] aPrefix  The address prefix.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError RemoveOnMeshPrefix(const Ip6Prefix &aPrefix);

    /**
     * This method adds an external route.
     *
     * @param[in] aExternalroute  The external route config
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError AddExternalRoute(const ExternalRoute &aExternalRoute);

    /**
     * This method removes an external route.
     *
     * @param[in] aPrefix  The route prefix.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError RemoveExternalRoute(const Ip6Prefix &aPrefix);

    /**
     * This method sets the mesh-local prefix.
     *
     * @param[in] aPrefix The address prefix.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError SetMeshLocalPrefix(const std::array<uint8_t, OTBR_IP6_PREFIX_SIZE> &aPrefix);

    /**
     * This method sets the legacy prefix of ConnectIP.
     *
     * @param[in] aPrefix  The address prefix.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError SetLegacyUlaPrefix(const std::array<uint8_t, OTBR_IP6_PREFIX_SIZE> &aPrefix);

    /**
     * This method sets the active operational dataset.
     *
     * @param[out] aDataset  The active operational dataset
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError SetActiveDatasetTlvs(const std::vector<uint8_t> &aDataset);

    /**
     * This method sets the link operating mode.
     *
     * @param[in] aConfig  The operating mode config.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError SetLinkMode(const LinkModeConfig &aConfig);

    /**
     * This method sets the radio region.
     *
     * @param[in] aRadioRegion  The radio region.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError SetRadioRegion(const std::string &aRadioRegion);

    /**
     * This method gets the link operating mode.
     *
     * @param[out] aConfig  The operating mode config.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError GetLinkMode(LinkModeConfig &aConfig);

    /**
     * This method gets the current device role.
     *
     * @param[out] aDeviceRole  The device role
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError GetDeviceRole(DeviceRole &aDeviceRole);

    /**
     * This method gets the network name.
     *
     * @param[out] aName  The network name.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError GetNetworkName(std::string &aName);

    /**
     * This method gets the network pan id.
     *
     * @param[out] aPanId  The pan id.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError GetPanId(uint16_t &aPanId);

    /**
     * This method gets the extended pan id.
     *
     * @param[out] aExtPanId  The extended pan id.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError GetExtPanId(uint64_t &aExtPanId);

    /**
     * This method gets the extended pan id.
     *
     * @param[out] aChannel  The extended pan id.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError GetChannel(uint16_t &aChannel);

    /**
     * This method gets the network network key.
     *
     * @param[out] aNetworkKey  The network network key.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError GetNetworkKey(std::vector<uint8_t> &aNetworkKey);

    /**
     * This method gets the Clear Channel Assessment failure rate.
     *
     * @param[out] aFailureRate  The failure rate.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError GetCcaFailureRate(uint16_t &aFailureRate);

    /**
     * This method gets the mac level statistics counters.
     *
     * @param[out] aCounters  The statistic counters.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError GetLinkCounters(MacCounters &aCounters); // For telemetry

    /**
     * This method gets the ip level statistics counters.
     *
     * @param[out] aCounters  The statistic counters.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError GetIp6Counters(IpCounters &aCounters); // For telemetry

    /**
     * This method gets the supported channel mask.
     *
     * @param[out] aChannelMask  The channel mask.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError GetSupportedChannelMask(uint32_t &aChannelMask);

    /**
     * This method gets the Thread routing locator
     *
     * @param[out] aRloc16  The routing locator
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError GetRloc16(uint16_t &aRloc16);

    /**
     * This method gets 802.15.4 extended address
     *
     * @param[out] aExtendedAddress  The extended address
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError GetExtendedAddress(uint64_t &aExtendedAddress);

    /**
     * This method gets the node's router id.
     *
     * @param[out] aRouterId  The router id.
     *
     * @retval ERROR_NONE              Successfully performed the dbus function call.
     * @retval ERROR_DBUS              dbus encode/decode error.
     * @retval OT_ERROR_INVALID_STATE  The node is not a router.
     * @retval ...                     OpenThread defined error value otherwise.
     *
     */
    ClientError GetRouterId(uint8_t &aRouterId);

    /**
     * This method gets the network's leader data.
     *
     * @param[out] aLeaderData  The leader data.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError GetLeaderData(LeaderData &aLeaderData);

    /**
     * This method gets the network data.
     *
     * @param[out] aNetworkData  The network data.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError GetNetworkData(std::vector<uint8_t> &aNetworkData);

    /**
     * This method gets the stable network data.
     *
     * @param[out] aNetworkData  The stable network data.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError GetStableNetworkData(std::vector<uint8_t> &aNetworkData);

    /**
     * This method gets the node's local leader weight.
     *
     * @param[out] aWeight  The local leader weight.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError GetLocalLeaderWeight(uint8_t &aWeight);

    /**
     * This method gets the channel monitor sample count.
     *
     * @param[out] aSampleCount  The channel monitor sample count.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError GetChannelMonitorSampleCount(uint32_t &aSampleCount);

    /**
     * This method gets the channel qualities
     *
     * @param[out] aChannelQualities  The channel qualities.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError GetChannelMonitorAllChannelQualities(std::vector<ChannelQuality> &aChannelQualities);

    /**
     * This method gets the child table.
     *
     * @param[out] aChildTable  The child table.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError GetChildTable(std::vector<ChildInfo> &aChildTable);

    /**
     * This method gets the neighbor table.
     *
     * @param[out] aNeighborTable  The neighbor table.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError GetNeighborTable(std::vector<NeighborInfo> &aNeighborTable);

    /**
     * This method gets the network's parition id.
     *
     * @param[out] aPartitionId  The partition id.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError GetPartitionId(uint32_t &aPartitionId);

    /**
     * This method gets the rssi of the latest packet.
     *
     * @param[out] aRssi  The rssi of the latest packet.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError GetInstantRssi(int8_t &aRssi);

    /**
     * This method gets the radio transmit power.
     *
     * @param[out] aTxPower  The radio transmit power.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError GetRadioTxPower(int8_t &aTxPower);

    /**
     * This method gets the external route table
     *
     * @param[out] aExternalRoutes  The external route table
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError GetExternalRoutes(std::vector<ExternalRoute> &aExternalRoutes);

    /**
     * This method gets the on-mesh prefixes
     *
     * @param[out] aOnMeshPrefixes  The on-mesh prefixes
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError GetOnMeshPrefixes(std::vector<OnMeshPrefix> &aOnMeshPrefixes);

    /**
     * This method gets the active operational dataset
     *
     * @param[out] aDataset  The active operational dataset
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError GetActiveDatasetTlvs(std::vector<uint8_t> &aDataset);

    /**
     * This method gets the radio region.
     *
     * @param[out] aRadioRegion  The radio region.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError GetRadioRegion(std::string &aRadioRegion);

    /**
     * This method gets the SRP server information.
     *
     * @param[out] aSrpServerInfo  The SRP server information.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError GetSrpServerInfo(SrpServerInfo &aSrpServerInfo);

    /**
     * This method gets the MDNS telemetry information.
     *
     * @param[out] aMdnsTelemetryInfo  The MDNS telemetry information.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError GetMdnsTelemetryInfo(MdnsTelemetryInfo &aMdnsTelemetryInfo);

#if OTBR_ENABLE_DNSSD_DISCOVERY_PROXY
    /**
     * This method gets the DNS-SD counters.
     *
     * @param[out] aDnssdCounters  The DNS-SD counters.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError GetDnssdCounters(DnssdCounters &aDnssdCounters);
#endif

    /**
     * This method returns the network interface name the client is bound to.
     *
     * @returns The network interface name.
     *
     */
    std::string GetInterfaceName(void);

    /**
     * This method sets multiple vendor-specific entries for the TXT record of the MeshCoP service.
     *
     * @note
     * - The @p aUpdate must contain all vendor-specific TXT entries you want to update. The latest call will supersede
     * previous calls.
     * - If @p aUpdate contains thread-specific entries like 'nn', 'at', the whole update will be rejected.
     * - If @p aUpdate contains a key which is already published in TXT record, it will be updated according to @p
     * aUpdate.
     *
     * @param[in] aUpdate  The updated key-value entries.
     *
     * @retval ERROR_NONE  Successfully performed the dbus function call
     * @retval ERROR_DBUS  dbus encode/decode error
     * @retval ...         OpenThread defined error value otherwise
     *
     */
    ClientError UpdateVendorMeshCopTxtEntries(std::vector<TxtEntry> &aUpdate);

private:
    ClientError CallDBusMethodSync(const std::string &aMethodName);
    ClientError CallDBusMethodAsync(const std::string &aMethodName, DBusPendingCallNotifyFunction aFunction);

    template <typename ArgType> ClientError CallDBusMethodSync(const std::string &aMethodName, const ArgType &aArgs);

    template <typename ArgType>
    ClientError CallDBusMethodAsync(const std::string &           aMethodName,
                                    const ArgType &               aArgs,
                                    DBusPendingCallNotifyFunction aFunction);

    template <typename ValType> ClientError SetProperty(const std::string &aPropertyName, const ValType &aValue);

    template <typename ValType> ClientError GetProperty(const std::string &aPropertyName, ValType &aValue);

    ClientError              SubscribeDeviceRoleSignal(void);
    static DBusHandlerResult sDBusMessageFilter(DBusConnection *aConnection, DBusMessage *aMessage, void *aData);
    DBusHandlerResult        DBusMessageFilter(DBusConnection *aConnection, DBusMessage *aMessage);

    template <void (ThreadApiDBus::*Handler)(DBusPendingCall *aPending)>
    static void sHandleDBusPendingCall(DBusPendingCall *aPending, void *aThreadApiDBus);

    void        AttachPendingCallHandler(DBusPendingCall *aPending);
    void        DetachPendingCallHandler(DBusPendingCall *aPending);
    void        FactoryResetPendingCallHandler(DBusPendingCall *aPending);
    void        JoinerStartPendingCallHandler(DBusPendingCall *aPending);
    static void sScanPendingCallHandler(DBusPendingCall *aPending, void *aThreadApiDBus);
    void        ScanPendingCallHandler(DBusPendingCall *aPending);
    void        EnergyScanPendingCallHandler(DBusPendingCall *aPending);

    static void EmptyFree(void *) {}

    std::string mInterfaceName;

    DBusConnection *mConnection;

    ScanHandler       mScanHandler;
    EnergyScanHandler mEnergyScanHandler;
    OtResultHandler   mAttachHandler;
    OtResultHandler   mDetachHandler;
    OtResultHandler   mFactoryResetHandler;
    OtResultHandler   mJoinerHandler;

    std::vector<DeviceRoleHandler> mDeviceRoleHandlers;
};

} // namespace DBus
} // namespace otbr

#endif // OTBR_THREAD_API_DBUS_HPP_
