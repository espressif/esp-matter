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

#include <cstdint>

#include "pw_bytes/span.h"
#include "pw_function/function.h"
#include "pw_rpc/internal/call.h"
#include "pw_rpc/internal/endpoint.h"
#include "pw_rpc/internal/lock.h"

namespace pw::rpc::internal {

// A Call object, as used by an RPC client.
class ClientCall : public Call {
 public:
  ~ClientCall() PW_LOCKS_EXCLUDED(rpc_lock()) {
    rpc_lock().lock();
    CloseClientCall();
    rpc_lock().unlock();
  }

  uint32_t id() const PW_LOCKS_EXCLUDED(rpc_lock()) {
    LockGuard lock(rpc_lock());
    return Call::id();
  }

 protected:
  constexpr ClientCall() = default;

  ClientCall(LockedEndpoint& client,
             uint32_t channel_id,
             uint32_t service_id,
             uint32_t method_id,
             MethodType type) PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock())
      : Call(client, channel_id, service_id, method_id, type) {}

  // Sends CLIENT_STREAM_END if applicable, releases any held payload buffer,
  // and marks the call as closed.
  void CloseClientCall() PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock());

  void MoveClientCallFrom(ClientCall& other)
      PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock()) {
    CloseClientCall();
    MoveFrom(other);
  }
};

// Unary response client calls receive both a payload and the status in their
// on_completed callback. The on_next callback is not used.
class UnaryResponseClientCall : public ClientCall {
 public:
  template <typename CallType>
  static CallType Start(Endpoint& client,
                        uint32_t channel_id,
                        uint32_t service_id,
                        uint32_t method_id,
                        Function<void(ConstByteSpan, Status)>&& on_completed,
                        Function<void(Status)>&& on_error,
                        ConstByteSpan request) PW_LOCKS_EXCLUDED(rpc_lock()) {
    rpc_lock().lock();
    CallType call(client.ClaimLocked(), channel_id, service_id, method_id);
    call.set_on_completed_locked(std::move(on_completed));
    call.set_on_error_locked(std::move(on_error));

    call.SendInitialClientRequest(request);
    return call;
  }

  void HandleCompleted(ConstByteSpan response, Status status)
      PW_UNLOCK_FUNCTION(rpc_lock()) {
    const bool invoke_callback = on_completed_ != nullptr;
    UnregisterAndMarkClosed();

    rpc_lock().unlock();
    if (invoke_callback) {
      on_completed_(response, status);
    }
  }

 protected:
  constexpr UnaryResponseClientCall() = default;

  UnaryResponseClientCall(LockedEndpoint& client,
                          uint32_t channel_id,
                          uint32_t service_id,
                          uint32_t method_id,
                          MethodType type)
      PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock())
      : ClientCall(client, channel_id, service_id, method_id, type) {}

  UnaryResponseClientCall(UnaryResponseClientCall&& other) {
    *this = std::move(other);
  }

  UnaryResponseClientCall& operator=(UnaryResponseClientCall&& other)
      PW_LOCKS_EXCLUDED(rpc_lock()) {
    LockGuard lock(rpc_lock());
    MoveUnaryResponseClientCallFrom(other);
    return *this;
  }

  void MoveUnaryResponseClientCallFrom(UnaryResponseClientCall& other)
      PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock()) {
    MoveClientCallFrom(other);
    on_completed_ = std::move(other.on_completed_);
  }

  void set_on_completed(Function<void(ConstByteSpan, Status)>&& on_completed)
      PW_LOCKS_EXCLUDED(rpc_lock()) {
    // TODO(b/234876851): Ensure on_completed_ is properly guarded.
    LockGuard lock(rpc_lock());
    set_on_completed_locked(std::move(on_completed));
  }

  void set_on_completed_locked(
      Function<void(ConstByteSpan, Status)>&& on_completed)
      PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock()) {
    on_completed_ = std::move(on_completed);
  }

 private:
  using internal::ClientCall::set_on_next;  // Not used in unary response calls.

  Function<void(ConstByteSpan, Status)> on_completed_;
};

// Stream response client calls only receive the status in their on_completed
// callback. Payloads are sent through the on_next callback.
class StreamResponseClientCall : public ClientCall {
 public:
  template <typename CallType>
  static CallType Start(Endpoint& client,
                        uint32_t channel_id,
                        uint32_t service_id,
                        uint32_t method_id,
                        Function<void(ConstByteSpan)>&& on_next,
                        Function<void(Status)>&& on_completed,
                        Function<void(Status)>&& on_error,
                        ConstByteSpan request) {
    rpc_lock().lock();
    CallType call(client.ClaimLocked(), channel_id, service_id, method_id);

    call.set_on_next_locked(std::move(on_next));
    call.set_on_completed_locked(std::move(on_completed));
    call.set_on_error_locked(std::move(on_error));

    call.SendInitialClientRequest(request);
    return call;
  }

  void HandleCompleted(Status status) PW_UNLOCK_FUNCTION(rpc_lock()) {
    const bool invoke_callback = on_completed_ != nullptr;

    UnregisterAndMarkClosed();
    rpc_lock().unlock();

    // TODO(b/234876851): Ensure on_completed_ is properly guarded.
    if (invoke_callback) {
      on_completed_(status);
    }
  }

 protected:
  constexpr StreamResponseClientCall() = default;

  StreamResponseClientCall(LockedEndpoint& client,
                           uint32_t channel_id,
                           uint32_t service_id,
                           uint32_t method_id,
                           MethodType type)
      PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock())
      : ClientCall(client, channel_id, service_id, method_id, type) {}

  StreamResponseClientCall(StreamResponseClientCall&& other) {
    *this = std::move(other);
  }

  StreamResponseClientCall& operator=(StreamResponseClientCall&& other)
      PW_LOCKS_EXCLUDED(rpc_lock()) {
    LockGuard lock(rpc_lock());
    MoveStreamResponseClientCallFrom(other);
    return *this;
  }

  void MoveStreamResponseClientCallFrom(StreamResponseClientCall& other)
      PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock()) {
    MoveClientCallFrom(other);
    on_completed_ = std::move(other.on_completed_);
  }

  void set_on_completed(Function<void(Status)>&& on_completed)
      PW_LOCKS_EXCLUDED(rpc_lock()) {
    // TODO(b/234876851): Ensure on_completed_ is properly guarded.
    LockGuard lock(rpc_lock());
    set_on_completed_locked(std::move(on_completed));
  }

  void set_on_completed_locked(Function<void(Status)>&& on_completed)
      PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock()) {
    on_completed_ = std::move(on_completed);
  }

 private:
  Function<void(Status)> on_completed_;
};

}  // namespace pw::rpc::internal
