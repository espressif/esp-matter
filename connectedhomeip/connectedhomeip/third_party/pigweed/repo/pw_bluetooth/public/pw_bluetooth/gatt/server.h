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

#include "pw_bluetooth/gatt/error.h"
#include "pw_bluetooth/gatt/types.h"
#include "pw_bluetooth/internal/raii_ptr.h"
#include "pw_bluetooth/result.h"
#include "pw_bluetooth/types.h"
#include "pw_containers/vector.h"
#include "pw_function/function.h"
#include "pw_span/span.h"
#include "pw_status/status.h"

namespace pw::bluetooth::gatt {

// Parameters for registering a local GATT service.
struct LocalServiceInfo {
  // A unique (within a Server) handle identifying this service.
  Handle handle;

  // Indicates whether this is a primary or secondary service.
  bool primary;

  // The UUID that identifies the type of this service.
  // There may be multiple services with the same UUID.
  Uuid type;

  // The characteristics of this service.
  span<const Characteristic> characteristics;

  // Handles of other services that are included by this service.
  span<const Handle> includes;
};

// Interface for serving a local GATT service. This is implemented by the API
// client.
class LocalServiceDelegate {
 public:
  virtual ~LocalServiceDelegate() = default;

  // Called when there is a fatal error related to this service that forces the
  // service to close. LocalServiceDelegate methods will no longer be called.
  // This invalidates the associated LocalService. It is OK to destroy both
  // `LocalServiceDelegate` and the associated `LocalService::Ptr` from within
  // this method.
  virtual void OnError(Error error) = 0;

  // This notifies the current configuration of a particular
  // characteristic/descriptor for a particular peer. It will be called when the
  // peer GATT client changes the configuration.
  //
  // The Bluetooth stack maintains the state of each peer's configuration across
  // reconnections. As such, this method will also be called when a peer
  // connects for each characteristic with the initial, persisted state of the
  // newly-connected peer's configuration. However, clients should not rely on
  // this state being persisted indefinitely by the Bluetooth stack.
  //
  // Parameters:
  // `peer_id` - The PeerId of the GATT client associated with this particular
  //     CCC.
  // `handle` - The handle of the characteristic associated with the `notify`
  //     and `indicate` parameters.
  // `notify` - True if the client has enabled notifications, false otherwise.
  // `indicate` - True if the client has enabled indications, false otherwise.
  virtual void CharacteristicConfiguration(PeerId peer_id,
                                           Handle handle,
                                           bool notify,
                                           bool indicate) = 0;

  // Called when a peer requests to read the value of a characteristic or
  // descriptor. It is guaranteed that the peer satisfies the permissions
  // associated with this attribute.
  //
  // Parameters:
  // `peer_id` - The PeerId of the GATT client making the read request.
  // `handle` - The handle of the requested descriptor/characteristic.
  // `offset` - The offset at which to start reading the requested value.
  // `result_callback` - Called with the value of the characteristic on success,
  //     or an Error on failure. The value will be truncated to fit in the MTU
  //     if necessary. It is OK to call `result_callback` in `ReadValue`.
  virtual void ReadValue(PeerId peer_id,
                         Handle handle,
                         uint32_t offset,
                         Function<void(Result<Error, span<const std::byte>>)>&&
                             result_callback) = 0;

  // Called when a peer issues a request to write the value of a characteristic
  // or descriptor. It is guaranteed that the peer satisfies the permissions
  // associated with this attribute.
  //
  // Parameters:
  // `peer_id` - The PeerId of the GATT client making the write request.
  // `handle` - The handle of the requested descriptor/characteristic.
  // `offset` - The offset at which to start writing the requested value. If the
  //     offset is 0, any existing value should be overwritten by the new value.
  //     Otherwise, the existing value between offset:(offset + len(value))
  //     should be changed to `value`.
  // `value` - The new value for the descriptor/characteristic.
  // `status_callback` - Called with the result of the write.
  virtual void WriteValue(PeerId peer_id,
                          Handle handle,
                          uint32_t offset,
                          span<const std::byte> value,
                          Function<void(Result<Error>)>&& status_callback) = 0;

