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

#include "pb_common.h"
#include "pw_bytes/span.h"
#include "pw_rpc/internal/lock.h"
#include "pw_status/status_with_size.h"

namespace pw::rpc::internal {

// Nanopb 0.3 uses pb_field_t, but Nanopb 4 uses pb_msgdesc_t. Determine which
// type to use by deducing it from the pb_field_iter_begin function.
template <typename PbFieldIterBeginFunction>
struct NanopbDescriptorTraits;

template <typename T>
struct NanopbDescriptorTraits<bool(pb_field_iter_t*, T, void*)> {
  using Type = T;
};

using NanopbMessageDescriptor =
    NanopbDescriptorTraits<decltype(pb_field_iter_begin)>::Type;

// Serializer/deserializer for a Nanopb protobuf message.
class NanopbSerde {
 public:
  explicit constexpr NanopbSerde(NanopbMessageDescriptor fields)
      : fields_(fields) {}

  NanopbSerde(const NanopbSerde&) = default;
  NanopbSerde& operator=(const NanopbSerde&) = default;

  // Encodes a Nanopb protobuf struct to the serialized wire format.
  StatusWithSize Encode(const void* proto_struct, ByteSpan buffer) const;

  // Calculates the encoded size of the provided protobuf struct without
  // actually encoding it.
  StatusWithSize EncodedSizeBytes(const void* proto_struct) const;

  // Decodes a serialized protobuf to a Nanopb struct.
  bool Decode(ConstByteSpan buffer, void* proto_struct) const;

 private:
  NanopbMessageDescriptor fields_;
};

// Serializer/deserializer for Nanopb message request and response structs in an
// RPC method.
class NanopbMethodSerde {
 public:
  constexpr NanopbMethodSerde(NanopbMessageDescriptor request_fields,
                              NanopbMessageDescriptor response_fields)
      : request_fields_(request_fields), response_fields_(response_fields) {}

  NanopbMethodSerde(const NanopbMethodSerde&) = delete;
  NanopbMethodSerde& operator=(const NanopbMethodSerde&) = delete;

  StatusWithSize EncodeRequest(const void* proto_struct,
                               ByteSpan buffer) const {
    return request_fields_.Encode(proto_struct, buffer);
  }
  StatusWithSize EncodeResponse(const void* proto_struct,
                                ByteSpan buffer) const {
    return response_fields_.Encode(proto_struct, buffer);
  }

  bool DecodeRequest(ConstByteSpan buffer, void* proto_struct) const {
    return request_fields_.Decode(buffer, proto_struct);
  }
  bool DecodeResponse(ConstByteSpan buffer, void* proto_struct) const {
    return response_fields_.Decode(buffer, proto_struct);
  }

  const NanopbSerde& request() const { return request_fields_; }
  const NanopbSerde& response() const { return response_fields_; }

 private:
  NanopbSerde request_fields_;
  NanopbSerde response_fields_;
};

template <NanopbMessageDescriptor kRequest, NanopbMessageDescriptor kResponse>
inline constexpr NanopbMethodSerde kNanopbMethodSerde(kRequest, kResponse);

class Call;
class ClientCall;
class NanopbServerCall;

// [Client] Encodes and sends the initial request message for the call.
// active() must be true.
void NanopbSendInitialRequest(ClientCall& call,
                              NanopbSerde serde,
                              const void* payload)
    PW_UNLOCK_FUNCTION(rpc_lock());

// [Client/Server] Encodes and sends a client or server stream message.
// active() must be true.
Status NanopbSendStream(Call& call, const void* payload, NanopbSerde serde)
    PW_LOCKS_EXCLUDED(rpc_lock());

// [Server] Encodes and sends the final response message.
// Returns Status::FailedPrecondition if active() is false.
Status SendFinalResponse(NanopbServerCall& call,
                         const void* payload,
                         Status status) PW_LOCKS_EXCLUDED(rpc_lock());

}  // namespace pw::rpc::internal
