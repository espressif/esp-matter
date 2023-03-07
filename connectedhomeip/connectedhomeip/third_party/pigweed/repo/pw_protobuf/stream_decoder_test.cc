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

#include "pw_protobuf/stream_decoder.h"

#include <array>

#include "gtest/gtest.h"
#include "pw_status/status.h"
#include "pw_status/status_with_size.h"
#include "pw_stream/memory_stream.h"
#include "pw_stream/stream.h"

namespace pw::protobuf {
namespace {
// Non-seekable wrapper for MemoryReader for testing behavior when seeking is
// not available.
class NonSeekableMemoryReader : public stream::NonSeekableReader {
 public:
  explicit NonSeekableMemoryReader(stream::MemoryReader& reader)
      : reader_(reader) {}

  size_t bytes_read() const { return reader_.bytes_read(); }
  const std::byte* data() const { return reader_.data(); }

 private:
  StatusWithSize DoRead(ByteSpan destination) override {
    const pw::Result<pw::ByteSpan> result = reader_.Read(destination);
    if (!result.ok()) {
      return StatusWithSize(result.status(), 0);
    }
    return StatusWithSize(result.value().size_bytes());
  }

  stream::MemoryReader& reader_;
};

TEST(StreamDecoder, Decode) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // type=int32, k=1, v=42
    0x08, 0x2a,
    // type=sint32, k=2, v=-13
    0x10, 0x19,
    // type=bool, k=3, v=false
    0x18, 0x00,
    // type=double, k=4, v=3.14159
    0x21, 0x6e, 0x86, 0x1b, 0xf0, 0xf9, 0x21, 0x09, 0x40,
    // type=fixed32, k=5, v=0xdeadbeef
    0x2d, 0xef, 0xbe, 0xad, 0xde,
    // type=string, k=6, v="Hello world"
    0x32, 0x0b, 'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd',
    // type=sfixed32, k=7, v=-50
    0x3d, 0xce, 0xff, 0xff, 0xff,
    // type=sfixed64, k=8, v=-1647993274
    0x41, 0x46, 0x9e, 0xc5, 0x9d, 0xff, 0xff, 0xff, 0xff,
    // type=float, k=9, v=2.718
    0x4d, 0xb6, 0xf3, 0x2d, 0x40,
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 1u);
  Result<int32_t> int32 = decoder.ReadInt32();
  ASSERT_EQ(int32.status(), OkStatus());
  EXPECT_EQ(int32.value(), 42);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 2u);
  Result<int32_t> sint32 = decoder.ReadSint32();
  ASSERT_EQ(sint32.status(), OkStatus());
  EXPECT_EQ(sint32.value(), -13);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 3u);
  Result<bool> boolean = decoder.ReadBool();
  ASSERT_EQ(boolean.status(), OkStatus());
  EXPECT_FALSE(boolean.value());

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 4u);
  Result<double> dbl = decoder.ReadDouble();
  ASSERT_EQ(dbl.status(), OkStatus());
  EXPECT_EQ(dbl.value(), 3.14159);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 5u);
  Result<uint32_t> fixed32 = decoder.ReadFixed32();
  ASSERT_EQ(fixed32.status(), OkStatus());
  EXPECT_EQ(fixed32.value(), 0xdeadbeef);

  char buffer[16];
  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 6u);
  StatusWithSize sws = decoder.ReadString(buffer);
  ASSERT_EQ(sws.status(), OkStatus());
  buffer[sws.size()] = '\0';
  EXPECT_STREQ(buffer, "Hello world");

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 7u);
  Result<int32_t> sfixed32 = decoder.ReadSfixed32();
  ASSERT_EQ(sfixed32.status(), OkStatus());
  EXPECT_EQ(sfixed32.value(), -50);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 8u);
  Result<int64_t> sfixed64 = decoder.ReadSfixed64();
  ASSERT_EQ(sfixed64.status(), OkStatus());
  EXPECT_EQ(sfixed64.value(), -1647993274);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 9u);
  Result<float> flt = decoder.ReadFloat();
  ASSERT_EQ(flt.status(), OkStatus());
  EXPECT_EQ(flt.value(), 2.718f);

  EXPECT_EQ(decoder.Next(), Status::OutOfRange());
}

TEST(StreamDecoder, Decode_SkipsUnusedFields) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // type=int32, k=1, v=42
    0x08, 0x2a,
    // type=sint32, k=2, v=-13
    0x10, 0x19,
    // type=bool, k=3, v=false
    0x18, 0x00,
    // type=double, k=4, v=3.14159
    0x21, 0x6e, 0x86, 0x1b, 0xf0, 0xf9, 0x21, 0x09, 0x40,
    // type=fixed32, k=5, v=0xdeadbeef
    0x2d, 0xef, 0xbe, 0xad, 0xde,
    // type=string, k=6, v="Hello world"
    0x32, 0x0b, 'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd',
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  // Don't process any fields except for the fourth. Next should still iterate
  // correctly despite field values not being consumed.
  EXPECT_EQ(decoder.Next(), OkStatus());
  EXPECT_EQ(decoder.Next(), OkStatus());
  EXPECT_EQ(decoder.Next(), OkStatus());
  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(*decoder.FieldNumber(), 4u);
  EXPECT_EQ(decoder.Next(), OkStatus());
  EXPECT_EQ(decoder.Next(), OkStatus());
  EXPECT_EQ(decoder.Next(), Status::OutOfRange());
}

