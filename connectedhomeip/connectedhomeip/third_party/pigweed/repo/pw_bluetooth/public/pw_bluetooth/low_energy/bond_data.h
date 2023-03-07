// Copyright 2022 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.
#pragma once

#include <array>
#include <cstdint>
#include <optional>

#include "pw_bluetooth/low_energy/connection.h"
#include "pw_bluetooth/types.h"
#include "pw_containers/vector.h"

namespace pw::bluetooth::low_energy {

// A 128-bit secret key.
using Key = std::array<uint8_t, 16>;

/// Represents a LE Long-Term peer key used for link encyrption. The `ediv` and
/// `rand` fields are zero if distributed using LE Secure Connections pairing.
struct LongTermKey {
  Key key;
  uint16_t ediv;
  uint16_t rand;
};

struct BondData {
  // The identifier that uniquely identifies this peer.
  PeerId peer_id;

  // The local Bluetooth identity address that this bond is associated with.
  Address local_address;

  std::optional<DeviceName> name;

  // The identity address of the peer.
  Address peer_address;

  // The peer's preferred connection parameters, if known.
  std::optional<RequestedConnectionParameters> connection_parameters;

  // Identity Resolving RemoteKey used to generate and resolve random addresses.
  Key identity_resolving_remote_key;

  // Connection Signature Resolving RemoteKey used for data signing without
  // encryption.
  Key connection_signature_resolving_remote_key;

  // LE long-term key used to encrypt a connection when the peer is in the LE
  // Peripheral role.
  //
  // In legacy pairing (`peer_long_term_key.security.secure_connections` is
  // false),  this key corresponds to the key distributed by the peer. In Secure
  // Connections pairing there is only one LTK and `peer_long_term_key` is the
  // same as `local_long_term_key`.
  LongTermKey peer_long_term_key;

  // LE long-term key used to encrypt a connection when the peer is in the LE
  // Central role.
  //
  // In legacy pairing (`local_long_term_key.security.secure_connections` is
  // false), this key corresponds to the key distributed by the local device.
  // In Secure Connections pairing there is only one LTK and
  // `local_long_term_key` is the same as `peer_long_term_key`.
  LongTermKey local_long_term_key;
};

}  // namespace pw::bluetooth::low_energy
