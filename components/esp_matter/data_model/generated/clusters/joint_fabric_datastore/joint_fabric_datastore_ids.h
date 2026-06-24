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
namespace joint_fabric_datastore {

inline constexpr uint32_t Id = 0x0752;

namespace attribute {
namespace AnchorRootCA {
inline constexpr uint32_t Id = 0x0000;
} /* AnchorRootCA */
namespace AnchorNodeID {
inline constexpr uint32_t Id = 0x0001;
} /* AnchorNodeID */
namespace AnchorVendorID {
inline constexpr uint32_t Id = 0x0002;
} /* AnchorVendorID */
namespace FriendlyName {
inline constexpr uint32_t Id = 0x0003;
} /* FriendlyName */
namespace GroupKeySetList {
inline constexpr uint32_t Id = 0x0004;
} /* GroupKeySetList */
namespace GroupList {
inline constexpr uint32_t Id = 0x0005;
} /* GroupList */
namespace NodeList {
inline constexpr uint32_t Id = 0x0006;
} /* NodeList */
namespace AdminList {
inline constexpr uint32_t Id = 0x0007;
} /* AdminList */
namespace Status {
inline constexpr uint32_t Id = 0x0008;
} /* Status */
namespace EndpointGroupIDList {
inline constexpr uint32_t Id = 0x0009;
} /* EndpointGroupIDList */
namespace EndpointBindingList {
inline constexpr uint32_t Id = 0x000A;
} /* EndpointBindingList */
namespace NodeKeySetList {
inline constexpr uint32_t Id = 0x000B;
} /* NodeKeySetList */
namespace NodeACLList {
inline constexpr uint32_t Id = 0x000C;
} /* NodeACLList */
namespace NodeEndpointList {
inline constexpr uint32_t Id = 0x000D;
} /* NodeEndpointList */
} /* attribute */

namespace command {
namespace AddKeySet {
inline constexpr uint32_t Id = 0x00;
} /* AddKeySet */
namespace UpdateKeySet {
inline constexpr uint32_t Id = 0x01;
} /* UpdateKeySet */
namespace RemoveKeySet {
inline constexpr uint32_t Id = 0x02;
} /* RemoveKeySet */
namespace AddGroup {
inline constexpr uint32_t Id = 0x03;
} /* AddGroup */
namespace UpdateGroup {
inline constexpr uint32_t Id = 0x04;
} /* UpdateGroup */
namespace RemoveGroup {
inline constexpr uint32_t Id = 0x05;
} /* RemoveGroup */
namespace AddAdmin {
inline constexpr uint32_t Id = 0x06;
} /* AddAdmin */
namespace UpdateAdmin {
inline constexpr uint32_t Id = 0x07;
} /* UpdateAdmin */
namespace RemoveAdmin {
inline constexpr uint32_t Id = 0x08;
} /* RemoveAdmin */
namespace AddPendingNode {
inline constexpr uint32_t Id = 0x09;
} /* AddPendingNode */
namespace RefreshNode {
inline constexpr uint32_t Id = 0x0A;
} /* RefreshNode */
namespace UpdateNode {
inline constexpr uint32_t Id = 0x0B;
} /* UpdateNode */
namespace RemoveNode {
inline constexpr uint32_t Id = 0x0C;
} /* RemoveNode */
namespace UpdateEndpointForNode {
inline constexpr uint32_t Id = 0x0D;
} /* UpdateEndpointForNode */
namespace AddGroupIDToEndpointForNode {
inline constexpr uint32_t Id = 0x0E;
} /* AddGroupIDToEndpointForNode */
namespace RemoveGroupIDFromEndpointForNode {
inline constexpr uint32_t Id = 0x0F;
} /* RemoveGroupIDFromEndpointForNode */
namespace AddBindingToEndpointForNode {
inline constexpr uint32_t Id = 0x10;
} /* AddBindingToEndpointForNode */
namespace RemoveBindingFromEndpointForNode {
inline constexpr uint32_t Id = 0x11;
} /* RemoveBindingFromEndpointForNode */
namespace AddACLToNode {
inline constexpr uint32_t Id = 0x12;
} /* AddACLToNode */
namespace RemoveACLFromNode {
inline constexpr uint32_t Id = 0x13;
} /* RemoveACLFromNode */
} /* command */

} /* joint_fabric_datastore */
} /* cluster */
} /* esp_matter */
