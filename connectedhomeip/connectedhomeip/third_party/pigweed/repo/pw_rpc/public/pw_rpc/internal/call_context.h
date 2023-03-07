// Copyright 2020 The Pigweed Authors
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

#include <cstddef>
#include <cstdint>

#include "pw_rpc/internal/channel.h"
#include "pw_rpc/internal/lock.h"

namespace pw::rpc {

class Service;

namespace internal {

class Endpoint;
class LockedCallContext;
class LockedEndpoint;
class Method;

// The Server creates a CallContext object to represent a method invocation. The
// CallContext is used to initialize a call object for the RPC.
class CallContext {
 public:
  constexpr CallContext(Endpoint& server,
                        uint32_t channel_id,
                        Service& service,
                        const internal::Method& method,
                        uint32_t call_id)
      : server_(server),
        channel_id_(channel_id),
        service_(service),
        method_(method),
        call_id_(call_id) {}

  // Claims that `rpc_lock()` is held, returning a wrapped context.
  //
  // This function should only be called in contexts in which it is clear that
  // `rpc_lock()` is held. When calling this function from a constructor, the
  // lock annotation will not result in errors, so care should be taken to
  // ensure that `rpc_lock()` is held.
  const LockedCallContext& ClaimLocked() const
      PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock());

  LockedCallContext& ClaimLocked() PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock());

  constexpr Endpoint& server() const { return server_; }

  constexpr const uint32_t& channel_id() const { return channel_id_; }

  constexpr Service& service() const { return service_; }

  constexpr const internal::Method& method() const { return method_; }

  constexpr const uint32_t& call_id() const { return call_id_; }

  // For testing use only
  void set_channel_id(uint32_t channel_id) { channel_id_ = channel_id; }

 private:
  Endpoint& server_;
  uint32_t channel_id_;
  Service& service_;
  const internal::Method& method_;
  uint32_t call_id_;
};

// A `CallContext` indicating that `rpc_lock()` is held.
//
// This is used as a constructor argument to supplement
// `PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock())`. Current compilers do not enforce
// lock annotations on constructors; no warnings or errors are produced when
// calling an annotated constructor without holding `rpc_lock()`.
class LockedCallContext : public CallContext {
 public:
  friend class CallContext;
  // No public constructor: this is created only via the `ClaimLocked` method on
  // `CallContext`.
  constexpr LockedCallContext() = delete;
};

inline const LockedCallContext& CallContext::ClaimLocked() const {
  return *static_cast<const LockedCallContext*>(this);
}

inline LockedCallContext& CallContext::ClaimLocked() {
  return *static_cast<LockedCallContext*>(this);
}

}  // namespace internal
}  // namespace pw::rpc
