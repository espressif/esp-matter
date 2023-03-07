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

#include "pw_bluetooth/gatt/client.h"
#include "pw_bluetooth/internal/raii_ptr.h"
#include "pw_bluetooth/types.h"

namespace pw::bluetooth::low_energy {

// Actual connection parameters returned by the controller.
struct ConnectionParameters {
  // The connection interval indicates the frequency of link layer connection
  // events over which data channel PDUs can be transmitted. See Core Spec v5.3,
  // Vol 6, Part B, Section 4.5.1 for more information on the link layer
  // connection events.
  // Range: 0x0006 to 0x0C80
  // Time: N * 1.25 ms
  // Time Range: 7.5 ms to 4 s.
  uint16_t interval;

  // The maximum allowed peripheral connection latency in number of connection
  // events. See Core Spec v5.3, Vol 6, Part B, Section 4.5.2.
  // Range: 0x0000 to 0x01F3
  uint16_t latency;

  // This defines the maximum time between two received data packet PDUs
  // before the connection is considered lost. See Core Spec v5.3, Vol 6, Part
  // B, Section 4.5.2.
  // Range: 0x000A to 0x0C80
  // Time: N * 10 ms
  // Time Range: 100 ms to 32 s
  uint16_t supervision_timeout;
};

// Connection parameters that either the local device or a peer device are
// requesting.
struct RequestedConnectionParameters {
  // Minimum value for the connection interval. This shall be less than or equal
  // to `max_interval`. The connection interval indicates the frequency of link
  // layer connection events over which data channel PDUs can be transmitted.
  // See Core Spec v5.3, Vol 6, Part B, Section 4.5.1 for more information on
  // the link layer connection events.
  // Range: 0x0006 to 0x0C80
  // Time: N * 1.25 ms
  // Time Range: 7.5 ms to 4 s.
  uint16_t min_interval;

  // Maximum value for the connection interval. This shall be greater than or
  // equal to `min_interval`. The connection interval indicates the frequency
  // of link layer connection events over which data channel PDUs can be
  // transmitted.  See Core Spec v5.3, Vol 6, Part B, Section 4.5.1 for more
  // information on the link layer connection events.
  // Range: 0x0006 to 0x0C80
  // Time: N * 1.25 ms
  // Time Range: 7.5 ms to 4 s.
  uint16_t max_interval;

  // Maximum peripheral latency for the connection in number of connection
  // events. See Core Spec v5.3, Vol 6, Part B, Section 4.5.2.
  // Range: 0x0000 to 0x01F3
  uint16_t max_latency;

  // This defines the maximum time between two received data packet PDUs
  // before the connection is considered lost. See Core Spec v5.3, Vol 6, Part
  // B, Section 4.5.2.
  // Range: 0x000A to 0x0C80
  // Time: N * 10 ms
  // Time Range: 100 ms to 32 s
  uint16_t supervision_timeout;
};

// Represents parameters that are set on a per-connection basis.
struct ConnectionOptions {
  // When true, the connection operates in bondable mode. This means pairing
  // will form a bond, or persist across disconnections, if the peer is also
  // in bondable mode. When false, the connection operates in non-bondable
  // mode, which means the local device only allows pairing that does not form
  // a bond.
  bool bondable_mode = true;

  // When present, service discovery performed following the connection is
  // restricted to primary services that match this field. Otherwise, by
  // default all available services are discovered.
  std::optional<Uuid> service_filter;

  // When present, specifies the initial connection parameters. Otherwise, the
  // connection parameters will be selected by the implementation.
  std::optional<RequestedConnectionParameters> parameters;
};

/// Class that represents a connection to a peer. This can be used to interact
/// with GATT services and establish LE L2CAP channels.
///
/// This lifetime of this object is tied to that of the LE connection it
/// represents. Destroying the object results in a disconnection.
class Connection {
 public:
  // Possible errors when updating the connection parameters.
  enum class ConnectionParameterUpdateError : uint8_t {
    kFailure,
    kInvalidParameters,
    kRejected,
  };

  // Possible reasons a connection was disconnected.
  enum class DisconnectReason : uint8_t {
    kFailure,
    kRemoteUserTerminatedConnection,
    // This usually indicates that the link supervision timeout expired.
    kConnectionTimeout,
  };

  // If a disconnection has not occurred, destroying this object will result in
  // disconnection.
  virtual ~Connection() = default;

  // Sets a callback that will be called when the peer disconnects or there is a
  // connection error that causes a disconnection. This should be configured by
  // the client immediately after establishing the connection. `callback` will
  // not be called for disconnections initiated by the client (e.g. by
  // destroying `Connection`). It is OK to destroy this object from within
  // `callback`.
  virtual void SetDisconnectCallback(
      Function<void(DisconnectReason)>&& callback) = 0;

  // Returns a GATT client to the connected peer that is valid for the lifetime
  // of this connection. The client is valid for the lifetime of this
  // connection.
  virtual gatt::Client* GattClient() = 0;

  // Returns the current ATT Maximum Transmission Unit. By subtracting ATT
  // headers from the MTU, the maximum payload size of messages can be
  // calculated.
  virtual uint16_t AttMtu() = 0;

  // Sets a callback that will be called with the new ATT MTU whenever it is
  // updated.
  virtual void SetAttMtuChangeCallback(Function<void(uint16_t)> callback) = 0;

  // Returns the current connection parameters.
  virtual ConnectionParameters Parameters() = 0;

  // Requests an update to the connection parameters. `callback` will be called
  // with the result of the request.
  virtual void RequestConnectionParameterUpdate(
      RequestedConnectionParameters parameters,
      Function<void(Result<ConnectionParameterUpdateError>)>&& callback) = 0;

 private:
  // Request to disconnect this connection. This method is called by the
  // ~Connection::Ptr() when it goes out of scope, the API client should never
  // call this method.
  virtual void Disconnect() = 0;

 public:
  // Movable Connection smart pointer. When Connection::Ptr is destroyed the
  // Connection will disconnect automatically.
  using Ptr = internal::RaiiPtr<Connection, &Connection::Disconnect>;
};

}  // namespace pw::bluetooth::low_energy
