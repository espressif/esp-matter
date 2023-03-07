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

#include "pw_rpc/internal/call.h"

#include "pw_assert/check.h"
#include "pw_rpc/client.h"
#include "pw_rpc/internal/endpoint.h"
#include "pw_rpc/internal/method.h"
#include "pw_rpc/server.h"

namespace pw::rpc::internal {

using pwpb::PacketType;

// Creates an active server-side Call.
Call::Call(const LockedCallContext& context, MethodType type)
    : Call(context.server().ClaimLocked(),
           context.call_id(),
           context.channel_id(),
           UnwrapServiceId(context.service().service_id()),
           context.method().id(),
           type,
           kServerCall) {}

// Creates an active client-side call, assigning it a new ID.
Call::Call(LockedEndpoint& client,
           uint32_t channel_id,
           uint32_t service_id,
           uint32_t method_id,
           MethodType type)
    : Call(client,
           client.NewCallId(),
           channel_id,
           service_id,
           method_id,
           type,
           kClientCall) {}

Call::Call(LockedEndpoint& endpoint_ref,
           uint32_t call_id,
           uint32_t channel_id,
           uint32_t service_id,
           uint32_t method_id,
           MethodType type,
           CallType call_type)
    : endpoint_(&endpoint_ref),
      channel_id_(channel_id),
      id_(call_id),
      service_id_(service_id),
      method_id_(method_id),
      rpc_state_(kActive),
      type_(type),
      call_type_(call_type),
      client_stream_state_(HasClientStream(type) ? kClientStreamActive
                                                 : kClientStreamInactive) {
  endpoint().RegisterCall(*this);
}

Call::~Call() {
  // Note: this explicit deregistration is necessary to ensure that
  // modifications to the endpoint call list occur while holding rpc_lock.
  // Removing this explicit registration would result in unsynchronized
  // modification of the endpoint call list via the destructor of the
  // superclass `IntrusiveList<Call>::Item`.
  LockGuard lock(rpc_lock());

  // This `active_locked()` guard is necessary to ensure that `endpoint()` is
  // still valid.
  if (active_locked()) {
    endpoint().UnregisterCall(*this);
  }
}

void Call::MoveFrom(Call& other) {
  PW_DCHECK(!active_locked());

  if (!other.active_locked()) {
    return;  // Nothing else to do; this call is already closed.
  }

  // Copy all members from the other call.
  endpoint_ = other.endpoint_;
  channel_id_ = other.channel_id_;
  id_ = other.id_;
  service_id_ = other.service_id_;
  method_id_ = other.method_id_;

  rpc_state_ = other.rpc_state_;
  type_ = other.type_;
  call_type_ = other.call_type_;
  client_stream_state_ = other.client_stream_state_;

  on_error_ = std::move(other.on_error_);
  on_next_ = std::move(other.on_next_);

  // Mark the other call inactive, unregister it, and register this one.
  other.rpc_state_ = kInactive;
  other.client_stream_state_ = kClientStreamInactive;

  endpoint().UnregisterCall(other);
  endpoint().RegisterUniqueCall(*this);
}

Status Call::SendPacket(PacketType type, ConstByteSpan payload, Status status) {
  if (!active_locked()) {
    return Status::FailedPrecondition();
  }

  Channel* channel = endpoint_->GetInternalChannel(channel_id_);
  if (channel == nullptr) {
    return Status::Unavailable();
  }
  return channel->Send(MakePacket(type, payload, status));
}

Status Call::CloseAndSendFinalPacketLocked(PacketType type,
                                           ConstByteSpan response,
                                           Status status) {
  const Status send_status = SendPacket(type, response, status);
  UnregisterAndMarkClosed();
  return send_status;
}

Status Call::WriteLocked(ConstByteSpan payload) {
  return SendPacket(call_type_ == kServerCall ? PacketType::SERVER_STREAM
                                              : PacketType::CLIENT_STREAM,
                    payload);
}

void Call::UnregisterAndMarkClosed() {
  if (active_locked()) {
    endpoint().UnregisterCall(*this);
    MarkClosed();
  }
}

}  // namespace pw::rpc::internal
