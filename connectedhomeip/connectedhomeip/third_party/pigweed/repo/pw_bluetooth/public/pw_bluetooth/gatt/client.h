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

#include "pw_bluetooth/gatt/constants.h"
#include "pw_bluetooth/gatt/error.h"
#include "pw_bluetooth/gatt/types.h"
#include "pw_bluetooth/internal/raii_ptr.h"
#include "pw_bluetooth/result.h"
#include "pw_bluetooth/types.h"
#include "pw_containers/vector.h"
#include "pw_function/function.h"
#include "pw_span/span.h"

namespace pw::bluetooth::gatt {

// Represents a GATT service on a remote GATT server.
// Clients should call `SetErrorCallback` before using in order to handle fatal
// errors.
class RemoteService {
 public:
  enum class RemoteServiceError {
    // The service has been modified or removed.
    kServiceRemoved = 0,

    // The peer serving this service has disconnected.
    kPeerDisconnected = 1,
  };

  // Wrapper around a possible truncated value received from the server.
  struct ReadValue {
    // Characteristic or descriptor handle.
    Handle handle;

    // The value of the characteristic or descriptor.
    Vector<std::byte> value;

    // True if `value` might be truncated (the buffer was completely filled by
    // the server and the read was a short read).  `ReadCharacteristic` or
    // `ReadDescriptor` should be used to read the complete value.
    bool maybe_truncated;
  };

  // A result returned by `ReadByType`.
  struct ReadByTypeResult {
    // Characteristic or descriptor handle.
    Handle handle;

    // The value of the characteristic or descriptor, if it was read
    // successfully, or an error explaining why the value could not be read.
    Result<Error, ReadValue> result;
  };

  // Represents the supported options to read a long characteristic or
  // descriptor value from a server. Long values are those that may not fit in a
  // single message (longer than 22 bytes).
  struct LongReadOptions {
    // The byte to start the read at. Must be less than the length of the
    // value.
    uint16_t offset = 0;

    // The maximum number of bytes to read.
    uint16_t max_bytes = kMaxValueLength;
  };

  // Represents the supported write modes for writing characteristics &
  // descriptors to the server.
  enum class WriteMode : uint8_t {
    // Wait for a response from the server before returning but do not verify
    // the echo response. Supported for both characteristics and descriptors.
    kDefault = 0,

    // Every value blob is verified against an echo response from the server.
    // The procedure is aborted if a value blob has not been reliably delivered
    // to the peer. Only supported for characteristics.
    kReliable = 1,

    // Delivery will not be confirmed before returning. Writing without a
    // response is only supported for short characteristics with the
    // `WRITE_WITHOUT_RESPONSE` property. The value must fit into a single
    // message. It is guaranteed that at least 20 bytes will fit into a single
    // message. If the value does not fit, a `kFailure` error will be produced.
    // The value will be written at offset 0. Only supported for
    // characteristics.
    kWithoutResponse = 2,
  };

  // Represents the supported options to write a characteristic/descriptor value
  // to a server.
  struct WriteOptions {
    // The mode of the write operation. For descriptors, only
    // `WriteMode::kDefault` is supported
    WriteMode mode = WriteMode::kDefault;

    // Request a write starting at the byte indicated.
    // Must be 0 if `mode` is `WriteMode.kWithoutResponse`.
    uint16_t offset = 0;
  };

  using ReadByTypeCallback = Function<void(Result<Vector<ReadByTypeResult>>)>;
  using ReadCallback = Function<void(Result<ReadValue>)>;
  using NotificationCallback = Function<void(ReadValue)>;

  // Set a callback that will be called when there is an error with this
  // RemoteService, after which this RemoteService will be invalid.
  void SetErrorCallback(Function<void(RemoteServiceError)>&& error_callback);

  // Calls `characteristic_callback` with the characteristics and descriptors in
  // this service.
  void DiscoverCharacteristics(
      Function<void(Characteristic)>&& characteristic_callback);

  // Reads characteristics and descriptors with the specified type. This method
  // is useful for reading values before discovery has completed, thereby
  // reducing latency.
  // `uuid` - The UUID of the characteristics/descriptors to read.
  // `result_callback` - Results are returned via this callback. Results may be
  //   empty if no matching values are read. If reading a value results in a
  //   permission error, the handle and error will be included.
  //
  // This may fail with the following errors:
  // kInvalidParameters: if `uuid` refers to an internally reserved descriptor
  //     type (e.g. the Client Characteristic Configuration descriptor).
  // kTooManyResults: More results were read than can fit
  //    in a Vector. Consider reading characteristics/descriptors individually
  //    after performing discovery.
  // kFailure: The server returned an error not specific to a single result.
  void ReadByType(Uuid uuid, ReadByTypeCallback&& result_callback);

  // Reads the value of a characteristic.
  // `handle` - The handle of the characteristic to be read.
  // `options` - If null, a short read will be performed, which may be truncated
  //     to what fits in a single message (at least 22 bytes). If long read
  //     options are present, performs a long read with the indicated options.
  // `result_callback` - called with the result of the read and the value of the
  //     characteristic if successful.
  //
  // This may fail with the following errors:
  // kInvalidHandle - if `handle` is invalid
  // kInvalidParameters - if `options is invalid
  // kReadNotPermitted or kInsufficient* if the server rejects the request.
  // kFailure if the server returns an error not covered by the above errors.
  void ReadCharacteristic(Handle handle,
                          std::optional<LongReadOptions> options,
                          ReadCallback&& result_callback);

