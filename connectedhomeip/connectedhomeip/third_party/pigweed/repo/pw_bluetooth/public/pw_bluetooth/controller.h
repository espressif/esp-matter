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

#include "pw_bluetooth/vendor.h"
#include "pw_containers/vector.h"
#include "pw_function/function.h"
#include "pw_result/result.h"
#include "pw_span/span.h"
#include "pw_status/status.h"

namespace pw::bluetooth {

// The Controller class is a shim for communication between the Host and the
// Controller.
class Controller {
 public:
  // The lifetime of the span is only guaranteed for the lifetime of the
  // function call.
  using DataFunction = Function<void(span<const std::byte>)>;

  // Bitmask of features the controller supports.
  enum class FeaturesBits : uint32_t {
    // Indicates support for transferring Synchronous Connection-Oriented link
    // data over the HCI. Offloaded SCO links may still be supported even if HCI
    // SCO isn't.
    kHciSco = (1 << 0),
    // Indicates support for the Set Acl Priority command.
    kSetAclPriorityCommand = (1 << 1),
    // Indicates support for the `LE_Get_Vendor_Capabilities` command documented
    // at
    // https://source.android.com/docs/core/connect/bluetooth/hci_requirements
    kAndroidVendorExtensions = (1 << 2),
    // Bits 3-31 reserved.
  };

  enum class ScoCodingFormat : uint8_t {
    kCvsd,
    kMsbc,
  };

  enum class ScoEncoding : uint8_t {
    k8Bits,
    k16Bits,
  };

  enum class ScoSampleRate : uint8_t {
    k8Khz,
    k16Khz,
  };

  // Closes the controller. `Close` should be called first to safely clean up
  // state (which may be an asynchronous process).
  virtual ~Controller() = default;

  // Sets a function that will be called with HCI event packets received from
  // the controller. This should  be called before `Initialize` or else incoming
  // packets will be dropped. The lifetime of data passed to `func` is only
  // guaranteed for the lifetime of the function call.
  virtual void SetEventFunction(DataFunction func) = 0;

  // Sets a function that will be called with ACL data packets received from the
  // controller. This should be called before `Initialize` or else incoming
  // packets will be dropped. The lifetime of data passed to `func` is only
  // guaranteed for the lifetime of the function call.
  virtual void SetReceiveAclFunction(DataFunction func) = 0;

  // Sets a function that will be called with SCO packets received from the
  // controller. On Classic and Dual Mode stacks, this should be called before
  // `Initialize` or else incoming packets will be dropped. The lifetime of data
  // passed to `func` is only guaranteed for the lifetime of the function call.
  virtual void SetReceiveScoFunction(DataFunction func) = 0;

  // Initializes the controller interface and starts processing packets.
  // `complete_callback` will be called with the result of initialization.
  // `error_callback` will be called for fatal errors that occur after
  // initialization. After a fatal error, this object is invalid. `Close` should
  // be called to ensure a safe clean up.
  virtual void Initialize(Callback<void(Status)> complete_callback,
                          Callback<void(Status)> error_callback) = 0;

  // Closes the controller interface, resetting all state. `callback` will be
  // called when closure is complete. After this method is called, this object
  // should be considered invalid and no other methods should be called
  // (including `Initialize`).
  // `callback` will be called with:
  // OK - the controller interface was successfully closed, or is already closed
  // INTERNAL - the controller interface could not be closed
  virtual void Close(Callback<void(Status)> callback) = 0;

  // Sends an HCI command packet to the controller.
  virtual void SendCommand(span<const std::byte> command) = 0;

  // Sends an ACL data packet to the controller.
  virtual void SendAclData(span<const std::byte> data) = 0;

  // Sends a SCO data packet to the controller.
  virtual void SendScoData(span<const std::byte> data) = 0;

  // Configure the HCI for a SCO connection with the indicated parameters.
  // `SetReceiveScoFunction` must be called before calling this method.
  // `callback will be called with:
  // OK - success, packets can be sent/received.
  // UNIMPLEMENTED - the implementation/controller does not support SCO over HCI
  // ALREADY_EXISTS - a SCO connection is already configured
  // INTERNAL - an internal error occurred
  virtual void ConfigureSco(ScoCodingFormat coding_format,
                            ScoEncoding encoding,
                            ScoSampleRate sample_rate,
                            Callback<void(Status)> callback) = 0;

  // Releases the resources held by an active SCO connection. This should be
  // called when a SCO connection is closed. `ConfigureSco` must be called
  // before calling this method.
  // `callback will be called with:
  // OK - success, the SCO configuration was reset.
  // UNIMPLEMENTED - the implementation/controller does not support SCO over HCI
  // INTERNAL - an internal error occurred
  virtual void ResetSco(Callback<void(Status)> callback) = 0;

  // Calls `callback` with a bitmask of features supported by the controller.
  virtual void GetFeatures(Callback<void(FeaturesBits)> callback) = 0;

  // Encodes the vendor command indicated by `parameters`.
  // `callback` will be called with the result of the encoding request.
  // The lifetime of data passed to `callback` is only guaranteed for the
  // lifetime of the function call.
  virtual void EncodeVendorCommand(
      VendorCommandParameters parameters,
      Callback<void(Result<span<const std::byte>>)> callback) = 0;
};

inline constexpr bool operator&(Controller::FeaturesBits left,
                                Controller::FeaturesBits right) {
  return static_cast<bool>(
      static_cast<std::underlying_type_t<Controller::FeaturesBits>>(left) &
      static_cast<std::underlying_type_t<Controller::FeaturesBits>>(right));
}

inline constexpr Controller::FeaturesBits operator|(
    Controller::FeaturesBits left, Controller::FeaturesBits right) {
  return static_cast<Controller::FeaturesBits>(
      static_cast<std::underlying_type_t<Controller::FeaturesBits>>(left) |
      static_cast<std::underlying_type_t<Controller::FeaturesBits>>(right));
}

inline constexpr Controller::FeaturesBits& operator|=(
    Controller::FeaturesBits& left, Controller::FeaturesBits right) {
  return left = left | right;
}

}  // namespace pw::bluetooth
