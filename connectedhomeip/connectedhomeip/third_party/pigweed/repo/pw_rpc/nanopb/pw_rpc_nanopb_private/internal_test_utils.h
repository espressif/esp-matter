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

#include "pb_decode.h"
#include "pb_encode.h"
#include "pw_span/span.h"

namespace pw::rpc::internal {

// Encodes a protobuf to a local span named by result from a list of nanopb
// struct initializers.
//
//  PW_ENCODE_PB(pw_rpc_TestProto, encoded, .value = 42);
//
#define PW_ENCODE_PB(proto, result, ...) \
  _PW_ENCODE_PB_EXPAND(proto, result, __LINE__, __VA_ARGS__)

#define _PW_ENCODE_PB_EXPAND(proto, result, unique, ...) \
  _PW_ENCODE_PB_IMPL(proto, result, unique, __VA_ARGS__)

#define _PW_ENCODE_PB_IMPL(proto, result, unique, ...)            \
  std::array<pb_byte_t, 2 * sizeof(proto)> _pb_buffer_##unique{}; \
  const span result =                                             \
      ::pw::rpc::internal::EncodeProtobuf<proto, proto##_fields>( \
          proto{__VA_ARGS__}, _pb_buffer_##unique)

template <typename T, auto kFields>
span<const std::byte> EncodeProtobuf(const T& protobuf,
                                     span<pb_byte_t> buffer) {
  auto output = pb_ostream_from_buffer(buffer.data(), buffer.size());
  EXPECT_TRUE(pb_encode(&output, kFields, &protobuf));
  return as_bytes(buffer.first(output.bytes_written));
}

// Decodes a protobuf to a nanopb struct named by result.
#define PW_DECODE_PB(proto, result, buffer)                                   \
  proto result;                                                               \
  ::pw::rpc::internal::DecodeProtobuf<proto, proto##_fields>(                 \
      span(reinterpret_cast<const pb_byte_t*>(buffer.data()), buffer.size()), \
      result);

template <typename T, auto kFields>
void DecodeProtobuf(span<const pb_byte_t> buffer, T& protobuf) {
  auto input = pb_istream_from_buffer(buffer.data(), buffer.size());
  EXPECT_TRUE(pb_decode(&input, kFields, &protobuf));
}

}  // namespace pw::rpc::internal
