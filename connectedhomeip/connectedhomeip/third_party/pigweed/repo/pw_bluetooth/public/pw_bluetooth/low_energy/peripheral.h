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
#include <memory>

#include "pw_bluetooth/internal/raii_ptr.h"
#include "pw_bluetooth/low_energy/advertising_data.h"
#include "pw_bluetooth/low_energy/connection.h"
#include "pw_bluetooth/result.h"
#include "pw_bluetooth/types.h"
#include "pw_function/function.h"
#include "pw_status/status.h"

namespace pw::bluetooth::low_energy {

// `AdvertisedPeripheral` instances are valid for the duration of advertising.
class AdvertisedPeripheral {
 public:
  virtual ~AdvertisedPeripheral() = default;

  // Set a callback that will be called when an error occurs and advertising
  // has been stopped (invalidating this object). It is OK to destroy the
  // `AdvertisedPeripheral::Ptr` object from within `callback`.
  virtual void SetErrorCallback(Closure callback) = 0;

  // For connectable advertisements, this callback will be called when an LE
  // central connects to the advertisement. It is recommended to set this
  // callback immediately after advertising starts to avoid dropping
  // connections.
  //
  // The returned Connection can be used to interact with the peer. It also
  // represents a peripheral's ownership over the connection: the client can
  // drop the object to request a disconnection. Similarly, the Connection
  // error handler is called by the system to indicate that the connection to
  // the peer has been lost. While connections are exclusive among peripherals,
  // they may be shared with centrals.
  //
  // If advertising is not stopped, this callback may be called with multiple
  // connections over the lifetime of an advertisement. It is OK to destroy
  // the `AdvertisedPeripheral::Ptr` object from within `callback` in order to
  // stop advertising.
  virtual void SetConnectionCallback(
      Function<void(Connection::Ptr)>&& callback) = 0;

 private:
  // Stop advertising. This method is called by the ~AdvertisedPeripheral::Ptr()
  // when it goes out of scope, the API client should never call this method.
  virtual void StopAdvertising() = 0;

 public:
  // Movable AdvertisedPeripheral smart pointer. The peripheral will continue
  // advertising until the returned AdvertisedPeripheral::Ptr is destroyed.
  using Ptr = internal::RaiiPtr<AdvertisedPeripheral,
                                &AdvertisedPeripheral::StopAdvertising>;
};

// Represents the LE Peripheral role, which advertises and is connected to.
class Peripheral {
 public:
  // The range of the time interval between advertisements. Shorter intervals
  // result in faster discovery at the cost of higher power consumption. The
  // exact interval used is determined by the Bluetooth controller.
  // Time = N * 0.625ms.
  // Time range: 0x0020 (20ms) - 0x4000 (10.24s)
  struct AdvertisingIntervalRange {
    uint16_t min = 0x0800;  // 1.28s
    uint16_t max = 0x0800;  // 1.28s
  };

  // Represents the parameters for configuring advertisements.
  struct AdvertisingParameters {
    // The fields that will be encoded in the data section of advertising
    // packets.
    AdvertisingData data;

    // The fields that are to be sent in a scan response packet. Clients may use
    // this to send additional data that does not fit inside an advertising
    // packet on platforms that do not support the advertising data length
    // extensions.
    //
    // If present advertisements will be configured to be scannable.
    std::optional<AdvertisingData> scan_response;

    // See `AdvertisingIntervalRange` documentation.
    AdvertisingIntervalRange interval_range;

    // If present, the controller will broadcast connectable advertisements
    // which allow peers to initiate connections to the Peripheral. The fields
    // of `ConnectionOptions` will configure any connections set up from
    // advertising.
    std::optional<ConnectionOptions> connection_options;
  };

  // Errors returned by `Advertise`.
  enum class AdvertiseError {
    // The operation or parameters requested are not supported on the current
    // hardware.
    kNotSupported = 1,

    // The provided advertising data exceeds the maximum allowed length when
    // encoded.
    kAdvertisingDataTooLong = 2,

    // The provided scan response data exceeds the maximum allowed length when
    // encoded.
    kScanResponseDataTooLong = 3,

    // The requested parameters are invalid.
    kInvalidParameters = 4,

    // This may be called if the maximum number of simultaneous advertisements
    // has already been reached.
    kNotEnoughAdvertisingSlots = 5,

    // Advertising could not be initiated due to a hardware or system error.
    kFailed = 6,
  };

  using AdvertiseCallback =
      Function<void(Result<AdvertiseError, AdvertisedPeripheral::Ptr>)>;

  virtual ~Peripheral() = default;

  // Start advertising continuously as a LE peripheral. If advertising cannot
  // be initiated then `result_callback` will be called with an error. Once
  // started, advertising can be stopped by destroying the returned
  // `AdvertisedPeripheral::Ptr`.
  //
  // If the system supports multiple advertising, this may be called as many
  // times as there are advertising slots. To reconfigure an advertisement,
  // first close the original advertisement and then initiate a new
  // advertisement.
  //
  // Parameters:
  // `parameters` - Parameters used while configuring the advertising
  //     instance.
  // `result_callback` - Called once advertising has started or failed. On
  //     success, called with an `AdvertisedPeripheral` that models the lifetime
  //     of the advertisement. Destroying it will stop advertising.
  virtual void Advertise(const AdvertisingParameters& parameters,
                         AdvertiseCallback&& result_callback) = 0;
};

}  // namespace pw::bluetooth::low_energy