TEST(StreamDecoder, Decode_NonSeekable_SkipsUnusedFields) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // type=int32, k=1, v=42
    0x08, 0x2a,
    // type=sint32, k=2, v=-13
    0x10, 0x19,
    // type=bool, k=3, v=false
    0x18, 0x00,
    // type=double, k=4, v=3.14159
    0x21, 0x6e, 0x86, 0x1b, 0xf0, 0xf9, 0x21, 0x09, 0x40,
    // type=fixed32, k=5, v=0xdeadbeef
    0x2d, 0xef, 0xbe, 0xad, 0xde,
    // type=string, k=6, v="Hello world"
    0x32, 0x0b, 'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd',
  };
  // clang-format on

  // Test with a non-seekable memory reader
  stream::MemoryReader wrapped_reader(as_bytes(span(encoded_proto)));
  NonSeekableMemoryReader reader(wrapped_reader);
  StreamDecoder decoder(reader);

  // Don't process any fields except for the fourth. Next should still iterate
  // correctly despite field values not being consumed.
  EXPECT_EQ(decoder.Next(), OkStatus());
  EXPECT_EQ(decoder.Next(), OkStatus());
  EXPECT_EQ(decoder.Next(), OkStatus());
  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(*decoder.FieldNumber(), 4u);
  EXPECT_EQ(decoder.Next(), OkStatus());
  EXPECT_EQ(decoder.Next(), OkStatus());
  EXPECT_EQ(decoder.Next(), Status::OutOfRange());
}

TEST(StreamDecoder, Decode_BadData) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // type=int32, k=1, v=42
    0x08, 0x2a,
    // type=sint32, k=2, value... missing
    0x10,
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(*decoder.FieldNumber(), 1u);
  Result<int32_t> int32 = decoder.ReadInt32();
  ASSERT_EQ(int32.status(), OkStatus());
  EXPECT_EQ(int32.value(), 42);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(*decoder.FieldNumber(), 2u);
  EXPECT_EQ(decoder.ReadSint32().status(), Status::DataLoss());

  EXPECT_EQ(decoder.Next(), Status::DataLoss());
}

TEST(StreamDecoder, Decode_MissingDelimitedLength) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // type=int32, k=1, v=42
    0x08, 0x2a,
    // Submessage (bytes) key=8, length=... missing
    0x32,
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(*decoder.FieldNumber(), 1u);
  Result<int32_t> int32 = decoder.ReadInt32();
  ASSERT_EQ(int32.status(), OkStatus());
  EXPECT_EQ(int32.value(), 42);

  EXPECT_EQ(decoder.Next(), Status::DataLoss());
}

TEST(StreamDecoder, Decode_VarintTooBig) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // type=uint32, k=1, v=>uint32_t::max
    0x08, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f,
    // type=int32, k=2, v=>int32_t::max
    0x10, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f,
    // type=int32, k=3, v<=int32_t::min
    0x18, 0x80, 0x80, 0x80, 0x80, 0x80, 0xff, 0xff, 0xff, 0xff, 0x01,
    // type=sint32, k=4, v=>int32_t::max
    0x20, 0xfe, 0xff, 0xff, 0xff, 0xff, 0x0f,
    // type=sint32, k=5, v<=int32_t::max
    0x28, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f,
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(*decoder.FieldNumber(), 1u);
  Result<uint32_t> uint32 = decoder.ReadUint32();
  ASSERT_EQ(uint32.status(), Status::FailedPrecondition());

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(*decoder.FieldNumber(), 2u);
  Result<int32_t> int32 = decoder.ReadInt32();
  ASSERT_EQ(int32.status(), Status::FailedPrecondition());

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(*decoder.FieldNumber(), 3u);
  int32 = decoder.ReadInt32();
  ASSERT_EQ(int32.status(), Status::FailedPrecondition());

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(*decoder.FieldNumber(), 4u);
  Result<int32_t> sint32 = decoder.ReadSint32();
  ASSERT_EQ(sint32.status(), Status::FailedPrecondition());

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(*decoder.FieldNumber(), 5u);
  sint32 = decoder.ReadSint32();
  ASSERT_EQ(sint32.status(), Status::FailedPrecondition());

  EXPECT_EQ(decoder.Next(), Status::OutOfRange());
}

TEST(Decoder, Decode_SkipsBadFieldNumbers) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // type=int32, k=1, v=42
    0x08, 0x2a,
    // type=int32, k=19001, v=42 (invalid field number)
    0xc8, 0xa3, 0x09, 0x2a,
    // type=bool, k=3, v=false
    0x18, 0x00,
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  EXPECT_EQ(decoder.Next(), OkStatus());
  EXPECT_EQ(*decoder.FieldNumber(), 1u);
  Result<int32_t> int32 = decoder.ReadInt32();
  ASSERT_EQ(int32.status(), OkStatus());
  EXPECT_EQ(int32.value(), 42);

  // Bad field.
  EXPECT_EQ(decoder.Next(), Status::DataLoss());
  EXPECT_EQ(decoder.FieldNumber().status(), Status::FailedPrecondition());

  EXPECT_EQ(decoder.Next(), Status::DataLoss());
}

TEST(StreamDecoder, Decode_Nested) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // type=int32, k=1, v=42
    0x08, 0x2a,

    // Submessage (bytes) key=8, length=4
    0x32, 0x04,
    // type=uint32, k=1, v=2
    0x08, 0x02,
    // type=uint32, k=2, v=7
    0x10, 0x07,
    // End submessage

    // type=sint32, k=2, v=-13
    0x10, 0x19,
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(*decoder.FieldNumber(), 1u);
  Result<int32_t> int32 = decoder.ReadInt32();
  ASSERT_EQ(int32.status(), OkStatus());
  EXPECT_EQ(int32.value(), 42);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(*decoder.FieldNumber(), 6u);
  {
    StreamDecoder nested = decoder.GetNestedDecoder();

    EXPECT_EQ(nested.Next(), OkStatus());
    ASSERT_EQ(*nested.FieldNumber(), 1u);
    Result<uint32_t> uint32 = nested.ReadUint32();
    ASSERT_EQ(uint32.status(), OkStatus());
    EXPECT_EQ(uint32.value(), 2u);

    EXPECT_EQ(nested.Next(), OkStatus());
    ASSERT_EQ(*nested.FieldNumber(), 2u);
    uint32 = nested.ReadUint32();
    ASSERT_EQ(uint32.status(), OkStatus());
    EXPECT_EQ(uint32.value(), 7u);

    ASSERT_EQ(nested.Next(), Status::OutOfRange());
  }

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(*decoder.FieldNumber(), 2u);
  Result<int32_t> sint32 = decoder.ReadSint32();
  ASSERT_EQ(sint32.status(), OkStatus());
  EXPECT_EQ(sint32.value(), -13);

  EXPECT_EQ(decoder.Next(), Status::OutOfRange());
}

