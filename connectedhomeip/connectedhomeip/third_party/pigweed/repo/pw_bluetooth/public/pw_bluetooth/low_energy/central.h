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

#include <memory>
#include <optional>

#include "pw_bluetooth/internal/raii_ptr.h"
#include "pw_bluetooth/low_energy/advertising_data.h"
#include "pw_bluetooth/low_energy/connection.h"
#include "pw_bluetooth/result.h"
#include "pw_bluetooth/types.h"
#include "pw_chrono/system_clock.h"
#include "pw_containers/vector.h"
#include "pw_function/function.h"

namespace pw::bluetooth::low_energy {

// Represents the LE central role. Used to scan and connect to peripherals.
class Central {
 public:
  // Represents an ongoing LE scan.
  class Scan {
   public:
    enum class ScanError : uint8_t { kCanceled = 0 };

    virtual ~Scan() = 0;

    // Set a callback that will be called if the scan is stopped due to an error
    // in the BLE stack.
    virtual void SetErrorCallback(Function<void(ScanError)>&& callback) = 0;

   private:
    // Stop the current scan. This method is called by the ~Scan::Ptr() when it
    // goes out of scope, the API client should never call this method.
    virtual void StopScan() = 0;

   public:
    // Movable Scan smart pointer. The controller will continue scanning until
    // the returned Scan::Ptr is destroyed.
    using Ptr = internal::RaiiPtr<Scan, &Scan::StopScan>;
  };

  // Filter parameters for use during a scan. A discovered peer only matches the
  // filter if it satisfies all of the present filter parameters.
  struct ScanFilter {
    // Filter based on advertised service UUID.
    std::optional<Uuid> service_uuid;

    // Filter based on service data containing the given UUID.
    std::optional<Uuid> service_data_uuid;

    // Filter based on a manufacturer identifier present in the manufacturer
    // data. If this filter parameter is set, then the advertising payload must
    // contain manufacturer specific data with the provided company identifier
    // to satisfy this filter. Manufacturer identifiers can be found at
    // https://www.bluetooth.com/specifications/assigned-numbers/company-identifiers/
    std::optional<uint16_t> manufacturer_id;

    // Filter based on whether or not a device is connectable. For example, a
    // client that is only interested in peripherals that it can connect to can
    // set this to true. Similarly a client can scan only for broadcasters by
    // setting this to false.
    std::optional<bool> connectable;

    // Filter results based on a portion of the advertised device name.
    // Substring matches are allowed.
    // The name length must be at most pw::bluetooth::kMaxDeviceNameLength.
    std::optional<std::string_view> name;

    // Filter results based on the path loss of the radio wave. A device that
    // matches this filter must satisfy the following:
    //   1. Radio transmission power level and received signal strength must be
    //      available for the path loss calculation.
    //   2. The calculated path loss value must be less than, or equal to,
    //      `max_path_loss`.
    //
    // NOTE: This field is calculated using the RSSI and TX Power information
    // obtained from advertising and scan response data during a scan procedure.
    // It should NOT be confused with information for an active connection
    // obtained using the "Path Loss Reporting" feature.
    std::optional<uint8_t> max_path_loss;
  };

  // Parameters used during a scan.
  struct ScanOptions {
    // List of filters for use during a scan. A peripheral that satisfies any of
    // these filters will be reported. At least 1 filter must be specified.
    // While not recommended, clients that require that all peripherals be
    // reported can specify an empty filter.
    Vector<ScanFilter> filters;

    // The time interval between scans.
    // Time = N * 0.625ms
    // Range: 0x0004 (2.5ms) - 10.24ms (0x4000)
    // Default: 10ms
    uint16_t interval = 0x0010;

    // The duration of the scan. The window must be less than or equal to the
    // interval.
    // Time = N * 0.625ms
    // Range: 0x0004 (2.5ms) - 10.24ms (0x4000)
    // Default: 10ms
    uint16_t window = 0x0010;
  };

  // Information obtained from advertising and scan response data broadcast by a
  // peer.
  struct ScanData {
    // The radio transmit power level.
    // NOTE: This field should NOT be confused with the "connection TX Power
    // Level" of a peer that is currently connected to the system obtained via
    // the "Transmit Power reporting" feature.
    std::optional<uint8_t> tx_power;

    // The appearance of the device.
    std::optional<Appearance> appearance;

    Vector<Uuid> service_uuids;

    Vector<ServiceData> service_data;

    Vector<ManufacturerData> manufacturer_data;

