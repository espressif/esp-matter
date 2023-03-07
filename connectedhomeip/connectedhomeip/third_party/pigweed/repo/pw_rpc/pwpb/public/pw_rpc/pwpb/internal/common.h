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

#include "pw_assert/assert.h"
#include "pw_bytes/span.h"
#include "pw_protobuf/encoder.h"
#include "pw_protobuf/internal/codegen.h"
#include "pw_protobuf/stream_decoder.h"
#include "pw_rpc/internal/client_call.h"
#include "pw_rpc/internal/server_call.h"
#include "pw_rpc/pwpb/serde.h"
#include "pw_span/span.h"
#include "pw_status/status.h"
#include "pw_status/status_with_size.h"

namespace pw::rpc::internal {

// Defines per-message struct type instance of the serializer/deserializer.
template <PwpbMessageDescriptor kRequest, PwpbMessageDescriptor kResponse>
constexpr PwpbMethodSerde kPwpbMethodSerde(kRequest, kResponse);

// Encodes a message struct into a payload buffer.
template <typename Payload>
Result<ByteSpan> PwpbEncodeToPayloadBuffer(const Payload& payload,
                                           PwpbSerde serde)
    PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock()) {
  ByteSpan buffer = GetPayloadBuffer();
  const StatusWithSize sws = serde.Encode(payload, buffer);
  if (!sws.ok()) {
    return sws.status();
  }
  return buffer.first(sws.size());
}

// [Client] Encodes and sends the initial request message for the call.
// active() must be true.
template <typename Request>
void PwpbSendInitialRequest(ClientCall& call,
                            PwpbSerde serde,
                            const Request& request)
    PW_UNLOCK_FUNCTION(rpc_lock()) {
  PW_ASSERT(call.active_locked());

  Result<ByteSpan> buffer = PwpbEncodeToPayloadBuffer(request, serde);
  if (buffer.ok()) {
    call.SendInitialClientRequest(*buffer);
  } else {
    call.HandleError(buffer.status());
  }
}

// [Client/Server] Encodes and sends a client or server stream message.
// active() must be true.
template <typename Payload>
Status PwpbSendStream(Call& call, const Payload& payload, PwpbSerde serde)
    PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock()) {
  Result<ByteSpan> buffer = PwpbEncodeToPayloadBuffer(payload, serde);
  PW_TRY(buffer);

  return call.WriteLocked(*buffer);
}

// [Server] Encodes and sends the final response message from an untyped
// ConstByteSpan.
// active() must be true.
template <typename Response>
Status PwpbSendFinalResponse(internal::ServerCall& call,
                             const Response& response,
                             Status status,
                             PwpbSerde serde)
    PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock()) {
  PW_ASSERT(call.active_locked());

  Result<ByteSpan> buffer = PwpbEncodeToPayloadBuffer(response, serde);
  if (!buffer.ok()) {
    return call.CloseAndSendServerErrorLocked(Status::Internal());
  }

  return call.CloseAndSendResponseLocked(*buffer, status);
}

}  // namespace pw::rpc::internal
