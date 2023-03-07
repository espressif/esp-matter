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

#include <optional>

#include "pw_bluetooth/types.h"
#include "pw_containers/vector.h"

namespace pw::bluetooth {

// Information about a remote Bluetooth device.
struct Peer {
  // Uniquely identifies this peer on the current system.
  PeerId peer_id;

  // Bluetooth device address that identifies this peer.
  // NOTE: Clients should use the `peer_id` field to keep track of peers
  // instead of their address.
  Address address;

  // The name of the peer, if known.
  std::optional<DeviceName> name;

  // The LE appearance property. Present if the appearance information was
  // obtained over advertising and/or GATT.
  std::optional<Appearance> appearance;
};

}  // namespace pw::bluetooth
