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

#include <cstdint>

namespace pw::bluetooth::low_energy {

// The LE Security Mode of a BLE device determines the possible security
// properties of the device. The security mode does not make specific guarantees
// about the current security properties of a device's connections; it sets
// restrictions on the allowable security properties. See Core Spec v5.2 Vol. 3,
// Part C 10.2 for more details.
enum class SecurityMode : uint8_t {
  // In LE Security Mode 1, communication is secured by encryption, and
  // BLE-based services may specify varying requirements for authentication, key
  // size, or Secure Connections protection on the encryption keys.
  kMode1 = 0,

  // In Secure Connections Only mode, all secure communication must use 128 bit,
  // authenticated, and LE Secure Connections-generated encryption keys. If
  // these encryption key properties cannot be satisfied by a device due to
  // system constraints, any connection involving such a device will not be able
  // to secure the link at all. This mode does not prevent unencrypted
  // communication; it merely enforces stricter policies on all encrypted
  // communication.
  kSecureConnectionsOnly = 1
};

}  // namespace pw::bluetooth::low_energy
