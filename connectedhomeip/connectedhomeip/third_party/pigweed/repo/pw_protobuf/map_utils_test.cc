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

#include "pw_protobuf/map_utils.h"

#include <string_view>

#include "gtest/gtest.h"
#include "pw_stream/memory_stream.h"
#include "pw_stream/stream.h"

#define ASSERT_OK(status) ASSERT_EQ(OkStatus(), status)

namespace pw::protobuf {

TEST(ProtoHelper, WriteProtoStringToBytesMapEntry) {
  // The following defines an instance of the message below:
  //
  // message Maps {
  //   map<string, string> map_a = 1;
  //   map<string, string> map_b = 2;
  // }
  //
  // where
  //
  // Maps.map_a['key_foo'] = 'foo_a'
  // Maps.map_a['key_bar'] = 'bar_a'
  //
  // Maps.map_b['key_foo'] = 'foo_b'
  // Maps.map_b['key_bar'] = 'bar_b'
  //
  // clang-format off
  std::uint8_t encoded_proto[] = {
    // map_a["key_bar"] = "bar_a", key = 1
    0x0a, 0x10,
    0x0a, 0x07, 'k', 'e', 'y', '_', 'b', 'a', 'r', // map key
    0x12, 0x05, 'b', 'a', 'r', '_', 'a', // map value

    // map_a["key_foo"] = "foo_a", key = 1
    0x0a, 0x10,
    0x0a, 0x07, 'k', 'e', 'y', '_', 'f', 'o', 'o',
    0x12, 0x05, 'f', 'o', 'o', '_', 'a',

    // map_b["key_foo"] = "foo_b", key = 2
    0x12, 0x10,
    0x0a, 0x07, 'k', 'e', 'y', '_', 'f', 'o', 'o',
    0x12, 0x05, 'f', 'o', 'o', '_', 'b',

    // map_b["key_bar"] = "bar_b", key = 2
    0x12, 0x10,
    0x0a, 0x07, 'k', 'e', 'y', '_', 'b', 'a', 'r',
    0x12, 0x05, 'b', 'a', 'r', '_', 'b',
  };
  // clang-format on

  // Now construct the same message with WriteStringToBytesMapEntry
  std::byte dst_buffer[sizeof(encoded_proto)];
  stream::MemoryWriter writer(dst_buffer);

  const struct {
    uint32_t field_number;
    std::string_view key;
    std::string_view value;
  } kMapData[] = {
      {1, "key_bar", "bar_a"},
      {1, "key_foo", "foo_a"},
      {2, "key_foo", "foo_b"},
      {2, "key_bar", "bar_b"},
  };

  std::byte stream_pipe_buffer[1];
  for (auto ele : kMapData) {
    stream::MemoryReader key_reader(as_bytes(span<const char>{ele.key}));
    stream::MemoryReader value_reader(as_bytes(span<const char>{ele.value}));
    ASSERT_OK(WriteProtoStringToBytesMapEntry(ele.field_number,
                                              key_reader,
                                              ele.key.size(),
                                              value_reader,
                                              ele.value.size(),
                                              stream_pipe_buffer,
                                              writer));
  }

  ASSERT_EQ(memcmp(dst_buffer, encoded_proto, sizeof(dst_buffer)), 0);
}

TEST(ProtoHelper, WriteProtoStringToBytesMapEntryExceedsWriteLimit) {
  // Construct an instance of the message below:
  //
  // message Maps {
  //   map<string, string> map_a = 1;
  // }
  //
  // where
  //
  // Maps.map_a['key_bar'] = 'bar_a'. The needed buffer size is 18 in this
  // case:
  //
  // {
  //   0x0a, 0x10,
  //   0x0a, 0x07, 'k', 'e', 'y', '_', 'b', 'a', 'r',
  //   0x12, 0x05, 'b', 'a', 'r', '_', 'a',
  // }
  //
  // Use a smaller buffer.
  std::byte encode_buffer[17];
  stream::MemoryWriter writer(encode_buffer);
  constexpr uint32_t kFieldNumber = 1;
  std::string_view key = "key_bar";
  std::string_view value = "bar_a";
  stream::MemoryReader key_reader(as_bytes(span<const char>{key}));
  stream::MemoryReader value_reader(as_bytes(span<const char>{value}));
  std::byte stream_pipe_buffer[1];
  ASSERT_EQ(
      WriteProtoStringToBytesMapEntry(kFieldNumber,
                                      key_reader,
                                      key_reader.ConservativeReadLimit(),
                                      value_reader,
                                      value_reader.ConservativeReadLimit(),
                                      stream_pipe_buffer,
                                      writer),
      Status::ResourceExhausted());
}

TEST(ProtoHelper, WriteProtoStringToBytesMapEntryInvalidArgument) {
  std::byte encode_buffer[17];
  stream::MemoryWriter writer(encode_buffer);
  std::string_view key = "key_bar";
  std::string_view value = "bar_a";
  stream::MemoryReader key_reader(as_bytes(span<const char>{key}));
  stream::MemoryReader value_reader(as_bytes(span<const char>{value}));
  std::byte stream_pipe_buffer[1];

  ASSERT_EQ(
      WriteProtoStringToBytesMapEntry(19091,
                                      key_reader,
                                      key_reader.ConservativeReadLimit(),
                                      value_reader,
                                      value_reader.ConservativeReadLimit(),
                                      stream_pipe_buffer,
                                      writer),
      Status::InvalidArgument());
}

}  // namespace pw::protobuf
