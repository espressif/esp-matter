// Copyright 2026 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/* THIS IS A GENERATED FILE, DO NOT EDIT */

#pragma once
#include <esp_matter_data_model.h>

namespace esp_matter {
namespace cluster {
namespace thread_network_diagnostics {

inline constexpr uint32_t Id = 0x0035;

namespace feature {
namespace PacketCounts {
inline constexpr uint32_t Id = 0x1;
} /* PacketCounts */
namespace ErrorCounts {
inline constexpr uint32_t Id = 0x2;
} /* ErrorCounts */
namespace MLECounts {
inline constexpr uint32_t Id = 0x4;
} /* MLECounts */
namespace MACCounts {
inline constexpr uint32_t Id = 0x8;
} /* MACCounts */
} /* feature */

namespace attribute {
namespace Channel {
inline constexpr uint32_t Id = 0x0000;
} /* Channel */
namespace RoutingRole {
inline constexpr uint32_t Id = 0x0001;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 6;
} /* RoutingRole */
namespace NetworkName {
inline constexpr uint32_t Id = 0x0002;
} /* NetworkName */
namespace PanId {
inline constexpr uint32_t Id = 0x0003;
} /* PanId */
namespace ExtendedPanId {
inline constexpr uint32_t Id = 0x0004;
} /* ExtendedPanId */
namespace MeshLocalPrefix {
inline constexpr uint32_t Id = 0x0005;
} /* MeshLocalPrefix */
namespace OverrunCount {
inline constexpr uint32_t Id = 0x0006;
} /* OverrunCount */
namespace NeighborTable {
inline constexpr uint32_t Id = 0x0007;
} /* NeighborTable */
namespace RouteTable {
inline constexpr uint32_t Id = 0x0008;
} /* RouteTable */
namespace PartitionId {
inline constexpr uint32_t Id = 0x0009;
} /* PartitionId */
namespace Weighting {
inline constexpr uint32_t Id = 0x000A;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 255;
} /* Weighting */
namespace DataVersion {
inline constexpr uint32_t Id = 0x000B;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 255;
} /* DataVersion */
namespace StableDataVersion {
inline constexpr uint32_t Id = 0x000C;
inline constexpr uint16_t Min = 0;
inline constexpr uint16_t Max = 255;
} /* StableDataVersion */
namespace LeaderRouterId {
inline constexpr uint32_t Id = 0x000D;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 62;
} /* LeaderRouterId */
namespace DetachedRoleCount {
inline constexpr uint32_t Id = 0x000E;
} /* DetachedRoleCount */
namespace ChildRoleCount {
inline constexpr uint32_t Id = 0x000F;
} /* ChildRoleCount */
namespace RouterRoleCount {
inline constexpr uint32_t Id = 0x0010;
} /* RouterRoleCount */
namespace LeaderRoleCount {
inline constexpr uint32_t Id = 0x0011;
} /* LeaderRoleCount */
namespace AttachAttemptCount {
inline constexpr uint32_t Id = 0x0012;
} /* AttachAttemptCount */
namespace PartitionIdChangeCount {
inline constexpr uint32_t Id = 0x0013;
} /* PartitionIdChangeCount */
namespace BetterPartitionAttachAttemptCount {
inline constexpr uint32_t Id = 0x0014;
} /* BetterPartitionAttachAttemptCount */
namespace ParentChangeCount {
inline constexpr uint32_t Id = 0x0015;
} /* ParentChangeCount */
namespace TxTotalCount {
inline constexpr uint32_t Id = 0x0016;
} /* TxTotalCount */
namespace TxUnicastCount {
inline constexpr uint32_t Id = 0x0017;
} /* TxUnicastCount */
namespace TxBroadcastCount {
inline constexpr uint32_t Id = 0x0018;
} /* TxBroadcastCount */
namespace TxAckRequestedCount {
inline constexpr uint32_t Id = 0x0019;
} /* TxAckRequestedCount */
namespace TxAckedCount {
inline constexpr uint32_t Id = 0x001A;
} /* TxAckedCount */
namespace TxNoAckRequestedCount {
inline constexpr uint32_t Id = 0x001B;
} /* TxNoAckRequestedCount */
namespace TxDataCount {
inline constexpr uint32_t Id = 0x001C;
} /* TxDataCount */
namespace TxDataPollCount {
inline constexpr uint32_t Id = 0x001D;
} /* TxDataPollCount */
namespace TxBeaconCount {
inline constexpr uint32_t Id = 0x001E;
} /* TxBeaconCount */
namespace TxBeaconRequestCount {
inline constexpr uint32_t Id = 0x001F;
} /* TxBeaconRequestCount */
namespace TxOtherCount {
inline constexpr uint32_t Id = 0x0020;
} /* TxOtherCount */
namespace TxRetryCount {
inline constexpr uint32_t Id = 0x0021;
} /* TxRetryCount */
namespace TxDirectMaxRetryExpiryCount {
inline constexpr uint32_t Id = 0x0022;
} /* TxDirectMaxRetryExpiryCount */
namespace TxIndirectMaxRetryExpiryCount {
inline constexpr uint32_t Id = 0x0023;
} /* TxIndirectMaxRetryExpiryCount */
namespace TxErrCcaCount {
inline constexpr uint32_t Id = 0x0024;
} /* TxErrCcaCount */
namespace TxErrAbortCount {
inline constexpr uint32_t Id = 0x0025;
} /* TxErrAbortCount */
namespace TxErrBusyChannelCount {
inline constexpr uint32_t Id = 0x0026;
} /* TxErrBusyChannelCount */
namespace RxTotalCount {
inline constexpr uint32_t Id = 0x0027;
} /* RxTotalCount */
namespace RxUnicastCount {
inline constexpr uint32_t Id = 0x0028;
} /* RxUnicastCount */
namespace RxBroadcastCount {
inline constexpr uint32_t Id = 0x0029;
} /* RxBroadcastCount */
namespace RxDataCount {
inline constexpr uint32_t Id = 0x002A;
} /* RxDataCount */
namespace RxDataPollCount {
inline constexpr uint32_t Id = 0x002B;
} /* RxDataPollCount */
namespace RxBeaconCount {
inline constexpr uint32_t Id = 0x002C;
} /* RxBeaconCount */
namespace RxBeaconRequestCount {
inline constexpr uint32_t Id = 0x002D;
} /* RxBeaconRequestCount */
namespace RxOtherCount {
inline constexpr uint32_t Id = 0x002E;
} /* RxOtherCount */
namespace RxAddressFilteredCount {
inline constexpr uint32_t Id = 0x002F;
} /* RxAddressFilteredCount */
namespace RxDestAddrFilteredCount {
inline constexpr uint32_t Id = 0x0030;
} /* RxDestAddrFilteredCount */
namespace RxDuplicatedCount {
inline constexpr uint32_t Id = 0x0031;
} /* RxDuplicatedCount */
namespace RxErrNoFrameCount {
inline constexpr uint32_t Id = 0x0032;
} /* RxErrNoFrameCount */
namespace RxErrUnknownNeighborCount {
inline constexpr uint32_t Id = 0x0033;
} /* RxErrUnknownNeighborCount */
namespace RxErrInvalidSrcAddrCount {
inline constexpr uint32_t Id = 0x0034;
} /* RxErrInvalidSrcAddrCount */
namespace RxErrSecCount {
inline constexpr uint32_t Id = 0x0035;
} /* RxErrSecCount */
namespace RxErrFcsCount {
inline constexpr uint32_t Id = 0x0036;
} /* RxErrFcsCount */
namespace RxErrOtherCount {
inline constexpr uint32_t Id = 0x0037;
} /* RxErrOtherCount */
namespace ActiveTimestamp {
inline constexpr uint32_t Id = 0x0038;
} /* ActiveTimestamp */
namespace PendingTimestamp {
inline constexpr uint32_t Id = 0x0039;
} /* PendingTimestamp */
namespace Delay {
inline constexpr uint32_t Id = 0x003A;
} /* Delay */
namespace SecurityPolicy {
inline constexpr uint32_t Id = 0x003B;
} /* SecurityPolicy */
namespace ChannelPage0Mask {
inline constexpr uint32_t Id = 0x003C;
} /* ChannelPage0Mask */
namespace OperationalDatasetComponents {
inline constexpr uint32_t Id = 0x003D;
} /* OperationalDatasetComponents */
namespace ActiveNetworkFaultsList {
inline constexpr uint32_t Id = 0x003E;
} /* ActiveNetworkFaultsList */
} /* attribute */

namespace command {
namespace ResetCounts {
inline constexpr uint32_t Id = 0x00;
} /* ResetCounts */
} /* command */

namespace event {
namespace ConnectionStatus {
inline constexpr uint32_t Id = 0x00;
} /* ConnectionStatus */
namespace NetworkFaultChange {
inline constexpr uint32_t Id = 0x01;
} /* NetworkFaultChange */
} /* event */

} /* thread_network_diagnostics */
} /* cluster */
} /* esp_matter */
