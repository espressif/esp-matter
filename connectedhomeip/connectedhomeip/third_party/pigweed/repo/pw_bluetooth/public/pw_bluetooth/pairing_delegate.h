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

#include "pw_bluetooth/peer.h"
#include "pw_function/function.h"

namespace pw::bluetooth {

// Input Capabilities for pairing exchanges.
// See Core Spec v5.3 Volume 3, Part C, Section 5.2.2.4, Table 5.3.
enum class InputCapability : uint8_t { kNone, kConfirmation, kKeyboard };

// Output Capabilities for pairing exchanges.
// See Core Spec v5.3 Volume 3, Part C, Section 5.2.2.4, Table 5.4.
enum class OutputCapability : uint8_t { kNone, kDisplay };

// Pairing event handler implemented by the API client.
class PairingDelegate {
 public:
  // Different types required by the Security Manager for pairing methods.
  // Bluetooth SIG has different requirements for different device capabilities.
  enum class PairingMethod : uint8_t {
    // The user is asked to accept or reject pairing.
    kConsent,

    // The user is shown a 6-digit numerical passkey which they must enter on
    // the
    // peer device.
    kPasskeyDisplay,

    // The user is shown a 6-digit numerical passkey which will also shown on
    // the
    // peer device. The user must compare the passkeys and accept the pairing if
    // the passkeys match.
    kPasskeyComparison,

    // The user is asked to enter a 6-digit passkey.
    kPasskeyEntry
  };

  enum class PairingKeypress : uint8_t {
    // The user has entered a single digit.
    kDigitEntered,

    // The user has erased a single digit.
    kDigitErased,

    // The user has cleared the entire passkey.
    kPasskeyCleared,

    // The user has finished entering the passkey.
    kPasskeyEntered
  };

  // Callback for responding to pairing requests.
  using ResponseCallback =
      pw::Function<void(bool accept, uint32_t entered_passkey)>;

  // Callback for signaling local keypresses to a peer.
  using KeypressCallback =
      pw::Function<void(PeerId peer_id, PairingKeypress keypress)>;

  virtual ~PairingDelegate() = default;

  // Called to initiate a pairing request. The delegate must respond with "true"
  // or "false" in the callback to either accept or reject the pairing request.
  // If the pairing method requires a passkey this is returned as well. It is OK
  // to call `callback` synchronously in this method.
  //
  // Any response from this method will be ignored if the `OnPairingComplete`
  // event has already been sent for `peer`.
  //
  // The `displayed_passkey` parameter should be displayed to the user if
  // `method` equals `PairingMethod::kPasskeyDisplay` or
  // `PairingMethod.kPasskeyComparison`. Otherwise, this parameter has no
  // meaning and should be ignored.
  //
  // The `entered_passkey` parameter of `callback` only has meaning if `method`
  // equals `PairingMethod.kPasskeyEntry`. It will be ignored otherwise.
  virtual void OnPairingRequest(Peer peer,
                                PairingMethod method,
                                uint32_t displayed_passkey,
                                ResponseCallback&& callback) = 0;

  // Called if the pairing procedure for the device with the given ID is
  // completed. This can be due to successful completion or an error (e.g. due
  // to cancellation by the peer, a timeout, or disconnection) which is
  // indicated by `success`.
  virtual void OnPairingComplete(PeerId peer_id, bool success) = 0;

  // Called to notify keypresses from the peer device during pairing using
  // `PairingMethod::kPasskeyDisplay`.
  //
  // This event is used to provide key press events to the delegate for a
  // responsive user experience as the user types the passkey on the peer
  // device. This event will be called once for each keypress.
  virtual void OnRemoteKeypress(PeerId peer_id, PairingKeypress keypress) = 0;

  // Sets a callback that the client may call on local passkey keypresses during
  // a `PairingMethod::kPasskeyEntry` pairing request. Signaled keypresses may
  // be used in the UI of the peer. This should be set immediately by the
  // Bluetooth stack when the delegate is configured by the client unless
  // sending local keypresses is not supported.
  virtual void SetLocalKeypressCallback(KeypressCallback&& callback) = 0;
};

}  // namespace pw::bluetooth
