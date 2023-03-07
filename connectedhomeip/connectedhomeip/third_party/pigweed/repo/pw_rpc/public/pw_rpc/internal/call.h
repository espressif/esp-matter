// Copyright 2021 The Pigweed Authors
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

#include <cassert>
#include <cstddef>
#include <limits>
#include <utility>

#include "pw_containers/intrusive_list.h"
#include "pw_function/function.h"
#include "pw_rpc/internal/call_context.h"
#include "pw_rpc/internal/channel.h"
#include "pw_rpc/internal/lock.h"
#include "pw_rpc/internal/method.h"
#include "pw_rpc/internal/packet.h"
#include "pw_rpc/method_type.h"
#include "pw_rpc/service.h"
#include "pw_span/span.h"
#include "pw_status/status.h"
#include "pw_sync/lock_annotations.h"

namespace pw::rpc {

class Writer;

namespace internal {

class Endpoint;
class LockedEndpoint;
class Packet;

// Unrequested RPCs always use this call ID. When a subsequent request
// or response is sent with a matching channel + service + method,
// it will match a calls with this ID if one exists.
constexpr uint32_t kOpenCallId = std::numeric_limits<uint32_t>::max();

// Internal RPC Call class. The Call is used to respond to any type of RPC.
// Public classes like ServerWriters inherit from it with private inheritance
// and provide a public API for their use case. The Call's public API is used by
// the Server and Client classes.
//
// Private inheritance is used in place of composition or more complex
// inheritance hierarchy so that these objects all inherit from a common
// IntrusiveList::Item object. Private inheritance also gives the derived classs
// full control over their interfaces.
class Call : public IntrusiveList<Call>::Item {
 public:
  Call(const Call&) = delete;

  // Move support is provided to derived classes through the MoveFrom function.
  Call(Call&&) = delete;

  Call& operator=(const Call&) = delete;
  Call& operator=(Call&&) = delete;

  ~Call() PW_LOCKS_EXCLUDED(rpc_lock());

  // True if the Call is active and ready to send responses.
  [[nodiscard]] bool active() const PW_LOCKS_EXCLUDED(rpc_lock()) {
    LockGuard lock(rpc_lock());
    return active_locked();
  }

  [[nodiscard]] bool active_locked() const
      PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock()) {
    return rpc_state_ == kActive;
  }

  uint32_t id() const PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock()) { return id_; }