TEST(StreamDecoder, Decode_Nested_SeeksToNextFieldOnDestruction) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // type=int32, k=1, v=42
    0x08, 0x2a,

    // Submessage (bytes) key=8, length=4
    0x32, 0x04,
    // type=uint32, k=1, v=2
    0x08, 0x02,
    // type=uint32, k=2, v=7
    0x10, 0x07,
    // End submessage

    // type=sint32, k=2, v=-13
    0x10, 0x19,
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(*decoder.FieldNumber(), 1u);

  // Create a nested encoder for the nested field, but don't use it.
  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(*decoder.FieldNumber(), 6u);
  { StreamDecoder nested = decoder.GetNestedDecoder(); }

  // The root decoder should still advance to the next field after the nested
  // decoder is closed.
  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(*decoder.FieldNumber(), 2u);

  EXPECT_EQ(decoder.Next(), Status::OutOfRange());
}

TEST(StreamDecoder,
     Decode_Nested_NonSeekable_AdvancesToNextFieldOnDestruction) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // type=int32, k=1, v=42
    0x08, 0x2a,

    // Submessage (bytes) key=8, length=4
    0x32, 0x04,
    // type=uint32, k=1, v=2
    0x08, 0x02,
    // type=uint32, k=2, v=7
    0x10, 0x07,
    // End submessage

    // type=sint32, k=2, v=-13
    0x10, 0x19,
  };
  // clang-format on

  // Test with a non-seekable memory reader
  stream::MemoryReader wrapped_reader(as_bytes(span(encoded_proto)));
  NonSeekableMemoryReader reader(wrapped_reader);
  StreamDecoder decoder(reader);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(*decoder.FieldNumber(), 1u);

  // Create a nested encoder for the nested field, but don't use it.
  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(*decoder.FieldNumber(), 6u);
  { StreamDecoder nested = decoder.GetNestedDecoder(); }

  // The root decoder should still advance to the next field after the nested
  // decoder is closed.
  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(*decoder.FieldNumber(), 2u);

  EXPECT_EQ(decoder.Next(), Status::OutOfRange());
}

TEST(StreamDecoder, Decode_Nested_LastField) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // type=int32, k=1, v=42
    0x08, 0x2a,

    // Submessage (bytes) key=8, length=4
    0x32, 0x04,
    // type=uint32, k=1, v=2
    0x08, 0x02,
    // type=uint32, k=2, v=7
    0x10, 0x07,
    // End submessage and proto
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(*decoder.FieldNumber(), 1u);

  // Create a nested encoder for the nested field, which is the last field in
  // the root proto.
  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(*decoder.FieldNumber(), 6u);
  { StreamDecoder nested = decoder.GetNestedDecoder(); }

  // Root decoder should correctly terminate after the nested decoder is closed.
  EXPECT_EQ(decoder.Next(), Status::OutOfRange());
}

TEST(StreamDecoder, Decode_Nested_MultiLevel) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // Submessage key=1, length=4
    0x0a, 0x04,

    // Sub-submessage key=1, length=2
    0x0a, 0x02,
    // type=uint32, k=2, v=7
    0x10, 0x07,
    // End sub-submessage

    // End submessage
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(*decoder.FieldNumber(), 1u);
  {
    StreamDecoder nested = decoder.GetNestedDecoder();

    EXPECT_EQ(nested.Next(), OkStatus());
    ASSERT_EQ(*nested.FieldNumber(), 1u);

    {
      StreamDecoder double_nested = nested.GetNestedDecoder();

      EXPECT_EQ(double_nested.Next(), OkStatus());
      ASSERT_EQ(*double_nested.FieldNumber(), 2u);
      Result<uint32_t> result = double_nested.ReadUint32();
      ASSERT_EQ(result.status(), OkStatus());
      EXPECT_EQ(result.value(), 7u);

      EXPECT_EQ(double_nested.Next(), Status::OutOfRange());
    }

    EXPECT_EQ(nested.Next(), Status::OutOfRange());
  }

  EXPECT_EQ(decoder.Next(), Status::OutOfRange());
}

TEST(StreamDecoder, Decode_Nested_InvalidField) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // Submessage key=1, length=4
    0x0a, 0x04,

    // Oops. No data!
  };

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(*decoder.FieldNumber(), 1u);
  {
    StreamDecoder nested = decoder.GetNestedDecoder();
    EXPECT_EQ(nested.Next(), Status::DataLoss());
  }

  EXPECT_EQ(decoder.Next(), Status::DataLoss());
}

TEST(StreamDecoder, Decode_Nested_InvalidFieldKey) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // Submessage key=1, length=2
    0x0a, 0x02,
    // type=invalid...
    0xff, 0xff,
    // End submessage

    // type=sint32, k=2, v=-13
    0x10, 0x19,
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(*decoder.FieldNumber(), 1u);

  {
    StreamDecoder nested = decoder.GetNestedDecoder();
    EXPECT_EQ(nested.Next(), Status::DataLoss());

    // Make sure that the nested decoder didn't run off the end of the
    // submessage.
    ASSERT_EQ(reader.Tell(), 4u);
  }
}

