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
 * This file includes definitions for d-bus server constants.
 */

#ifndef OTBR_DBUS_CONSTANTS_HPP_
#define OTBR_DBUS_CONSTANTS_HPP_

#define DBUS_PROPERTY_GET_METHOD "Get"
#define DBUS_PROPERTY_SET_METHOD "Set"
#define DBUS_PROPERTY_GET_ALL_METHOD "GetAll"
#define DBUS_PROPERTIES_CHANGED_SIGNAL "PropertiesChanged"
#define DBUS_INTROSPECT_METHOD "Introspect"

#define OTBR_DBUS_SERVER_PREFIX "io.openthread.BorderRouter."
#define OTBR_DBUS_THREAD_INTERFACE "io.openthread.BorderRouter"
#define OTBR_DBUS_OBJECT_PREFIX "/io/openthread/BorderRouter/"

#define OTBR_DBUS_SCAN_METHOD "Scan"
#define OTBR_DBUS_ENERGY_SCAN_METHOD "EnergyScan"
#define OTBR_DBUS_ATTACH_METHOD "Attach"
#define OTBR_DBUS_DETACH_METHOD "Detach"
#define OTBR_DBUS_FACTORY_RESET_METHOD "FactoryReset"
#define OTBR_DBUS_RESET_METHOD "Reset"
#define OTBR_DBUS_ADD_ON_MESH_PREFIX_METHOD "AddOnMeshPrefix"
#define OTBR_DBUS_REMOVE_ON_MESH_PREFIX_METHOD "RemoveOnMeshPrefix"
#define OTBR_DBUS_PERMIT_UNSECURE_JOIN_METHOD "PermitUnsecureJoin"
#define OTBR_DBUS_JOINER_START_METHOD "JoinerStart"
#define OTBR_DBUS_JOINER_STOP_METHOD "JoinerStop"
#define OTBR_DBUS_ADD_EXTERNAL_ROUTE_METHOD "AddExternalRoute"
#define OTBR_DBUS_REMOVE_EXTERNAL_ROUTE_METHOD "RemoveExternalRoute"
#define OTBR_DBUS_ATTACH_ALL_NODES_TO_METHOD "AttachAllNodesTo"
#define OTBR_DBUS_UPDATE_VENDOR_MESHCOP_TXT_METHOD "UpdateVendorMeshCopTxtEntries"
#define OTBR_DBUS_GET_PROPERTIES_METHOD "GetProperties"

#define OTBR_DBUS_PROPERTY_MESH_LOCAL_PREFIX "MeshLocalPrefix"
#define OTBR_DBUS_PROPERTY_LEGACY_ULA_PREFIX "LegacyULAPrefix"
#define OTBR_DBUS_PROPERTY_LINK_MODE "LinkMode"
#define OTBR_DBUS_PROPERTY_DEVICE_ROLE "DeviceRole"
#define OTBR_DBUS_PROPERTY_NETWORK_NAME "NetworkName"
#define OTBR_DBUS_PROPERTY_PANID "PanId"
#define OTBR_DBUS_PROPERTY_EXTPANID "ExtPanId"
#define OTBR_DBUS_PROPERTY_CHANNEL "Channel"
#define OTBR_DBUS_PROPERTY_NETWORK_KEY "NetworkKey"
#define OTBR_DBUS_PROPERTY_CCA_FAILURE_RATE "CcaFailureRate"
#define OTBR_DBUS_PROPERTY_LINK_COUNTERS "LinkCounters"
#define OTBR_DBUS_PROPERTY_IP6_COUNTERS "Ip6Counters"
#define OTBR_DBUS_PROPERTY_SUPPORTED_CHANNEL_MASK "LinkSupportedChannelMask"
#define OTBR_DBUS_PROPERTY_RLOC16 "Rloc16"
#define OTBR_DBUS_PROPERTY_EXTENDED_ADDRESS "ExtendedAddress"
#define OTBR_DBUS_PROPERTY_ROUTER_ID "RouterID"
#define OTBR_DBUS_PROPERTY_LEADER_DATA "LeaderData"
#define OTBR_DBUS_PROPERTY_NETWORK_DATA_PRPOERTY "NetworkData"
#define OTBR_DBUS_PROPERTY_STABLE_NETWORK_DATA_PRPOERTY "StableNetworkData"
#define OTBR_DBUS_PROPERTY_LOCAL_LEADER_WEIGHT "LocalLeaderWeight"
#define OTBR_DBUS_PROPERTY_CHANNEL_MONITOR_SAMPLE_COUNT "ChannelMonitorSampleCount"
#define OTBR_DBUS_PROPERTY_CHANNEL_MONITOR_ALL_CHANNEL_QUALITIES "ChannelMonitorAllChannelQualities"
#define OTBR_DBUS_PROPERTY_CHILD_TABLE "ChildTable"
#define OTBR_DBUS_PROPERTY_NEIGHBOR_TABLE_PROEPRTY "NeighborTable"
#define OTBR_DBUS_PROPERTY_PARTITION_ID_PROEPRTY "PartitionID"
#define OTBR_DBUS_PROPERTY_INSTANT_RSSI "InstantRssi"
#define OTBR_DBUS_PROPERTY_RADIO_TX_POWER "RadioTxPower"
#define OTBR_DBUS_PROPERTY_EXTERNAL_ROUTES "ExternalRoutes"
#define OTBR_DBUS_PROPERTY_ON_MESH_PREFIXES "OnMeshPrefixes"
#define OTBR_DBUS_PROPERTY_ACTIVE_DATASET_TLVS "ActiveDatasetTlvs"
#define OTBR_DBUS_PROPERTY_RADIO_REGION "RadioRegion"
#define OTBR_DBUS_PROPERTY_SRP_SERVER_INFO "SrpServerInfo"
#define OTBR_DBUS_PROPERTY_DNSSD_COUNTERS "DnssdCounters"
#define OTBR_DBUS_PROPERTY_OT_HOST_VERSION "OtHostVersion"
#define OTBR_DBUS_PROPERTY_OT_RCP_VERSION "OtRcpVersion"
#define OTBR_DBUS_PROPERTY_THREAD_VERSION "ThreadVersion"
#define OTBR_DBUS_PROPERTY_EUI64 "Eui64"
#define OTBR_DBUS_PROPERTY_MDNS_TELEMETRY_INFO "MdnsTelemetryInfo"

#define OTBR_ROLE_NAME_DISABLED "disabled"
#define OTBR_ROLE_NAME_DETACHED "detached"
#define OTBR_ROLE_NAME_CHILD "child"
#define OTBR_ROLE_NAME_ROUTER "router"
#define OTBR_ROLE_NAME_LEADER "leader"

#define OTBR_DBUS_SIGNAL_READY "Ready"

#endif // OTBR_DBUS_CONSTANTS_HPP_