  // Writes `value` to the characteristic with `handle` using the provided
  // `options`.  It is not recommended to send additional writes while a write
  // is already in progress (the server may receive simultaneous writes in any
  // order).
  //
  // Parameters:
  // `handle` - Handle of the characteristic to be written to
  // `value` - The value to be written.
  // `options` - Options that apply to the write.
  //
  // This may fail with the following errors:
  // kInvalidHandle - if `handle` is invalid
  // kInvalidParameters - if `options is invalid
  // kWriteNotPermitted or kInsufficient* if the server rejects the request.
  // kFailure if the server returns an error not covered by the above errors.
  void WriteCharacteristic(Handle handle,
                           span<const std::byte> value,
                           WriteOptions options,
                           Function<void(Result<Error>)>&& result_callback);

  // Reads the value of the characteristic descriptor with `handle` and
  // returns it in the reply.
  // `handle` - The descriptor handle to read.
  // `options` - Options that apply to the read.
  // `result_callback` - Returns a result containing the value of the descriptor
  //     on success.
  //
  // This may fail with the following errors:
  // `kInvalidHandle` - if `handle` is invalid.
  // `kInvalidParameters` - if `options` is invalid.
  // `kReadNotPermitted` or `INSUFFICIENT_*` - if the server rejects the read
  // request. `kFailure` - if the server returns an error.
  void ReadDescriptor(Handle handle,
                      std::optional<LongReadOptions> options,
                      ReadCallback&& result_callback);

  void WriteDescriptor(Handle handle,
                       span<const std::byte> value,
                       WriteOptions options,
                       Function<void(Result<Error>)>&& result_callback);

  // Subscribe to notifications & indications from the characteristic with
  // the given `handle`.
  //
  // Either notifications or indications will be enabled depending on
  // characteristic properties. Indications will be preferred if they are
  // supported. This operation fails if the characteristic does not have the
  // "notify" or "indicate" property.
  //
  // A write request will be issued to configure the characteristic for
  // notifications/indications if it contains a Client Characteristic
  // Configuration descriptor. This method fails if an error occurs while
  // writing to the descriptor.
  //
  // On success, `notification_callback` will be called when
  // the peer sends a notification or indication. Indications are
  // automatically confirmed.
  //
  // Subscriptions can be canceled with `StopNotifications`.
  //
  // Parameters:
  // `handle` - the handle of the characteristic to subscribe to.
  // `notification_callback` - will be called with the values of
  //     notifications/indications when received.
  // `result_callback` - called with the result of enabling
  //     notifications/indications.
  //
  // This may fail with the following errors:
  // `kFailure` - the characteristic does not support notifications or
  //     indications.
  // `kInvalidHandle` - `handle` is invalid.
  // `kWriteNotPermitted`or `kInsufficient*` - descriptor write error.
  void RegisterNotificationCallback(
      Handle handle,
      NotificationCallback&& notification_callback,
      Function<void(Result<Error>)>&& result_callback);

  // Stops notifications for the characteristic with the given `handle`.
  void StopNotifications(Handle handle);

 private:
  // Disconnect from the remote service. This method is called by the
  // ~RemoteService::Ptr() when it goes out of scope, the API client should
  // never call this method.
  void Disconnect();

 public:
  // Movable RemoteService smart pointer. The remote server will remain
  // connected until the returned RemoteService::Ptr is destroyed.
  using Ptr = internal::RaiiPtr<RemoteService, &RemoteService::Disconnect>;
};

// Represents a GATT client that interacts with services on a GATT server.
class Client {
 public:
  // Represents a remote GATT service.
  struct RemoteServiceInfo {
    // Uniquely identifies this GATT service.
    Handle handle;

    // Indicates whether this is a primary or secondary service.
    bool primary;

    // The UUID that identifies the type of this service.
    // There may be multiple services with the same UUID.
    Uuid type;
  };

  virtual ~Client() = default;

  // Enumerates existing services found on the peer that this Client represents,
  // and provides a stream of updates thereafter. Results can be filtered by
  // specifying a list of UUIDs in `uuids`. To further interact with services,
  // clients must obtain a RemoteService protocol by calling ConnectToService().
  // `uuid_allowlist` - The allowlist of UUIDs to filter services with.
  // `updated_callback` - Will be called with services that are
  //     updated/modified.
  // `removed_callback` - Called with the handles of services
  //     that have been removed. Note that handles may be reused.
  virtual void WatchServices(
      Vector<Uuid> uuid_allowlist,
      Function<void(RemoteServiceInfo)>&& updated_callback,
      Function<void(Handle)>&& removed_callback) = 0;

  // Stops service watching if started by `WatchServices`.
  virtual void StopWatchingServices();

  // Connects to a RemoteService. Only 1 connection per service is allowed.
  // `handle` - the handle of the service to connect to.
  //
  // This may fail with the following errors:
  // kInvalidParameters - `handle` does not correspond to a known service.
  virtual Result<Error, RemoteService::Ptr> ConnectToService(Handle handle) = 0;
};

}  // namespace pw::bluetooth::gatt