  // Called when the MTU of a peer is updated. Also called for peers that are
  // already connected when the server is published. This method is safe to
  // ignore if you do not care about the MTU. It is intended for use cases where
  // throughput needs to be optimized.
  virtual void MtuUpdate(PeerId peer_id, uint16_t mtu) = 0;
};

// Interface provided by the backend to interact with a published service.
// LocalService is valid for the lifetime of a published GATT service. It is
// used to control the service and send notifications/indications.
class LocalService {
 public:
  // The parameters used to signal a characteristic value change from a
  // LocalService to a peer.
  struct ValueChangedParameters {
    // The PeerIds of the peers to signal. The LocalService should respect the
    // Characteristic Configuration associated with a peer+handle when deciding
    // whether to signal it. If empty, all peers are signalled.
    span<const PeerId> peer_ids;
    // The handle of the characteristic value being signaled.
    Handle handle;
    // The new value for the descriptor/characteristic.
    span<const std::byte> value;
  };

  virtual ~LocalService() = default;

  // Sends a notification to peers. Notifications should be used instead of
  // indications when the service does *not* require peer confirmation of the
  // update.
  //
  // Notifications should not be sent to peers which have not enabled
  // notifications on a particular characteristic - if they are sent, they will
  // not be propagated. The Bluetooth stack will track this configuration for
  // the lifetime of the service.
  //
  // Parameters:
  // `parameters` - The parameters associated with the changed characteristic.
  // `completion_callback` - Called when the notification has been sent.
  //     Additional values should not be notified until this callback is called.
  virtual void NotifyValue(const ValueChangedParameters& parameters,
                           Closure&& completion_callback) = 0;

  // Sends an indication to peers. Indications should be used instead of
  // notifications when the service *does* require peer confirmation of the
  // update.
  //
  // Indications should not be sent to peers which have not enabled indications
  // on a particular characteristic - if they are sent, they will not be
  // propagated. The Bluetooth stack will track this configuration for the
  // lifetime of the service.
  //
  // If any of the peers in `update.peer_ids` fails to confirm the indication
  // within the ATT transaction timeout (30 seconds per Bluetooth 5.2 Vol. 4
  // Part G 3.3.3), the link between the peer and the local adapter will be
  // closed.
  //
  // Parameters:
  // `parameters` - The parameters associated with the changed characteristic.
  // `confirmation` - When all the peers listed in `parameters.peer_ids` have
  //     confirmed the indication, `confirmation` is called. If the
  //     implementation wishes to receive indication confirmations on a per-peer
  //     basis, they should send this event with a single PeerId in
  //     `parameters.peer_ids`. Additional values should not be indicated until
  //     this callback is called.
  virtual void IndicateValue(const ValueChangedParameters& parameters,
                             Function<void(Result<Error>)>&& confirmation) = 0;

 private:
  // Unpublish the local service. This method is called by the
  // ~LocalService::Ptr() when it goes out of scope, the API client should never
  // call this method.
  virtual void UnpublishService() = 0;

 public:
  // Movable LocalService smart pointer. When the LocalService::Ptr object is
  // destroyed the service will be unpublished.
  using Ptr = internal::RaiiPtr<LocalService, &LocalService::UnpublishService>;
};

// Interface for a GATT server that serves many GATT services.
class Server {
 public:
  enum class PublishServiceError {
    kInternalError = 0,

    // The service handle provided was not unique.
    kInvalidHandle = 1,

    // Invalid service UUID provided.
    kInvalidUuid = 2,

    // Invalid service characteristics provided.
    kInvalidCharacteristics = 3,

    // Invalid service includes provided.
    kInvalidIncludes = 4,
  };

  // The Result passed by PublishService.
  using PublishServiceResult = Result<PublishServiceError, LocalService::Ptr>;

  virtual ~Server() = default;

  // Publishes the service defined by `info` and implemented by `delegate` so
  // that it is available to all remote peers.
  //
  // The caller must assign distinct handles to the characteristics and
  // descriptors listed in `info`. These identifiers will be used in requests
  // sent to `delegate`. On success, a `LocalService::Ptr` is returned. When the
  // `LocalService::Ptr` is destroyed or an error occurs
  // (LocalServiceDelegate.OnError), the service will be unpublished.
  virtual void PublishService(
      const LocalServiceInfo& info,
      LocalServiceDelegate* delegate,
      Function<void(PublishServiceResult)>&& result_callback) = 0;
};

}  // namespace pw::bluetooth::gatt