TEST(StreamDecoder, Decode_Nested_MissingDelimitedLength) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // Submessage key=1, length=1
    0x0a, 0x01,
    // Delimited field (bytes) key=1, length=missing...
    0x0a,
    // End submessage

    // type=sint32, k=2, v=-13
    0x10, 0x19,
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(*decoder.FieldNumber(), 1u);

  {
    StreamDecoder nested = decoder.GetNestedDecoder();
    EXPECT_EQ(nested.Next(), Status::DataLoss());

    // Make sure that the nested decoder didn't run off the end of the
    // submessage.
    ASSERT_EQ(reader.Tell(), 3u);
  }
}

TEST(StreamDecoder, Decode_Nested_InvalidDelimitedLength) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // Submessage key=1, length=2
    0x0a, 0x02,
    // Delimited field (bytes) key=1, length=invalid...
    0x0a, 0xff,
    // End submessage

    // type=sint32, k=2, v=-13
    0x10, 0x19,
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(*decoder.FieldNumber(), 1u);

  {
    StreamDecoder nested = decoder.GetNestedDecoder();
    EXPECT_EQ(nested.Next(), Status::DataLoss());

    // Make sure that the nested decoder didn't run off the end of the
    // submessage.
    ASSERT_EQ(reader.Tell(), 4u);
  }
}

TEST(StreamDecoder, Decode_Nested_InvalidVarint) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // Submessage key=1, length=2
    0x0a, 0x02,
    // type=uint32 key=1, value=invalid...
    0x08, 0xff,
    // End submessage

    // type=sint32, k=2, v=-13
    0x10, 0x19,
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(*decoder.FieldNumber(), 1u);

  {
    StreamDecoder nested = decoder.GetNestedDecoder();
    EXPECT_EQ(nested.Next(), OkStatus());
    ASSERT_EQ(*nested.FieldNumber(), 1u);

    Result<uint32_t> uint32 = nested.ReadUint32();
    EXPECT_EQ(uint32.status(), Status::DataLoss());

    // Make sure that the nested decoder didn't run off the end of the
    // submessage.
    ASSERT_EQ(reader.Tell(), 4u);
  }
}

TEST(StreamDecoder, Decode_Nested_SkipInvalidVarint) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // Submessage key=1, length=2
    0x0a, 0x02,
    // type=uint32 key=1, value=invalid...
    0x08, 0xff,
    // End submessage

    // type=sint32, k=2, v=-13
    0x10, 0x19,
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(*decoder.FieldNumber(), 1u);

  {
    StreamDecoder nested = decoder.GetNestedDecoder();
    EXPECT_EQ(nested.Next(), OkStatus());
    ASSERT_EQ(*nested.FieldNumber(), 1u);

    // Skip without reading.
    EXPECT_EQ(nested.Next(), Status::DataLoss());

    // Make sure that the nested decoder didn't run off the end of the
    // submessage.
    ASSERT_EQ(reader.Tell(), 4u);
  }
}

TEST(StreamDecoder, Decode_Nested_TruncatedFixed) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // Submessage key=1, length=2
    0x0a, 0x03,
    // type=fixed32 key=1, value=truncated...
    0x0d, 0x42, 0x00,
    // End submessage

    // type=sint32, k=2, v=-13
    0x10, 0x19,
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(*decoder.FieldNumber(), 1u);

  {
    StreamDecoder nested = decoder.GetNestedDecoder();
    EXPECT_EQ(nested.Next(), OkStatus());
    ASSERT_EQ(*nested.FieldNumber(), 1u);

    Result<uint32_t> uint32 = nested.ReadFixed32();
    EXPECT_EQ(uint32.status(), Status::DataLoss());

    // Make sure that the nested decoder didn't run off the end of the
    // submessage. Note that this will not read the data at all in this case.
    ASSERT_EQ(reader.Tell(), 3u);
  }
}

TEST(StreamDecoder, Decode_Nested_SkipTruncatedFixed) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // Submessage key=1, length=2
    0x0a, 0x03,
    // type=fixed32 key=1, value=truncated...
    0x0d, 0x42, 0x00,
    // End submessage

    // type=sint32, k=2, v=-13
    0x10, 0x19,
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(*decoder.FieldNumber(), 1u);

  {
    StreamDecoder nested = decoder.GetNestedDecoder();
    EXPECT_EQ(nested.Next(), OkStatus());
    ASSERT_EQ(*nested.FieldNumber(), 1u);

    // Skip without reading.
    EXPECT_EQ(nested.Next(), Status::DataLoss());

    // Make sure that the nested decoder didn't run off the end of the
    // submessage. Note that this will be unable to skip the field without
    // exceeding the range of the nested decoder, so it won't move the cursor.
    ASSERT_EQ(reader.Tell(), 3u);
  }
}

TEST(StreamDecoder, Decode_BytesReader) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // bytes key=1, length=14
    0x0a, 0x0e,

    0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b,
    0x0c, 0x0d,
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  EXPECT_EQ(decoder.Next(), OkStatus());
  EXPECT_EQ(*decoder.FieldNumber(), 1u);
  {
    StreamDecoder::BytesReader bytes = decoder.GetBytesReader();
    EXPECT_EQ(bytes.field_size(), 14u);

    std::byte buffer[7];
    EXPECT_EQ(bytes.Read(buffer).status(), OkStatus());
    EXPECT_EQ(std::memcmp(buffer, encoded_proto + 2, sizeof(buffer)), 0);

    EXPECT_EQ(bytes.Read(buffer).status(), OkStatus());
    EXPECT_EQ(std::memcmp(buffer, encoded_proto + 9, sizeof(buffer)), 0);

    EXPECT_EQ(bytes.Read(buffer).status(), Status::OutOfRange());
  }

  EXPECT_EQ(decoder.Next(), Status::OutOfRange());
}

