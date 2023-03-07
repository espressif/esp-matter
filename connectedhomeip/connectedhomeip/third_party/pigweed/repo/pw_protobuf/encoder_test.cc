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

#include "pw_protobuf/encoder.h"

#include "gtest/gtest.h"
#include "pw_bytes/span.h"
#include "pw_span/span.h"
#include "pw_stream/memory_stream.h"

namespace pw::protobuf {
namespace {

using stream::MemoryWriter;

// The tests in this file use the following proto message schemas.
//
//   message TestProto {
//     uint32 magic_number = 1;
//     sint32 ziggy = 2;
//     fixed64 cycles = 3;
//     float ratio = 4;
//     string error_message = 5;
//     NestedProto nested = 6;
//   }
//
//   message NestedProto {
//     string hello = 1;
//     uint32 id = 2;
//     repeated DoubleNestedProto pair = 3;
//   }
//
//   message DoubleNestedProto {
//     string key = 1;
//     string value = 2;
//   }
//

constexpr uint32_t kTestProtoMagicNumberField = 1;
constexpr uint32_t kTestProtoZiggyField = 2;
constexpr uint32_t kTestProtoCyclesField = 3;
constexpr uint32_t kTestProtoRatioField = 4;
constexpr uint32_t kTestProtoErrorMessageField = 5;
constexpr uint32_t kTestProtoNestedField = 6;
constexpr uint32_t kTestProtoPayloadFromStreamField = 7;

constexpr uint32_t kNestedProtoHelloField = 1;
constexpr uint32_t kNestedProtoIdField = 2;
constexpr uint32_t kNestedProtoPairField = 3;

constexpr uint32_t kDoubleNestedProtoKeyField = 1;
constexpr uint32_t kDoubleNestedProtoValueField = 2;

TEST(StreamEncoder, EncodePrimitives) {
  // TestProto tp;
  // tp.magic_number = 42;
  // tp.ziggy = -13;
  // tp.cycles = 0xdeadbeef8badf00d;
  // tp.ratio = 1.618034;
  // tp.error_message = "broken 💩";
  // tp.payload_from_stream = "byreader"

  // Hand-encoded version of the above.
  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // magic_number [varint k=1]
    0x08, 0x2a,
    // ziggy [varint k=2]
    0x10, 0x19,
    // cycles [fixed64 k=3]
    0x19, 0x0d, 0xf0, 0xad, 0x8b, 0xef, 0xbe, 0xad, 0xde,
    // ratio [fixed32 k=4]
    0x25, 0xbd, 0x1b, 0xcf, 0x3f,
    // error_message [delimited k=5],
    0x2a, 0x0b, 'b', 'r', 'o', 'k', 'e', 'n', ' ',
    // poop!
    0xf0, 0x9f, 0x92, 0xa9,
    // payload_from_stream [delimited k=7]
    0x3a, 0x08, 'b', 'y', 'r', 'e', 'a', 'd', 'e', 'r',
  };
  // clang-format on
  std::byte encode_buffer[64];
  std::byte dest_buffer[64];
  // This writer isn't necessary, it's just the most testable way to exercise
  // a stream interface. Use a MemoryEncoder when encoding a proto directly to
  // an in-memory buffer.
  MemoryWriter writer(dest_buffer);
  StreamEncoder encoder(writer, encode_buffer);

  EXPECT_EQ(encoder.WriteUint32(kTestProtoMagicNumberField, 42), OkStatus());
  EXPECT_EQ(writer.bytes_written(), 2u);
  EXPECT_EQ(encoder.WriteSint32(kTestProtoZiggyField, -13), OkStatus());
  EXPECT_EQ(encoder.WriteFixed64(kTestProtoCyclesField, 0xdeadbeef8badf00d),
            OkStatus());
  EXPECT_EQ(encoder.WriteFloat(kTestProtoRatioField, 1.618034), OkStatus());
  EXPECT_EQ(encoder.WriteString(kTestProtoErrorMessageField, "broken 💩"),
            OkStatus());