    // String representing a URI to be advertised, as defined in IETF STD
    // 66: https://tools.ietf.org/html/std66. Each entry should be a UTF-8
    // string including the scheme. For more information, see
    // https://www.iana.org/assignments/uri-schemes/uri-schemes.xhtml for
    // allowed schemes; NOTE: Bluetooth advertising compresses schemas over the
    // air to save space. See
    // https://www.bluetooth.com/specifications/assigned-numbers/uri-scheme-name-string-mapping.
    Vector<std::string_view> uris;

    // The time when this scan data was received.
    chrono::SystemClock::time_point timestamp;
  };

  struct ScanResult {
    // ScanResult is non-copyable becuase strings are only valid in the
    // result callback.
    ScanResult(const ScanResult&) = delete;
    ScanResult& operator=(const ScanResult&) = delete;

    // Uniquely identifies this peer on the current system.
    PeerId peer_id;

    // Whether or not this peer is connectable. Non-connectable peers are
    // typically in the LE broadcaster role.
    bool connectable;

    // The last observed signal strength of this peer. This field is only
    // present for a peer that is broadcasting. The RSSI can be stale if the
    // peer has not been advertising.
    //
    // NOTE: This field should NOT be confused with the "connection RSSI" of a
    // peer that is currently connected to the system.
    std::optional<uint8_t> rssi;

    // Information from advertising and scan response data broadcast by this
    // peer. This contains the advertising data last received from the peer.
    ScanData scan_data;

    // The name of this peer. The name is often obtained during a scan procedure
    // and can get updated during the name discovery procedure following a
    // connection.
    //
    // This field is present if the name is known.
    std::optional<std::string_view> name;

    // Timestamp of when the information in this `ScanResult` was last updated.
    chrono::SystemClock::time_point last_updated;
  };

  // Possible errors returned by `Connect`.
  enum class ConnectError : uint8_t {
    // The peer ID is unknown.
    kUnknownPeer,

    // The `ConnectionOptions` were invalid.
    kInvalidOptions,

    // A connection to the peer already exists.
    kAlreadyExists,

    // A connection could not be established.
    kCouldNotBeEstablished,
  };

  enum class StartScanError : uint8_t {
    // A scan is already in progress. Only 1 scan may be active at a time.
    kScanInProgress,
    // Some of the scan options are invalid.
    kInvalidParameters,
    // An internal error occurred and a scan could not be started.
    kInternal,
  };

  // The Result type returned by Connect() via the passed callback.
  using ConnectResult = Result<ConnectError, Connection::Ptr>;

  virtual ~Central() = default;

  // Connect to the peer with the given identifier.
  //
  // The requested `Connection` represents the client's interest in the LE
  // connection to the peer. Destroying the `Connection` will disconnect from
  // the peer. Only 1 connection per peer may exist at a time.
  //
  // The `Connection` will be closed by the system if the connection to the peer
  // is lost or an error occurs, as indicated by `Connection.OnError`.
  //
  // Parameters:
  // `id` - Identifier of the peer to initiate a connection to.
  // `options` - Options used to configure the connection.
  // `callback` - Called when a connection is successfully established, or an
  //     error occurs.
  //
  // Possible errors are documented in `ConnectError`.
  virtual void Connect(PeerId peer_id,
                       ConnectionOptions options,
                       Function<void(ConnectResult)>&& callback) = 0;

  // Scans for nearby LE peripherals and broadcasters. The lifetime of the scan
  // session is tied to the returned `Scan` object. Once a scan is started,
  // `scan_result_callback` will be called with scan results. Only 1 scan may be
  // active at a time. It is OK to destroy the `Scan::Ptr` object in
  // `scan_result_callback` to stop scanning (no more results will be returned).
  //
  // Parameters:
  // `options`  - Options used to configure the scan session.
  // `scan_result_callback` - If scanning starts successfully,called for LE
  //     peers that satisfy the filters indicated in `options`. The initial
  //     calls may report recently discovered peers. Subsequent calls will
  //     be made only when peers have been scanned or updated since the last
  //     call.
  // `scan_started_callback` - Called with a `Scan` object if the
  //     scan successfully starts, or a `ScanError` otherwise.
  virtual void Scan(ScanOptions options,
                    Function<void(ScanResult)>&& scan_result_callback,
                    Function<void(Result<StartScanError, Scan::Ptr>)>&&
                        scan_started_callback) = 0;
};

}  // namespace pw::bluetooth::low_energy