TEST(StreamDecoder, Decode_BytesReader_Seek) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // bytes key=1, length=14
    0x0a, 0x0e,

    0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b,
    0x0c, 0x0d,
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  EXPECT_EQ(decoder.Next(), OkStatus());
  EXPECT_EQ(*decoder.FieldNumber(), 1u);
  {
    StreamDecoder::BytesReader bytes = decoder.GetBytesReader();

    std::byte buffer[2];

    ASSERT_EQ(bytes.Seek(3), OkStatus());

    EXPECT_EQ(bytes.Read(buffer).status(), OkStatus());
    EXPECT_EQ(std::memcmp(buffer, encoded_proto + 5, sizeof(buffer)), 0);

    // Bad seek offset (absolute).
    ASSERT_EQ(bytes.Seek(15), Status::OutOfRange());

    // Seek back from current position.
    ASSERT_EQ(bytes.Seek(-4, stream::Stream::kCurrent), OkStatus());

    EXPECT_EQ(bytes.Read(buffer).status(), OkStatus());
    EXPECT_EQ(std::memcmp(buffer, encoded_proto + 3, sizeof(buffer)), 0);

    // Bad seek offset (relative).
    ASSERT_EQ(bytes.Seek(-4, stream::Stream::kCurrent), Status::OutOfRange());

    // Seek from the end of the bytes field.
    ASSERT_EQ(bytes.Seek(-2, stream::Stream::kEnd), OkStatus());

    EXPECT_EQ(bytes.Read(buffer).status(), OkStatus());
    EXPECT_EQ(std::memcmp(buffer, encoded_proto + 14, sizeof(buffer)), 0);

    // Bad seek offset (end).
    ASSERT_EQ(bytes.Seek(-15, stream::Stream::kEnd), Status::OutOfRange());
  }

  EXPECT_EQ(decoder.Next(), Status::OutOfRange());
}

TEST(StreamDecoder, Decode_BytesReader_Close) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // bytes key=1, length=14
    0x0a, 0x0e,

    0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b,
    0x0c, 0x0d,
    // End bytes

    // type=sint32, k=2, v=-13
    0x10, 0x19,
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  EXPECT_EQ(decoder.Next(), OkStatus());
  EXPECT_EQ(*decoder.FieldNumber(), 1u);
  {
    // Partially consume the bytes field.
    StreamDecoder::BytesReader bytes = decoder.GetBytesReader();

    std::byte buffer[2];
    EXPECT_EQ(bytes.Read(buffer).status(), OkStatus());
    EXPECT_EQ(std::memcmp(buffer, encoded_proto + 2, sizeof(buffer)), 0);
  }

  // Continue reading the top-level message.
  EXPECT_EQ(decoder.Next(), OkStatus());
  EXPECT_EQ(*decoder.FieldNumber(), 2u);

  EXPECT_EQ(decoder.Next(), Status::OutOfRange());
}

TEST(StreamDecoder, Decode_BytesReader_NonSeekable_Close) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // bytes key=1, length=14
    0x0a, 0x0e,

    0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b,
    0x0c, 0x0d,
    // End bytes

    // type=sint32, k=2, v=-13
    0x10, 0x19,
  };
  // clang-format on

  // Test with a non-seekable memory reader
  stream::MemoryReader wrapped_reader(as_bytes(span(encoded_proto)));
  NonSeekableMemoryReader reader(wrapped_reader);
  StreamDecoder decoder(reader);

  EXPECT_EQ(decoder.Next(), OkStatus());
  EXPECT_EQ(*decoder.FieldNumber(), 1u);
  {
    // Partially consume the bytes field.
    StreamDecoder::BytesReader bytes = decoder.GetBytesReader();

    std::byte buffer[2];
    EXPECT_EQ(bytes.Read(buffer).status(), OkStatus());
    EXPECT_EQ(std::memcmp(buffer, encoded_proto + 2, sizeof(buffer)), 0);
  }

  // Continue reading the top-level message.
  EXPECT_EQ(decoder.Next(), OkStatus());
  EXPECT_EQ(*decoder.FieldNumber(), 2u);

  EXPECT_EQ(decoder.Next(), Status::OutOfRange());
}

TEST(StreamDecoder, Decode_BytesReader_InvalidField) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // bytes key=1, length=4
    0x0a, 0x04,

    // Oops. No data!
  };

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(*decoder.FieldNumber(), 1u);
  {
    StreamDecoder::BytesReader bytes = decoder.GetBytesReader();
    EXPECT_EQ(bytes.Seek(0), Status::DataLoss());

    std::byte buffer[2];
    EXPECT_EQ(bytes.Read(buffer).status(), Status::DataLoss());
  }

  EXPECT_EQ(decoder.Next(), Status::DataLoss());
}

TEST(StreamDecoder, GetLengthDelimitedPayloadBounds) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // bytes key=1, length=14
    0x0a, 0x0e,

    0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b,
    0x0c, 0x0d,
    // End bytes

    // type=sint32, k=2, v=-13
    0x10, 0x19,
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  ASSERT_EQ(OkStatus(), decoder.Next());
  Result<StreamDecoder::Bounds> field_bound =
      decoder.GetLengthDelimitedPayloadBounds();
  ASSERT_EQ(OkStatus(), field_bound.status());
  ASSERT_EQ(field_bound.value().low, 2ULL);
  ASSERT_EQ(field_bound.value().high, 16ULL);

  ASSERT_EQ(OkStatus(), decoder.Next());
  ASSERT_EQ(Status::NotFound(),
            decoder.GetLengthDelimitedPayloadBounds().status());
}

