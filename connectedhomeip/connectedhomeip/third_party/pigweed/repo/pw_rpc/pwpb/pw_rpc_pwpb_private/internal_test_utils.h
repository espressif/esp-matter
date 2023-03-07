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

#include <array>
#include <cstddef>

#include "pw_span/span.h"
#include "pw_status/status.h"
#include "pw_stream/memory_stream.h"

namespace pw::rpc::internal {

// Encodes a protobuf to a local span named by result from a list of pw_protobuf
// struct initializers.  Note that the proto namespace is passed, not the name
// of the struct --- ie. exclude the "::Message" suffix.
//
//  PW_ENCODE_PB(pw::rpc::TestProto, encoded, .value = 42);
//
#define PW_ENCODE_PB(proto, result, ...) \
  _PW_ENCODE_PB_EXPAND(proto, result, __LINE__, __VA_ARGS__)

#define _PW_ENCODE_PB_EXPAND(proto, result, unique, ...) \
  _PW_ENCODE_PB_IMPL(proto, result, unique, __VA_ARGS__)

#define _PW_ENCODE_PB_IMPL(proto, result, unique, ...)                     \
  std::array<std::byte, 2 * sizeof(proto::Message)> _pb_buffer_##unique{}; \
  const span result =                                                      \
      ::pw::rpc::internal::EncodeProtobuf<proto::Message,                  \
                                          proto::MemoryEncoder>(           \
          proto::Message{__VA_ARGS__}, _pb_buffer_##unique)

template <typename Message, typename MemoryEncoder>
span<const std::byte> EncodeProtobuf(const Message& message,
                                     span<std::byte> buffer) {
  MemoryEncoder encoder(buffer);
  EXPECT_EQ(encoder.Write(message), OkStatus());
  return buffer.first(encoder.size());
}

// Decodes a protobuf to a pw_protobuf struct named by result. Note that the
// proto namespace is passed, not the name of the struct --- ie. exclude the
// "::Message" suffix.
//
//  PW_DECODE_PB(pw::rpc::TestProto, decoded, buffer);
//
#define PW_DECODE_PB(proto, result, buffer)                                  \
  proto::Message result;                                                     \
  ::pw::rpc::internal::DecodeProtobuf<proto::Message, proto::StreamDecoder>( \
      buffer, result);

template <typename Message, typename StreamDecoder>
void DecodeProtobuf(span<const std::byte> buffer, Message& message) {
  stream::MemoryReader reader(buffer);
  EXPECT_EQ(StreamDecoder(reader).Read(message), OkStatus());
}

}  // namespace pw::rpc::internal