  void set_id(uint32_t id) PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock()) { id_ = id; }

  uint32_t channel_id() const PW_LOCKS_EXCLUDED(rpc_lock()) {
    LockGuard lock(rpc_lock());
    return channel_id_locked();
  }
  uint32_t channel_id_locked() const PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock()) {
    return channel_id_;
  }
  uint32_t service_id() const PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock()) {
    return service_id_;
  }
  uint32_t method_id() const PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock()) {
    return method_id_;
  }

  // Closes the Call and sends a RESPONSE packet, if it is active. Returns the
  // status from sending the packet, or FAILED_PRECONDITION if the Call is not
  // active.
  Status CloseAndSendResponse(ConstByteSpan response, Status status)
      PW_LOCKS_EXCLUDED(rpc_lock()) {
    LockGuard lock(rpc_lock());
    return CloseAndSendResponseLocked(response, status);
  }

  Status CloseAndSendResponseLocked(ConstByteSpan response, Status status)
      PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock()) {
    return CloseAndSendFinalPacketLocked(
        pwpb::PacketType::RESPONSE, response, status);
  }

  Status CloseAndSendResponse(Status status) PW_LOCKS_EXCLUDED(rpc_lock()) {
    return CloseAndSendResponse({}, status);
  }

  Status CloseAndSendServerErrorLocked(Status error)
      PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock()) {
    return CloseAndSendFinalPacketLocked(
        pwpb::PacketType::SERVER_ERROR, {}, error);
  }

  // Public call that ends the client stream for a client call.
  Status CloseClientStream() PW_LOCKS_EXCLUDED(rpc_lock()) {
    LockGuard lock(rpc_lock());
    return CloseClientStreamLocked();
  }

  // Internal call that closes the client stream.
  Status CloseClientStreamLocked() PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock()) {
    client_stream_state_ = kClientStreamInactive;
    return SendPacket(pwpb::PacketType::CLIENT_STREAM_END, {}, {});
  }

  // Sends a payload in either a server or client stream packet.
  Status Write(ConstByteSpan payload) PW_LOCKS_EXCLUDED(rpc_lock()) {
    LockGuard lock(rpc_lock());
    return WriteLocked(payload);
  }

  Status WriteLocked(ConstByteSpan payload)
      PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock());

  // Sends the initial request for a client call. If the request fails, the call
  // is closed.
  void SendInitialClientRequest(ConstByteSpan payload)
      PW_UNLOCK_FUNCTION(rpc_lock()) {
    // TODO(b/234876851): Ensure the call object is locked before releasing the
    //     RPC mutex.
    if (const Status status = SendPacket(pwpb::PacketType::REQUEST, payload);
        !status.ok()) {
      HandleError(status);
    } else {
      rpc_lock().unlock();
    }
  }

  // Whenever a payload arrives (in a server/client stream or in a response),
  // call the on_next_ callback.
  // Precondition: rpc_lock() must be held.
  void HandlePayload(ConstByteSpan message) const
      PW_UNLOCK_FUNCTION(rpc_lock()) {
    const bool invoke = on_next_ != nullptr;
    // TODO(b/234876851): Ensure on_next_ is properly guarded.
    rpc_lock().unlock();

    if (invoke) {
      on_next_(message);
    }
  }

  // Handles an error condition for the call. This closes the call and calls the
  // on_error callback, if set.
  void HandleError(Status status) PW_UNLOCK_FUNCTION(rpc_lock()) {
    UnregisterAndMarkClosed();
    CallOnError(status);
  }

  // Aborts the RPC because of a change in the endpoint (e.g. channel closed,
  // service unregistered). Does NOT unregister the call! The calls must be
  // removed when iterating over the list in the endpoint.
  void Abort() PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock()) {
    // Locking here is problematic because CallOnError releases rpc_lock().
    //
    // b/234876851 must be addressed before the locking here can be cleaned up.
    MarkClosed();

    CallOnError(Status::Aborted());

    // Re-lock rpc_lock().
    rpc_lock().lock();
  }

  bool has_client_stream() const PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock()) {
    return HasClientStream(type_);
  }

  bool has_server_stream() const PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock()) {
    return HasServerStream(type_);
  }

  bool client_stream_open() const PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock()) {
    return client_stream_state_ == kClientStreamActive;
  }

  // Keep this public so the Nanopb implementation can set it from a helper
  // function.
  void set_on_next(Function<void(ConstByteSpan)>&& on_next)
      PW_LOCKS_EXCLUDED(rpc_lock()) {
    LockGuard lock(rpc_lock());
    set_on_next_locked(std::move(on_next));
  }

 protected:
  // Creates an inactive Call.
  constexpr Call()
      : endpoint_{},
        channel_id_{},
        id_{},
        service_id_{},
        method_id_{},
        rpc_state_{},
        type_{},
        call_type_{},
        client_stream_state_{} {}

  // Creates an active server-side Call.
  Call(const LockedCallContext& context, MethodType type)
      PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock());

  // Creates an active client-side Call.
  Call(LockedEndpoint& client,
       uint32_t channel_id,
       uint32_t service_id,
       uint32_t method_id,
       MethodType type) PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock());

  // This call must be in a closed state when this is called.
  void MoveFrom(Call& other) PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock());

  Endpoint& endpoint() const PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock()) {
    return *endpoint_;
  }

  void set_on_next_locked(Function<void(ConstByteSpan)>&& on_next)
      PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock()) {
    on_next_ = std::move(on_next);
  }

  void set_on_error(Function<void(Status)>&& on_error)
      PW_LOCKS_EXCLUDED(rpc_lock()) {
    LockGuard lock(rpc_lock());
    set_on_error_locked(std::move(on_error));
  }

  void set_on_error_locked(Function<void(Status)>&& on_error)
      PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock()) {
    on_error_ = std::move(on_error);
  }

  // Calls the on_error callback without closing the RPC. This is used when the
  // call has already completed.
  void CallOnError(Status error) PW_UNLOCK_FUNCTION(rpc_lock()) {
    const bool invoke = on_error_ != nullptr;

    // TODO(b/234876851): Ensure on_error_ is properly guarded.

    rpc_lock().unlock();
    if (invoke) {
      on_error_(error);
    }
  }

  void MarkClientStreamCompleted() PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock()) {
    client_stream_state_ = kClientStreamInactive;
  }

  Status CloseAndSendResponseLocked(Status status)
      PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock()) {
    return CloseAndSendFinalPacketLocked(
        pwpb::PacketType::RESPONSE, {}, status);
  }

  // Cancels an RPC. For client calls only.
  Status Cancel() PW_LOCKS_EXCLUDED(rpc_lock()) {
    LockGuard lock(rpc_lock());
    return CloseAndSendFinalPacketLocked(
        pwpb::PacketType::CLIENT_ERROR, {}, Status::Cancelled());
  }

  // Unregisters the RPC from the endpoint & marks as closed. The call may be
  // active or inactive when this is called.
  void UnregisterAndMarkClosed() PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock());

  // Define conversions to the generic server/client RPC writer class. These
  // functions are defined in pw_rpc/writer.h after the Writer class is defined.
  constexpr operator Writer&();
  constexpr operator const Writer&() const;

 private:
  enum CallType : bool { kServerCall, kClientCall };

  // Common constructor for server & client calls.
  Call(LockedEndpoint& endpoint,
       uint32_t id,
       uint32_t channel_id,
       uint32_t service_id,
       uint32_t method_id,
       MethodType type,
       CallType call_type);

  Packet MakePacket(pwpb::PacketType type,
                    ConstByteSpan payload,
                    Status status = OkStatus()) const
      PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock()) {
    return Packet(type,
                  channel_id_locked(),
                  service_id(),
                  method_id(),
                  id_,
                  payload,
                  status);
  }

  void MarkClosed() PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock()) {
    channel_id_ = Channel::kUnassignedChannelId;
    rpc_state_ = kInactive;
    client_stream_state_ = kClientStreamInactive;
  }

  // Sends a payload with the specified type. The payload may either be in a
  // previously acquired buffer or in a standalone buffer.
  //
  // Returns FAILED_PRECONDITION if the call is not active().
  Status SendPacket(pwpb::PacketType type,
                    ConstByteSpan payload,
                    Status status = OkStatus())
      PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock());

  Status CloseAndSendFinalPacketLocked(pwpb::PacketType type,
                                       ConstByteSpan response,
                                       Status status)
      PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock());

  internal::Endpoint* endpoint_ PW_GUARDED_BY(rpc_lock());
  uint32_t channel_id_ PW_GUARDED_BY(rpc_lock());
  uint32_t id_ PW_GUARDED_BY(rpc_lock());
  uint32_t service_id_ PW_GUARDED_BY(rpc_lock());
  uint32_t method_id_ PW_GUARDED_BY(rpc_lock());

  enum : bool { kInactive, kActive } rpc_state_ PW_GUARDED_BY(rpc_lock());
  MethodType type_ PW_GUARDED_BY(rpc_lock());
  CallType call_type_ PW_GUARDED_BY(rpc_lock());
  enum : bool {
    kClientStreamInactive,
    kClientStreamActive,
  } client_stream_state_ PW_GUARDED_BY(rpc_lock());

  // Called when the RPC is terminated due to an error.
  Function<void(Status error)> on_error_;

  // Called when a request is received. Only used for RPCs with client streams.
  // The raw payload buffer is passed to the callback.
  Function<void(ConstByteSpan payload)> on_next_;
};

}  // namespace internal
}  // namespace pw::rpc