TEST(StreamDecoder, ReadDelimitedField_DoesntOverConsume) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // type=string, k=1, v="Hello world"
    0x0a, 0x0b, 'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd',
    // type=int32, k=2, v=42
    0x10, 0x2a,
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  ASSERT_EQ(OkStatus(), decoder.Next());

  // This buffer is much larger than the string.
  char buffer[128];
  const StatusWithSize size = decoder.ReadString(buffer);
  EXPECT_EQ(size.status(), OkStatus());
  EXPECT_EQ(size.size(), 11u);

  // Make sure we can still read the next field.
  ASSERT_EQ(OkStatus(), decoder.Next());
  const pw::Result<int32_t> result = decoder.ReadInt32();
  EXPECT_EQ(result.status(), OkStatus());
  EXPECT_EQ(result.value(), 42);
}

TEST(StreamDecoder, Decode_WithLength) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // type=int32, k=1, v=42
    0x08, 0x2a,
    // This field is beyond the range of the protobuf:
    // type=sint32, k=2, v=-13
    0x10, 0x19,
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader, /*length=*/2u);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 1u);
  Result<int32_t> int32 = decoder.ReadInt32();
  ASSERT_EQ(int32.status(), OkStatus());
  EXPECT_EQ(int32.value(), 42);

  EXPECT_EQ(decoder.Next(), Status::OutOfRange());
}

TEST(StreamDecoder, Decode_WithLength_SkipsToEnd) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // type=string, k=1, v="Hello world"
    0x08, 0x0b, 'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd',
    // This field is beyond the range of the protobuf:
    // type=sint32, k=2, v=-13
    0x10, 0x19,
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  {
    StreamDecoder decoder(reader, /*length=*/13u);

    EXPECT_EQ(decoder.Next(), OkStatus());
    ASSERT_EQ(decoder.FieldNumber().value(), 1u);
    // Don't read the value out, or advance further. Destructing the object
    // should advance to the end of the length given.
  }

  EXPECT_EQ(reader.Tell(), 13u);
}

TEST(StreamDecoder, RepeatedField) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // type=uint32, k=1, v=0
    0x08, 0x00,
    // type=uint32, k=1, v=50
    0x08, 0x32,
    // type=uint32, k=1, v=100
    0x08, 0x64,
    // type=uint32, k=1, v=150
    0x08, 0x96, 0x01,
    // type=uint32, k=1, v=200
    0x08, 0xc8, 0x01
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 1u);
  Result<uint32_t> uint32 = decoder.ReadUint32();
  ASSERT_EQ(uint32.status(), OkStatus());
  EXPECT_EQ(uint32.value(), 0u);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 1u);
  uint32 = decoder.ReadUint32();
  ASSERT_EQ(uint32.status(), OkStatus());
  EXPECT_EQ(uint32.value(), 50u);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 1u);
  uint32 = decoder.ReadUint32();
  ASSERT_EQ(uint32.status(), OkStatus());
  EXPECT_EQ(uint32.value(), 100u);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 1u);
  uint32 = decoder.ReadUint32();
  ASSERT_EQ(uint32.status(), OkStatus());
  EXPECT_EQ(uint32.value(), 150u);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 1u);
  uint32 = decoder.ReadUint32();
  ASSERT_EQ(uint32.status(), OkStatus());
  EXPECT_EQ(uint32.value(), 200u);

  EXPECT_EQ(decoder.Next(), Status::OutOfRange());
}

TEST(StreamDecoder, RepeatedFieldVector) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // type=uint32, k=1, v=0
    0x08, 0x00,
    // type=uint32, k=1, v=50
    0x08, 0x32,
    // type=uint32, k=1, v=100
    0x08, 0x64,
    // type=uint32, k=1, v=150
    0x08, 0x96, 0x01,
    // type=uint32, k=1, v=200
    0x08, 0xc8, 0x01
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  pw::Vector<uint32_t, 8> uint32{};

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 1u);
  Status status = decoder.ReadRepeatedUint32(uint32);
  ASSERT_EQ(status, OkStatus());
  EXPECT_EQ(uint32.size(), 1u);
  EXPECT_EQ(uint32[0], 0u);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 1u);
  status = decoder.ReadRepeatedUint32(uint32);
  ASSERT_EQ(status, OkStatus());
  EXPECT_EQ(uint32.size(), 2u);
  EXPECT_EQ(uint32[1], 50u);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 1u);
  status = decoder.ReadRepeatedUint32(uint32);
  ASSERT_EQ(status, OkStatus());
  EXPECT_EQ(uint32.size(), 3u);
  EXPECT_EQ(uint32[2], 100u);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 1u);
  status = decoder.ReadRepeatedUint32(uint32);
  ASSERT_EQ(status, OkStatus());
  EXPECT_EQ(uint32.size(), 4u);
  EXPECT_EQ(uint32[3], 150u);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 1u);
  status = decoder.ReadRepeatedUint32(uint32);
  ASSERT_EQ(status, OkStatus());
  EXPECT_EQ(uint32.size(), 5u);
  EXPECT_EQ(uint32[4], 200u);

  EXPECT_EQ(decoder.Next(), Status::OutOfRange());
}

TEST(StreamDecoder, RepeatedFieldVectorFull) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // type=uint32, k=1, v=0
    0x08, 0x00,
    // type=uint32, k=1, v=50
    0x08, 0x32,
    // type=uint32, k=1, v=100
    0x08, 0x64,
    // type=uint32, k=1, v=150
    0x08, 0x96, 0x01,
    // type=uint32, k=1, v=200
    0x08, 0xc8, 0x01
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  pw::Vector<uint32_t, 2> uint32{};

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 1u);
  Status status = decoder.ReadRepeatedUint32(uint32);
  ASSERT_EQ(status, OkStatus());
  EXPECT_EQ(uint32.size(), 1u);
  EXPECT_EQ(uint32[0], 0u);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 1u);
  status = decoder.ReadRepeatedUint32(uint32);
  ASSERT_EQ(status, OkStatus());
  EXPECT_EQ(uint32.size(), 2u);
  EXPECT_EQ(uint32[1], 50u);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 1u);
  status = decoder.ReadRepeatedUint32(uint32);
  ASSERT_EQ(status, Status::ResourceExhausted());
  EXPECT_EQ(uint32.size(), 2u);
}