  const std::string_view kReaderMessage = "byreader";
  stream::MemoryReader msg_reader(as_bytes(span(kReaderMessage)));
  std::byte stream_pipe_buffer[1];
  EXPECT_EQ(encoder.WriteStringFromStream(kTestProtoPayloadFromStreamField,
                                          msg_reader,
                                          kReaderMessage.size(),
                                          stream_pipe_buffer),
            OkStatus());

  ASSERT_EQ(encoder.status(), OkStatus());
  ConstByteSpan result = writer.WrittenData();
  EXPECT_EQ(result.size(), sizeof(encoded_proto));
  EXPECT_EQ(std::memcmp(result.data(), encoded_proto, sizeof(encoded_proto)),
            0);
}

TEST(StreamEncoder, EncodeInsufficientSpace) {
  std::byte encode_buffer[12];
  MemoryEncoder encoder(encode_buffer);

  // 2 bytes.
  EXPECT_EQ(encoder.WriteUint32(kTestProtoMagicNumberField, 42), OkStatus());
  // 2 bytes.
  EXPECT_EQ(encoder.WriteSint32(kTestProtoZiggyField, -13), OkStatus());
  // 9 bytes; not enough space! The encoder will start writing the field but
  // should rollback when it realizes it doesn't have enough space.
  EXPECT_EQ(encoder.WriteFixed64(kTestProtoCyclesField, 0xdeadbeef8badf00d),
            Status::ResourceExhausted());
  // Any further write operations should fail.
  EXPECT_EQ(encoder.WriteFloat(kTestProtoRatioField, 1.618034),
            Status::ResourceExhausted());

  ASSERT_EQ(encoder.status(), Status::ResourceExhausted());
}

TEST(StreamEncoder, EncodeInvalidArguments) {
  std::byte encode_buffer[12];
  MemoryEncoder encoder(encode_buffer);

  EXPECT_EQ(encoder.WriteUint32(kTestProtoMagicNumberField, 42), OkStatus());
  // Invalid proto field numbers.
  EXPECT_EQ(encoder.WriteUint32(0, 1337), Status::InvalidArgument());

  // TODO(amontanez): Does it make sense to support this?
  // encoder.Clear();

  EXPECT_EQ(encoder.WriteString(1u << 31, "ha"), Status::InvalidArgument());

  // TODO(amontanez): Does it make sense to support this?
  // encoder.Clear();

  EXPECT_EQ(encoder.WriteBool(19091, false), Status::InvalidArgument());
  ASSERT_EQ(encoder.status(), Status::InvalidArgument());
}

TEST(StreamEncoder, Nested) {
  // This is the largest complete submessage in this test.
  constexpr size_t kLargestSubmessageSize = 0x30;
  constexpr size_t kScratchBufferSize =
      MaxScratchBufferSize(kLargestSubmessageSize, 2);
  std::byte encode_buffer[kScratchBufferSize];
  std::byte dest_buffer[128];
  MemoryWriter writer(dest_buffer);
  StreamEncoder encoder(writer, encode_buffer);

  // TestProto test_proto;
  // test_proto.magic_number = 42;
  EXPECT_EQ(encoder.WriteUint32(kTestProtoMagicNumberField, 42), OkStatus());

  {
    // NestedProto& nested_proto = test_proto.nested;
    StreamEncoder nested_proto =
        encoder.GetNestedEncoder(kTestProtoNestedField);
    // nested_proto.hello = "world";
    EXPECT_EQ(nested_proto.WriteString(kNestedProtoHelloField, "world"),
              OkStatus());

    {
      // DoubleNestedProto& double_nested_proto = nested_proto.append_pair();
      StreamEncoder double_nested_proto =
          nested_proto.GetNestedEncoder(kNestedProtoPairField);
      // double_nested_proto.key = "version";
      EXPECT_EQ(double_nested_proto.WriteString(kDoubleNestedProtoKeyField,
                                                "version"),
                OkStatus());
      // double_nested_proto.value = "2.9.1";
      EXPECT_EQ(double_nested_proto.WriteString(kDoubleNestedProtoValueField,
                                                "2.9.1"),
                OkStatus());
    }  // end DoubleNestedProto

    // nested_proto.id = 999;
    EXPECT_EQ(nested_proto.WriteUint32(kNestedProtoIdField, 999), OkStatus());

    {
      // DoubleNestedProto& double_nested_proto = nested_proto.append_pair();
      StreamEncoder double_nested_proto =
          nested_proto.GetNestedEncoder(kNestedProtoPairField);
      // double_nested_proto.key = "device";
      EXPECT_EQ(
          double_nested_proto.WriteString(kDoubleNestedProtoKeyField, "device"),
          OkStatus());
      // double_nested_proto.value = "left-soc";
      EXPECT_EQ(double_nested_proto.WriteString(kDoubleNestedProtoValueField,
                                                "left-soc"),
                OkStatus());
      // Rely on destructor for finalization.
    }  // end DoubleNestedProto
  }    // end NestedProto

  // test_proto.ziggy = -13;
  EXPECT_EQ(encoder.WriteSint32(kTestProtoZiggyField, -13), OkStatus());

  // clang-format off
  constexpr uint8_t encoded_proto[] = {
    // magic_number
    0x08, 0x2a,
    // nested header (key, size)
    0x32, 0x30,
    // nested.hello
    0x0a, 0x05, 'w', 'o', 'r', 'l', 'd',
    // nested.pair[0] header (key, size)
    0x1a, 0x10,
    // nested.pair[0].key
    0x0a, 0x07, 'v', 'e', 'r', 's', 'i', 'o', 'n',
    // nested.pair[0].value
    0x12, 0x05, '2', '.', '9', '.', '1',
    // nested.id
    0x10, 0xe7, 0x07,
    // nested.pair[1] header (key, size)
    0x1a, 0x12,
    // nested.pair[1].key
    0x0a, 0x06, 'd', 'e', 'v', 'i', 'c', 'e',
    // nested.pair[1].value
    0x12, 0x08, 'l', 'e', 'f', 't', '-', 's', 'o', 'c',
    // ziggy
    0x10, 0x19
  };
  // clang-format on

  ASSERT_EQ(encoder.status(), OkStatus());
  ConstByteSpan result = ConstByteSpan(writer.data(), writer.bytes_written());
  EXPECT_EQ(result.size(), sizeof(encoded_proto));
  EXPECT_EQ(std::memcmp(result.data(), encoded_proto, sizeof(encoded_proto)),
            0);
}

TEST(StreamEncoder, RepeatedField) {
  std::byte encode_buffer[32];
  MemoryEncoder encoder(encode_buffer);

  // repeated uint32 values = 1;
  constexpr uint32_t values[] = {0, 50, 100, 150, 200};
  for (int i = 0; i < 5; ++i) {
    ASSERT_EQ(OkStatus(), encoder.WriteUint32(1, values[i]));
  }

  constexpr uint8_t encoded_proto[] = {
      0x08, 0x00, 0x08, 0x32, 0x08, 0x64, 0x08, 0x96, 0x01, 0x08, 0xc8, 0x01};

  ASSERT_EQ(encoder.status(), OkStatus());
  ConstByteSpan result(encoder);
  EXPECT_EQ(result.size(), sizeof(encoded_proto));
  EXPECT_EQ(std::memcmp(result.data(), encoded_proto, sizeof(encoded_proto)),
            0);
}

TEST(StreamEncoder, PackedVarint) {
  std::byte encode_buffer[32];
  MemoryEncoder encoder(encode_buffer);

  // repeated uint32 values = 1;
  constexpr uint32_t values[] = {0, 50, 100, 150, 200};
  ASSERT_EQ(OkStatus(), encoder.WritePackedUint32(1, values));

  constexpr uint8_t encoded_proto[] = {
      0x0a, 0x07, 0x00, 0x32, 0x64, 0x96, 0x01, 0xc8, 0x01};
  //  key   size  v[0]  v[1]  v[2]  v[3]        v[4]

  ASSERT_EQ(encoder.status(), OkStatus());
  ConstByteSpan result(encoder);
  EXPECT_EQ(result.size(), sizeof(encoded_proto));
  EXPECT_EQ(std::memcmp(result.data(), encoded_proto, sizeof(encoded_proto)),
            0);
}

TEST(StreamEncoder, PackedVarintInsufficientSpace) {
  std::byte encode_buffer[8];
  MemoryEncoder encoder(encode_buffer);

  constexpr uint32_t values[] = {0, 50, 100, 150, 200};
  ASSERT_EQ(Status::ResourceExhausted(), encoder.WritePackedUint32(1, values));

  EXPECT_EQ(encoder.status(), Status::ResourceExhausted());
}

TEST(StreamEncoder, PackedVarintVector) {
  std::byte encode_buffer[32];
  MemoryEncoder encoder(encode_buffer);

  // repeated uint32 values = 1;
  const pw::Vector<uint32_t, 5> values = {0, 50, 100, 150, 200};
  ASSERT_EQ(OkStatus(), encoder.WriteRepeatedUint32(1, values));

  constexpr uint8_t encoded_proto[] = {
      0x0a, 0x07, 0x00, 0x32, 0x64, 0x96, 0x01, 0xc8, 0x01};
  //  key   size  v[0]  v[1]  v[2]  v[3]        v[4]

  ASSERT_EQ(encoder.status(), OkStatus());
  ConstByteSpan result(encoder);
  EXPECT_EQ(result.size(), sizeof(encoded_proto));
  EXPECT_EQ(std::memcmp(result.data(), encoded_proto, sizeof(encoded_proto)),
            0);
}

TEST(StreamEncoder, PackedVarintVectorInsufficientSpace) {
  std::byte encode_buffer[8];
  MemoryEncoder encoder(encode_buffer);

  const pw::Vector<uint32_t, 5> values = {0, 50, 100, 150, 200};
  ASSERT_EQ(Status::ResourceExhausted(),
            encoder.WriteRepeatedUint32(1, values));

  EXPECT_EQ(encoder.status(), Status::ResourceExhausted());
}

TEST(StreamEncoder, PackedBool) {
  std::byte encode_buffer[32];
  MemoryEncoder encoder(encode_buffer);

  // repeated bool values = 1;
  constexpr bool values[] = {true, false, true, true, false};
  ASSERT_EQ(OkStatus(), encoder.WritePackedBool(1, values));

  constexpr uint8_t encoded_proto[] = {
      0x0a, 0x05, 0x01, 0x00, 0x01, 0x01, 0x00};
  //  key   size  v[0]  v[1]  v[2]  v[3]  v[4]

  ASSERT_EQ(encoder.status(), OkStatus());
  ConstByteSpan result(encoder);
  EXPECT_EQ(result.size(), sizeof(encoded_proto));
  EXPECT_EQ(std::memcmp(result.data(), encoded_proto, sizeof(encoded_proto)),
            0);
}

TEST(StreamEncoder, PackedFixed) {
  std::byte encode_buffer[32];
  MemoryEncoder encoder(encode_buffer);

  // repeated fixed32 values = 1;
  constexpr uint32_t values[] = {0, 50, 100, 150, 200};
  ASSERT_EQ(OkStatus(), encoder.WritePackedFixed32(1, values));

  // repeated fixed64 values64 = 2;
  constexpr uint64_t values64[] = {0x0102030405060708};
  ASSERT_EQ(OkStatus(), encoder.WritePackedFixed64(2, values64));

  constexpr uint8_t encoded_proto[] = {
      0x0a, 0x14, 0x00, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0x64,
      0x00, 0x00, 0x00, 0x96, 0x00, 0x00, 0x00, 0xc8, 0x00, 0x00, 0x00,
      0x12, 0x08, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01};

  ASSERT_EQ(encoder.status(), OkStatus());
  ConstByteSpan result(encoder);
  EXPECT_EQ(result.size(), sizeof(encoded_proto));
  EXPECT_EQ(std::memcmp(result.data(), encoded_proto, sizeof(encoded_proto)),
            0);
}

TEST(StreamEncoder, PackedFixedVector) {
  std::byte encode_buffer[32];
  MemoryEncoder encoder(encode_buffer);

  // repeated fixed32 values = 1;
  const pw::Vector<uint32_t, 5> values = {0, 50, 100, 150, 200};
  ASSERT_EQ(OkStatus(), encoder.WriteRepeatedFixed32(1, values));

  // repeated fixed64 values64 = 2;
  const pw::Vector<uint64_t, 1> values64 = {0x0102030405060708};
  ASSERT_EQ(OkStatus(), encoder.WriteRepeatedFixed64(2, values64));

  constexpr uint8_t encoded_proto[] = {
      0x0a, 0x14, 0x00, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0x64,
      0x00, 0x00, 0x00, 0x96, 0x00, 0x00, 0x00, 0xc8, 0x00, 0x00, 0x00,
      0x12, 0x08, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01};

  ASSERT_EQ(encoder.status(), OkStatus());
  ConstByteSpan result(encoder);
  EXPECT_EQ(result.size(), sizeof(encoded_proto));
  EXPECT_EQ(std::memcmp(result.data(), encoded_proto, sizeof(encoded_proto)),
            0);
}

TEST(StreamEncoder, PackedSfixedVector) {
  std::byte encode_buffer[32];
  MemoryEncoder encoder(encode_buffer);

  // repeated fixed32 values = 1;
  const pw::Vector<int32_t, 5> values = {0, 50, 100, 150, 200};
  ASSERT_EQ(OkStatus(), encoder.WriteRepeatedSfixed32(1, values));

  // repeated fixed64 values64 = 2;
  const pw::Vector<int64_t, 1> values64 = {-2};
  ASSERT_EQ(OkStatus(), encoder.WriteRepeatedSfixed64(2, values64));

  constexpr uint8_t encoded_proto[] = {
      0x0a, 0x14, 0x00, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0x64,
      0x00, 0x00, 0x00, 0x96, 0x00, 0x00, 0x00, 0xc8, 0x00, 0x00, 0x00,
      0x12, 0x08, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

  ASSERT_EQ(encoder.status(), OkStatus());
  ConstByteSpan result(encoder);
  EXPECT_EQ(result.size(), sizeof(encoded_proto));
  EXPECT_EQ(std::memcmp(result.data(), encoded_proto, sizeof(encoded_proto)),
            0);
}

TEST(StreamEncoder, PackedZigzag) {
  std::byte encode_buffer[32];
  MemoryEncoder encoder(encode_buffer);

  // repeated sint32 values = 1;
  constexpr int32_t values[] = {-100, -25, -1, 0, 1, 25, 100};
  ASSERT_EQ(OkStatus(), encoder.WritePackedSint32(1, values));

  constexpr uint8_t encoded_proto[] = {
      0x0a, 0x09, 0xc7, 0x01, 0x31, 0x01, 0x00, 0x02, 0x32, 0xc8, 0x01};

  ASSERT_EQ(encoder.status(), OkStatus());
  ConstByteSpan result(encoder);
  EXPECT_EQ(result.size(), sizeof(encoded_proto));
  EXPECT_EQ(std::memcmp(result.data(), encoded_proto, sizeof(encoded_proto)),
            0);
}

TEST(StreamEncoder, PackedZigzagVector) {
  std::byte encode_buffer[32];
  MemoryEncoder encoder(encode_buffer);

  // repeated sint32 values = 1;
  const pw::Vector<int32_t, 7> values = {-100, -25, -1, 0, 1, 25, 100};
  ASSERT_EQ(OkStatus(), encoder.WriteRepeatedSint32(1, values));

  constexpr uint8_t encoded_proto[] = {
      0x0a, 0x09, 0xc7, 0x01, 0x31, 0x01, 0x00, 0x02, 0x32, 0xc8, 0x01};

  ASSERT_EQ(encoder.status(), OkStatus());
  ConstByteSpan result(encoder);
  EXPECT_EQ(result.size(), sizeof(encoded_proto));
  EXPECT_EQ(std::memcmp(result.data(), encoded_proto, sizeof(encoded_proto)),
            0);
}

TEST(StreamEncoder, ParentUnavailable) {
  std::byte encode_buffer[32];
  MemoryEncoder parent(encode_buffer);
  {
    StreamEncoder child = parent.GetNestedEncoder(kTestProtoNestedField);
    ASSERT_EQ(child.status(), OkStatus());
  }
  ASSERT_EQ(parent.status(), OkStatus());
}

TEST(StreamEncoder, NestedEncoderRequiresBuffer) {
  MemoryEncoder parent((ByteSpan()));
  {
    StreamEncoder child = parent.GetNestedEncoder(kTestProtoNestedField);

    ASSERT_EQ(child.status(), Status::ResourceExhausted());
  }
  ASSERT_EQ(parent.status(), Status::ResourceExhausted());
}

TEST(StreamEncoder, WriteTooBig) {
  constexpr size_t kTempBufferSize = 32;
  constexpr size_t kWriteSize = 2;
  std::byte encode_buffer[32];
  MemoryEncoder encoder(encode_buffer);
  // Each write is 2 bytes. Ensure we can write 16 times.
  for (size_t i = 0; i < kTempBufferSize; i += kWriteSize) {
    ASSERT_EQ(encoder.WriteUint32(1, 12), OkStatus());
  }
  ASSERT_EQ(encoder.size(), kTempBufferSize);
  ASSERT_EQ(encoder.WriteUint32(1, 12), Status::ResourceExhausted());
}

TEST(StreamEncoder, EmptyChildWrites) {
  std::byte encode_buffer[32];
  MemoryEncoder parent(encode_buffer);
  { StreamEncoder child = parent.GetNestedEncoder(kTestProtoNestedField); }
  ASSERT_EQ(parent.status(), OkStatus());
  const size_t kExpectedSize =
      varint::EncodedSize(
          FieldKey(kTestProtoNestedField, WireType::kDelimited)) +
      varint::EncodedSize(0);
  ASSERT_EQ(parent.size(), kExpectedSize);
}

TEST(StreamEncoder, NestedStatusPropagates) {
  std::byte encode_buffer[32];
  MemoryEncoder parent(encode_buffer);
  {
    StreamEncoder child = parent.GetNestedEncoder(kTestProtoNestedField);
    ASSERT_EQ(child.WriteUint32(0, 0), Status::InvalidArgument());
  }
  ASSERT_EQ(parent.status(), Status::InvalidArgument());
}

TEST(StreamEncoder, ManualCloseEncoderWrites) {
  std::byte encode_buffer[32];
  MemoryEncoder parent(encode_buffer);
  StreamEncoder child = parent.GetNestedEncoder(kTestProtoNestedField);
  child.CloseEncoder();
  ASSERT_EQ(parent.status(), OkStatus());
  const size_t kExpectedSize =
      varint::EncodedSize(
          FieldKey(kTestProtoNestedField, WireType::kDelimited)) +
      varint::EncodedSize(0);
  ASSERT_EQ(parent.size(), kExpectedSize);
}

}  // namespace
}  // namespace pw::protobuf
