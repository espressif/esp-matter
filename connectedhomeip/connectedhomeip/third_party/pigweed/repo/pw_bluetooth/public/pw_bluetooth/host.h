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
#include <string_view>

#include "pw_bluetooth/controller.h"
#include "pw_bluetooth/gatt/client.h"
#include "pw_bluetooth/gatt/server.h"
#include "pw_bluetooth/low_energy/bond_data.h"
#include "pw_bluetooth/low_energy/central.h"
#include "pw_bluetooth/low_energy/peripheral.h"
#include "pw_bluetooth/low_energy/security_mode.h"
#include "pw_bluetooth/pairing_delegate.h"
#include "pw_bluetooth/peer.h"
#include "pw_bluetooth/types.h"
#include "pw_function/function.h"
#include "pw_span/span.h"
#include "pw_status/status.h"

namespace pw::bluetooth {

// Host is the entrypoint API for interacting with a Bluetooth host stack. Host
// is an abstract class that is implemented by a host stack implementation.
class Host {
 public:
  // Represents the persistent configuration of a single Host instance. This is
  // used for identity representation in advertisements & bonding secrets
  // recall.
  struct PersistentData {
    // The local Identity Resolving Key used by a Host to generate Resolvable
    // Private Addresses when privacy is enabled. May be absent for hosts that
    // do not use LE privacy, or that only use Non-Resolvable Private Addresses.
    //
    // NOTE: This key is distributed to LE peers during pairing procedures. The
    // client must take care to assign an IRK that consistent with the local
    // Host identity.
    std::optional<Key> identity_resolving_key;

    // All bonds that use a public identity address must contain the same local
    // address.
    span<const low_energy::BondData> bonds;
  };

  // The security level required for this pairing. This corresponds to the
  // security levels defined in the Security Manager Protocol in Core spec v5.3,
  // Vol 3, Part H, Section 2.3.1
  enum class PairingSecurityLevel : uint8_t {
    // Encrypted without person-in-the-middle protection (unauthenticated)
    kEncrypted,
    // Encrypted with person-in-the-middle protection (authenticated), although
    // this level of security does not fully protect against passive
    // eavesdroppers
    kAuthenticated,
    // Encrypted with person-in-the-middle protection (authenticated).
    // This level of security fully protects against eavesdroppers.
    kLeSecureConnections,
  };

  // Whether or not the device should form a bluetooth bond during the pairing
  // prodecure. As described in Core Spec v5.2, Vol 3, Part C, Sec 4.3
  enum class BondableMode : uint8_t {
    // The device will form a bond during pairing with peers
    kBondable,
    // The device will not form a bond during pairing with peers
    kNonBondable,
  };

  // Parameters that give a caller more fine-grained control over the pairing
  // process.
  struct PairingOptions {
    // Determines the Security Manager security level to pair with.
    PairingSecurityLevel security_level = PairingSecurityLevel::kAuthenticated;

    // Indicated whether the device should form a bond or not during pairing. If
    // not present, interpreted as bondable mode.
    BondableMode bondable_mode = BondableMode::kBondable;
  };

  // `Close` should complete before `Host` is destroyed.
  virtual ~Host() = default;

  // Initializes the host stack. Vendor specific controller initialization (e.g.
  // loading firmware) must be done before initializing `Host`.
  //
  // Parameters:
  // `controller` - Pointer to a concrete `Controller` that the host stack
  //     should use to communicate with the controller.
  // `data` - Data to persist from a previous instance of `Host`.
  // `on_initialization_complete` - Called when initialization is complete.
  //     Other methods should not be called until initialization completes.
  virtual void Initialize(
      Controller* controller,
      PersistentData data,
      Function<void(Status)>&& on_initialization_complete) = 0;

  // Safely shuts down the host, ending all active Bluetooth procedures:
  // - All objects/pointers associated with this host are destroyed/invalidated
  //   and all connections disconnected.
  // - All scanning and advertising procedures are stopped.
  //
  // The Host may send events or call callbacks as procedures get terminated.
  // `callback` will be called once all procedures have terminated.
  virtual void Close(Closure callback) = 0;

  // Returns a pointer to the Central API, which is used to scan and connect to
  // peers.
  virtual low_energy::Central* Central() = 0;

  // Returns a pointer to the Peripheral API, which is used to advertise and
  // accept connections from peers.
  virtual low_energy::Peripheral* Peripheral() = 0;

  // Returns a pointer to the GATT Server API, which is used to publish GATT
  // services.
  virtual gatt::Server* GattServer() = 0;

  // Deletes a peer from the Bluetooth host. If the peer is connected, it will
  // be disconnected. `peer_id` will no longer refer to any peer.
  //
  // Returns `OK` after no peer exists that's identified by `peer_id` (even
  // if it didn't exist), `ABORTED` if the peer could not be disconnected or
  // deleted and still exists.
  virtual Status ForgetPeer(PeerId peer_id) = 0;

  // Enable or disable the LE privacy feature. When enabled, the host will use a
  // private device address in all LE procedures. When disabled, the public
  // identity address will be used instead (which is the default).
  virtual void EnablePrivacy(bool enabled) = 0;

  // Set the GAP LE Security Mode of the host. Only encrypted,
  // connection-based security modes are supported, i.e. Mode 1 and Secure
  // Connections Only mode. If the security mode is set to Secure Connections
  // Only, any existing encrypted connections which do not meet the security
  // requirements of Secure Connections Only mode will be disconnected.
  virtual void SetSecurityMode(low_energy::SecurityMode security_mode) = 0;

  // Assigns the pairing delegate that will respond to authentication challenges
  // using the given I/O capabilities. Calling this method cancels any on-going
  // pairing procedure started using a previous delegate. Pairing requests will
  // be rejected if no PairingDelegate has been assigned.
  virtual void SetPairingDelegate(InputCapability input,
                                  OutputCapability output,
                                  PairingDelegate* pairing_delegate) = 0;

  // NOTE: This is intended to satisfy test scenarios that require pairing
  // procedures to be initiated without relying on service access. In normal
  // operation, Bluetooth security is enforced during service access.
  //
  // Initiates pairing to the peer with the supplied `peer_id` and `options`.
  // Returns an error if no connected peer with `peer_id` is found or the
  // pairing procedure fails.
  //
  // If `options` specifies a higher security level than the current pairing,
  // this method attempts to raise the security level. Otherwise this method has
  // no effect and returns success.
  //
  // Returns the following errors via `callback`:
  // `NOT_FOUND` - The peer `peer_id` was not found.
  // `ABORTED` - The pairing procedure failed.
  virtual void Pair(PeerId peer_id,
                    PairingOptions options,
                    Function<void(Status)>&& callback) = 0;

  // Configures a callback to be called when new bond data for a peer has been
  // created. This data should be persisted and used to initialize Host in the
  // future. New bond data may be received for an already bonded peer, in which
  // case the new data should overwrite the old data.
  virtual void SetBondDataCallback(
      Function<void(low_energy::BondData)>&& callback) = 0;

  // Looks up the `PeerId` corresponding to `address`. If `address` does not
  // correspond to a known peer, a new `PeerId` will be generated for the
  // address. If a `PeerId` cannot be generated, std::nullopt will be returned.
  virtual std::optional<PeerId> PeerIdFromAddress(Address address) = 0;

  // Looks up the Address corresponding to `peer_id`. Returns null if `peer_id`
  // does not correspond to a known peer.
  virtual std::optional<Address> DeviceAddressFromPeerId(PeerId peer_id) = 0;
};

}  // namespace pw::bluetooth