TEST(StreamDecoder, PackedVarint) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // type=uint32[], k=1, v={0, 50, 100, 150, 200}
    0x0a, 0x07,
    0x00,
    0x32,
    0x64,
    0x96, 0x01,
    0xc8, 0x01
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 1u);
  std::array<uint32_t, 8> uint32{};
  StatusWithSize size = decoder.ReadPackedUint32(uint32);
  ASSERT_EQ(size.status(), OkStatus());
  EXPECT_EQ(size.size(), 5u);

  EXPECT_EQ(uint32[0], 0u);
  EXPECT_EQ(uint32[1], 50u);
  EXPECT_EQ(uint32[2], 100u);
  EXPECT_EQ(uint32[3], 150u);
  EXPECT_EQ(uint32[4], 200u);
}

TEST(StreamDecoder, PackedVarintInsufficientSpace) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // type=uint32[], k=1, v={0, 50, 100, 150, 200}
    0x0a, 0x07,
    0x00,
    0x32,
    0x64,
    0x96, 0x01,
    0xc8, 0x01
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 1u);
  std::array<uint32_t, 2> uint32{};
  StatusWithSize size = decoder.ReadPackedUint32(uint32);
  ASSERT_EQ(size.status(), Status::ResourceExhausted());
  EXPECT_EQ(size.size(), 2u);

  // Still returns values in case of error.
  EXPECT_EQ(uint32[0], 0u);
  EXPECT_EQ(uint32[1], 50u);
}

TEST(StreamDecoder, PackedVarintVector) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // type=uint32[], k=1, v={0, 50, 100, 150, 200}
    0x0a, 0x07,
    0x00,
    0x32,
    0x64,
    0x96, 0x01,
    0xc8, 0x01
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 1u);
  pw::Vector<uint32_t, 8> uint32{};
  Status status = decoder.ReadRepeatedUint32(uint32);
  ASSERT_EQ(status, OkStatus());
  EXPECT_EQ(uint32.size(), 5u);

  EXPECT_EQ(uint32[0], 0u);
  EXPECT_EQ(uint32[1], 50u);
  EXPECT_EQ(uint32[2], 100u);
  EXPECT_EQ(uint32[3], 150u);
  EXPECT_EQ(uint32[4], 200u);
}

TEST(StreamDecoder, PackedVarintVectorFull) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // type=uint32[], k=1, v={0, 50, 100, 150, 200}
    0x0a, 0x07,
    0x00,
    0x32,
    0x64,
    0x96, 0x01,
    0xc8, 0x01
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 1u);
  pw::Vector<uint32_t, 2> uint32{};
  Status status = decoder.ReadRepeatedUint32(uint32);
  ASSERT_EQ(status, Status::ResourceExhausted());
  EXPECT_EQ(uint32.size(), 2u);

  // Still returns values in case of error.
  EXPECT_EQ(uint32[0], 0u);
  EXPECT_EQ(uint32[1], 50u);
}

TEST(StreamDecoder, PackedZigZag) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // type=sint32[], k=1, v={-100, -25, -1, 0, 1, 25, 100}
    0x0a, 0x09,
    0xc7, 0x01,
    0x31,
    0x01,
    0x00,
    0x02,
    0x32,
    0xc8, 0x01
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 1u);
  std::array<int32_t, 8> sint32{};
  StatusWithSize size = decoder.ReadPackedSint32(sint32);
  ASSERT_EQ(size.status(), OkStatus());
  EXPECT_EQ(size.size(), 7u);

  EXPECT_EQ(sint32[0], -100);
  EXPECT_EQ(sint32[1], -25);
  EXPECT_EQ(sint32[2], -1);
  EXPECT_EQ(sint32[3], 0);
  EXPECT_EQ(sint32[4], 1);
  EXPECT_EQ(sint32[5], 25);
  EXPECT_EQ(sint32[6], 100);
}

TEST(StreamDecoder, PackedZigZagVector) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // type=sint32[], k=1, v={-100, -25, -1, 0, 1, 25, 100}
    0x0a, 0x09,
    0xc7, 0x01,
    0x31,
    0x01,
    0x00,
    0x02,
    0x32,
    0xc8, 0x01
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 1u);
  pw::Vector<int32_t, 8> sint32{};
  Status status = decoder.ReadRepeatedSint32(sint32);
  ASSERT_EQ(status, OkStatus());
  EXPECT_EQ(sint32.size(), 7u);

  EXPECT_EQ(sint32[0], -100);
  EXPECT_EQ(sint32[1], -25);
  EXPECT_EQ(sint32[2], -1);
  EXPECT_EQ(sint32[3], 0);
  EXPECT_EQ(sint32[4], 1);
  EXPECT_EQ(sint32[5], 25);
  EXPECT_EQ(sint32[6], 100);
}

