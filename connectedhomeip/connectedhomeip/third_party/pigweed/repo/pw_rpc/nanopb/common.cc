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

// clang-format off
#include "pw_rpc/internal/log_config.h" // PW_LOG_* macros must be first.

#include "pw_rpc/nanopb/internal/common.h"
// clang-format on

#include "pb_decode.h"
#include "pb_encode.h"
#include "pw_assert/check.h"
#include "pw_log/log.h"
#include "pw_result/result.h"
#include "pw_rpc/internal/client_call.h"
#include "pw_rpc/nanopb/server_reader_writer.h"
#include "pw_status/try.h"

namespace pw::rpc::internal {
namespace {

// Nanopb 3 uses pb_field_s and Nanopb 4 uses pb_msgdesc_s for fields. The
// Nanopb version macro is difficult to use, so deduce the correct type from the
// pb_decode function.
template <typename DecodeFunction>
struct NanopbTraits;

template <typename FieldsType>
struct NanopbTraits<bool(pb_istream_t*, FieldsType, void*)> {
  using Fields = FieldsType;
};

using Fields = typename NanopbTraits<decltype(pb_decode)>::Fields;

Result<ByteSpan> EncodeToPayloadBuffer(const void* payload, NanopbSerde serde)
    PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock()) {
  ByteSpan payload_buffer = GetPayloadBuffer();
  StatusWithSize result = serde.Encode(payload, payload_buffer);
  if (!result.ok()) {
    return result.status();
  }
  return payload_buffer.first(result.size());
}

}  // namespace

// PB_NO_ERRMSG is used in pb_decode.h and pb_encode.h to enable or disable the
// errmsg member of the istream and ostream structs. If errmsg is available, use
// it to give more detailed log messages.
#ifdef PB_NO_ERRMSG

#define PW_RPC_LOG_NANOPB_FAILURE(msg, stream) PW_LOG_ERROR(msg)

#else

#define PW_RPC_LOG_NANOPB_FAILURE(msg, stream) \
  PW_LOG_ERROR(msg ": %s", stream.errmsg)

#endif  // PB_NO_ERRMSG

StatusWithSize NanopbSerde::Encode(const void* proto_struct,
                                   ByteSpan buffer) const {
  auto output = pb_ostream_from_buffer(
      reinterpret_cast<pb_byte_t*>(buffer.data()), buffer.size());
  if (!pb_encode(&output, static_cast<Fields>(fields_), proto_struct)) {
    PW_RPC_LOG_NANOPB_FAILURE("Nanopb protobuf encode failed", output);
    return StatusWithSize::Internal();
  }
  return StatusWithSize(output.bytes_written);
}

StatusWithSize NanopbSerde::EncodedSizeBytes(const void* proto_struct) const {
  size_t encoded_size = 0;
  return pb_get_encoded_size(&encoded_size, fields_, proto_struct)
             ? StatusWithSize(encoded_size)
             : StatusWithSize::Unknown();
}

bool NanopbSerde::Decode(ConstByteSpan buffer, void* proto_struct) const {
  auto input = pb_istream_from_buffer(
      reinterpret_cast<const pb_byte_t*>(buffer.data()), buffer.size());
  bool result = pb_decode(&input, static_cast<Fields>(fields_), proto_struct);
  if (!result) {
    PW_RPC_LOG_NANOPB_FAILURE("Nanopb protobuf decode failed", input);
  }
  return result;
}

#undef PW_RPC_LOG_NANOPB_FAILURE

void NanopbSendInitialRequest(ClientCall& call,
                              NanopbSerde serde,
                              const void* payload) {
  PW_DCHECK(call.active_locked());

  Result<ByteSpan> result = EncodeToPayloadBuffer(payload, serde);

  if (result.ok()) {
    call.SendInitialClientRequest(*result);
  } else {
    call.HandleError(result.status());
  }
}

Status NanopbSendStream(Call& call, const void* payload, NanopbSerde serde) {
  LockGuard lock(rpc_lock());
  if (!call.active_locked()) {
    return Status::FailedPrecondition();
  }

  Result<ByteSpan> result = EncodeToPayloadBuffer(payload, serde);

  PW_TRY(result.status());
  return call.WriteLocked(*result);
}

Status SendFinalResponse(NanopbServerCall& call,
                         const void* payload,
                         const Status status) {
  LockGuard lock(rpc_lock());
  if (!call.active_locked()) {
    return Status::FailedPrecondition();
  }

  Result<ByteSpan> result =
      EncodeToPayloadBuffer(payload, call.serde().response());
  if (!result.ok()) {
    return call.CloseAndSendServerErrorLocked(Status::Internal());
  }
  return call.CloseAndSendResponseLocked(*result, status);
}

}  // namespace pw::rpc::internal