TEST(StreamDecoder, PackedFixed) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // type=fixed32[], k=1, v={0, 50, 100, 150, 200}
    0x0a, 0x14,
    0x00, 0x00, 0x00, 0x00,
    0x32, 0x00, 0x00, 0x00,
    0x64, 0x00, 0x00, 0x00,
    0x96, 0x00, 0x00, 0x00,
    0xc8, 0x00, 0x00, 0x00,
    // type=fixed64[], v=2, v={0x0102030405060708}
    0x12, 0x08,
    0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01,
    // type=sfixed32[], k=3, v={0, -50, 100, -150, 200}
    0x1a, 0x14,
    0x00, 0x00, 0x00, 0x00,
    0xce, 0xff, 0xff, 0xff,
    0x64, 0x00, 0x00, 0x00,
    0x6a, 0xff, 0xff, 0xff,
    0xc8, 0x00, 0x00, 0x00,
    // type=sfixed64[], v=4, v={-1647993274}
    0x22, 0x08,
    0x46, 0x9e, 0xc5, 0x9d, 0xff, 0xff, 0xff, 0xff,
    // type=double[], k=5, v=3.14159
    0x2a, 0x08,
    0x6e, 0x86, 0x1b, 0xf0, 0xf9, 0x21, 0x09, 0x40,
    // type=float[], k=6, v=2.718
    0x32, 0x04,
    0xb6, 0xf3, 0x2d, 0x40,
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 1u);
  std::array<uint32_t, 8> fixed32{};
  StatusWithSize size = decoder.ReadPackedFixed32(fixed32);
  ASSERT_EQ(size.status(), OkStatus());
  EXPECT_EQ(size.size(), 5u);

  EXPECT_EQ(fixed32[0], 0u);
  EXPECT_EQ(fixed32[1], 50u);
  EXPECT_EQ(fixed32[2], 100u);
  EXPECT_EQ(fixed32[3], 150u);
  EXPECT_EQ(fixed32[4], 200u);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 2u);
  std::array<uint64_t, 8> fixed64{};
  size = decoder.ReadPackedFixed64(fixed64);
  ASSERT_EQ(size.status(), OkStatus());
  EXPECT_EQ(size.size(), 1u);

  EXPECT_EQ(fixed64[0], 0x0102030405060708u);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 3u);
  std::array<int32_t, 8> sfixed32{};
  size = decoder.ReadPackedSfixed32(sfixed32);
  ASSERT_EQ(size.status(), OkStatus());
  EXPECT_EQ(size.size(), 5u);

  EXPECT_EQ(sfixed32[0], 0);
  EXPECT_EQ(sfixed32[1], -50);
  EXPECT_EQ(sfixed32[2], 100);
  EXPECT_EQ(sfixed32[3], -150);
  EXPECT_EQ(sfixed32[4], 200);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 4u);
  std::array<int64_t, 8> sfixed64{};
  size = decoder.ReadPackedSfixed64(sfixed64);
  ASSERT_EQ(size.status(), OkStatus());
  EXPECT_EQ(size.size(), 1u);

  EXPECT_EQ(sfixed64[0], -1647993274);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 5u);
  std::array<double, 8> dbl{};
  size = decoder.ReadPackedDouble(dbl);
  ASSERT_EQ(size.status(), OkStatus());
  EXPECT_EQ(size.size(), 1u);

  EXPECT_EQ(dbl[0], 3.14159);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 6u);
  std::array<float, 8> flt{};
  size = decoder.ReadPackedFloat(flt);
  ASSERT_EQ(size.status(), OkStatus());
  EXPECT_EQ(size.size(), 1u);

  EXPECT_EQ(flt[0], 2.718f);

  EXPECT_EQ(decoder.Next(), Status::OutOfRange());
}

TEST(StreamDecoder, PackedFixedInsufficientSpace) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // type=fixed32[], k=1, v={0, 50, 100, 150, 200}
    0x0a, 0x14,
    0x00, 0x00, 0x00, 0x00,
    0x32, 0x00, 0x00, 0x00,
    0x64, 0x00, 0x00, 0x00,
    0x96, 0x00, 0x00, 0x00,
    0xc8, 0x00, 0x00, 0x00,
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 1u);
  std::array<uint32_t, 2> fixed32{};
  StatusWithSize size = decoder.ReadPackedFixed32(fixed32);
  ASSERT_EQ(size.status(), Status::ResourceExhausted());
}

TEST(StreamDecoder, PackedFixedVector) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // type=sfixed32[], k=1, v={0, -50, 100, -150, 200}
    0x0a, 0x14,
    0x00, 0x00, 0x00, 0x00,
    0xce, 0xff, 0xff, 0xff,
    0x64, 0x00, 0x00, 0x00,
    0x6a, 0xff, 0xff, 0xff,
    0xc8, 0x00, 0x00, 0x00,
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 1u);
  pw::Vector<int32_t, 8> sfixed32{};
  Status status = decoder.ReadRepeatedSfixed32(sfixed32);
  ASSERT_EQ(status, OkStatus());
  EXPECT_EQ(sfixed32.size(), 5u);

  EXPECT_EQ(sfixed32[0], 0);
  EXPECT_EQ(sfixed32[1], -50);
  EXPECT_EQ(sfixed32[2], 100);
  EXPECT_EQ(sfixed32[3], -150);
  EXPECT_EQ(sfixed32[4], 200);

  EXPECT_EQ(decoder.Next(), Status::OutOfRange());
}

TEST(StreamDecoder, PackedFixedVectorFull) {
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // type=sfixed32[], k=1, v={0, -50, 100, -150, 200}
    0x0a, 0x14,
    0x00, 0x00, 0x00, 0x00,
    0xce, 0xff, 0xff, 0xff,
    0x64, 0x00, 0x00, 0x00,
    0x6a, 0xff, 0xff, 0xff,
    0xc8, 0x00, 0x00, 0x00,
  };
  // clang-format on

  stream::MemoryReader reader(as_bytes(span(encoded_proto)));
  StreamDecoder decoder(reader);

  EXPECT_EQ(decoder.Next(), OkStatus());
  ASSERT_EQ(decoder.FieldNumber().value(), 1u);
  pw::Vector<int32_t, 2> sfixed32{};
  Status status = decoder.ReadRepeatedSfixed32(sfixed32);
  ASSERT_EQ(status, Status::ResourceExhausted());
  EXPECT_EQ(sfixed32.size(), 0u);
}

}  // namespace
}  // namespace pw::protobuf
