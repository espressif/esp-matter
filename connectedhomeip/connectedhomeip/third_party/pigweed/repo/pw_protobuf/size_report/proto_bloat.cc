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

#include "proto_bloat.h"

#include <array>
#include <cstdint>
#include <string_view>

#include "pw_containers/vector.h"
#include "pw_preprocessor/concat.h"
#include "pw_protobuf/decoder.h"
#include "pw_protobuf/encoder.h"
#include "pw_protobuf/stream_decoder.h"
#include "pw_status/status.h"
#include "pw_stream/null_stream.h"
#include "pw_stream/stream.h"

namespace pw::protobuf_size_report {
namespace {

template <typename T>
constexpr std::array<T, 4> GetIntegerArray() {
  return std::array<T, 4>{958736, 2085792374, 0, 42};
}

template <typename T>
constexpr Vector<T, 4> GetIntegerVector() {
  return Vector<T, 4>{958736, 2085792374, 0, 42};
}

constexpr std::array<bool, 5> GetBoolArray() {
  return std::array<bool, 5>{true, false, false, true, true};
}

Vector<bool, 5> GetBoolVector() {
  return Vector<bool, 5>{true, false, false, true, true};
}

constexpr std::array<float, 5> GetFloatArray() {
  return std::array<float, 5>{1.2f, 3.4f, 5.6e20f, 0.0789f, 0.0f};
}

Vector<float, 5> GetFloatVector() {
  return Vector<float, 5>{1.2f, 3.4f, 5.6e20f, 0.0789f, 0.0f};
}

constexpr std::array<double, 5> GetDoubleArray() {
  return std::array<double, 5>{1.2, 3.4, 5.6e20, 0.0789, 0.0};
}

Vector<double, 5> GetDoubleVector() {
  return Vector<double, 5>{1.2, 3.4, 5.6e20, 0.0789, 0.0};
}

constexpr std::string_view kTestString("I eat chips too often");

constexpr protobuf::internal::MessageField kFakeTable[] = {
    {4567,
     protobuf::WireType::kDelimited,
     234567,
     protobuf::internal::VarintType::kNormal,
     false,
     true,
     true,
     true,
     true,
     260,
     840245,
     nullptr},
    {4567,
     protobuf::WireType::kDelimited,
     234567,
     protobuf::internal::VarintType::kNormal,
     false,
     true,
     true,
     true,
     true,
     260,
     840245,
     nullptr}};

class FakeMessageEncoder : public protobuf::StreamEncoder {
 public:
  FakeMessageEncoder(stream::Writer& writer)
      : protobuf::StreamEncoder(writer, ByteSpan()) {}
  void DoBloat() { Write(ByteSpan(), kFakeTable); }
};

class FakeMessageDecoder : public protobuf::StreamDecoder {
 public:
  FakeMessageDecoder(stream::Reader& reader)
      : protobuf::StreamDecoder(reader) {}
  void DoBloat() { Read(ByteSpan(), kFakeTable); }
};

void CodeToSetUpSizeReportEnvironment() {
  [[maybe_unused]] volatile auto arr1 = GetIntegerArray<uint32_t>();
  [[maybe_unused]] volatile auto arr2 = GetIntegerArray<int32_t>();
  [[maybe_unused]] volatile auto arr3 = GetIntegerArray<uint64_t>();
  [[maybe_unused]] volatile auto arr4 = GetIntegerArray<int64_t>();

  [[maybe_unused]] volatile auto vec1 = GetIntegerVector<uint32_t>();
  [[maybe_unused]] volatile auto vec2 = GetIntegerVector<int32_t>();
  [[maybe_unused]] volatile auto vec3 = GetIntegerVector<uint64_t>();
  [[maybe_unused]] volatile auto vec4 = GetIntegerVector<int64_t>();

  [[maybe_unused]] volatile auto bool1 = GetBoolArray();
  [[maybe_unused]] volatile auto bool2 = GetBoolVector();

  [[maybe_unused]] volatile auto float1 = GetFloatArray();
  [[maybe_unused]] volatile auto float2 = GetFloatVector();

  [[maybe_unused]] volatile auto double1 = GetDoubleArray();
  [[maybe_unused]] volatile auto double2 = GetDoubleVector();

  [[maybe_unused]] volatile std::string_view test_string = kTestString;

  [[maybe_unused]] volatile stream::NullStream null_stream;
}

void Dependencies() {
  std::array<std::byte, 2> buffer;
  stream::NullStream null_stream;
  stream::MemoryWriter memory_writer(buffer);
  memory_writer.Write(buffer);
  null_stream.Write(buffer);
  stream::MemoryReader memory_reader(buffer);
  memory_reader.Read(buffer).IgnoreError();
}

void CodeToPullInProtoEncoder() {
  std::array<std::byte, 1024> buffer;
  protobuf::MemoryEncoder encoder(buffer);

  encoder.WriteUint32(1, 1);
  encoder.WritePackedUint32(1, GetIntegerArray<uint32_t>());
  encoder.WriteRepeatedUint32(1, GetIntegerVector<uint32_t>());

  encoder.WriteInt32(1, 1);
  encoder.WritePackedInt32(1, GetIntegerArray<int32_t>());
  encoder.WriteRepeatedInt32(1, GetIntegerVector<int32_t>());

  encoder.WriteUint64(1, 1);
  encoder.WritePackedUint64(1, GetIntegerArray<uint64_t>());
  encoder.WriteRepeatedUint64(1, GetIntegerVector<uint64_t>());

  encoder.WriteInt64(1, 1);
  encoder.WritePackedInt64(1, GetIntegerArray<int64_t>());
  encoder.WriteRepeatedInt64(1, GetIntegerVector<int64_t>());

  encoder.WriteSint32(1, 1);
  encoder.WritePackedSint32(1, GetIntegerArray<int32_t>());
  encoder.WriteRepeatedSint32(1, GetIntegerVector<int32_t>());

  encoder.WriteSint64(1, 1);
  encoder.WritePackedSint64(1, GetIntegerArray<int64_t>());
  encoder.WriteRepeatedSint64(1, GetIntegerVector<int64_t>());

  encoder.WriteFixed32(1, 1);
  encoder.WritePackedFixed32(1, GetIntegerArray<uint32_t>());
  encoder.WriteRepeatedFixed32(1, GetIntegerVector<uint32_t>());

  encoder.WriteFixed64(1, 1);
  encoder.WritePackedFixed64(1, GetIntegerArray<uint64_t>());
  encoder.WriteRepeatedFixed64(1, GetIntegerVector<uint64_t>());

  encoder.WriteSfixed32(1, 1);
  encoder.WritePackedSfixed32(1, GetIntegerArray<int32_t>());
  encoder.WriteRepeatedSfixed32(1, GetIntegerVector<int32_t>());

  encoder.WriteSfixed64(1, 1);
  encoder.WritePackedSfixed64(1, GetIntegerArray<int64_t>());
  encoder.WriteRepeatedSfixed64(1, GetIntegerVector<int64_t>());

  {
    protobuf::StreamEncoder child = encoder.GetNestedEncoder(0xc01dfee7);

    child.WriteFloat(234, 3.14f);
    child.WritePackedFloat(234, GetFloatArray());
    child.WriteRepeatedFloat(234, GetFloatVector());

    child.WriteFloat(234, 3.14);
    child.WritePackedDouble(234, GetDoubleArray());
    child.WriteRepeatedDouble(234, GetDoubleVector());

    child.WriteBool(7, true);
    child.WritePackedBool(8, GetBoolArray());
    child.WriteRepeatedBool(8, GetBoolVector());

    encoder.WriteBytes(93, as_bytes(span(GetDoubleArray())));
    encoder.WriteString(21343, kTestString);
  }

  stream::NullStream null_stream;
  protobuf::StreamEncoder stream_encoder(null_stream, buffer);
  stream_encoder.WriteBytesFromStream(3636, null_stream, 10824, buffer);
}

void CodeToPullInTableEncoder() {
  stream::NullStream stream;
  FakeMessageEncoder fake_encoder(stream);
  fake_encoder.DoBloat();
}

void CodeToPullInTableDecoder() {
  stream::NullStream stream;
  FakeMessageDecoder fake_decoder(stream);
  fake_decoder.DoBloat();
}

#define _PW_USE_FUNCTIONS_FOR_STREAM_DECODER(type_camel_case, underlying_type) \
  do {                                                                         \
    Status status;                                                             \
    Vector<underlying_type, 3> vec;                                            \
    span<underlying_type> packed_span;                                         \
    status.Update(decoder.PW_CONCAT(Read, type_camel_case)().status());        \
    status.Update(                                                             \
        decoder.PW_CONCAT(ReadPacked, type_camel_case)(packed_span).status()); \
    status.Update(decoder.PW_CONCAT(ReadRepeated, type_camel_case)(vec));      \
    [[maybe_unused]] volatile bool ok = status.ok();                           \
  } while (0)

void CodeToPullInProtoStreamDecoder() {
  stream::NullStream null_stream;
  protobuf::StreamDecoder decoder(null_stream);
  decoder.Next().IgnoreError();

  _PW_USE_FUNCTIONS_FOR_STREAM_DECODER(Int32, int32_t);
  _PW_USE_FUNCTIONS_FOR_STREAM_DECODER(Uint32, uint32_t);
  _PW_USE_FUNCTIONS_FOR_STREAM_DECODER(Int64, int64_t);
  _PW_USE_FUNCTIONS_FOR_STREAM_DECODER(Uint64, uint64_t);

  _PW_USE_FUNCTIONS_FOR_STREAM_DECODER(Sint32, int32_t);
  _PW_USE_FUNCTIONS_FOR_STREAM_DECODER(Sint64, int64_t);

  _PW_USE_FUNCTIONS_FOR_STREAM_DECODER(Bool, bool);

  _PW_USE_FUNCTIONS_FOR_STREAM_DECODER(Fixed32, uint32_t);
  _PW_USE_FUNCTIONS_FOR_STREAM_DECODER(Fixed64, uint64_t);
  _PW_USE_FUNCTIONS_FOR_STREAM_DECODER(Sfixed32, int32_t);
  _PW_USE_FUNCTIONS_FOR_STREAM_DECODER(Sfixed64, int64_t);

  _PW_USE_FUNCTIONS_FOR_STREAM_DECODER(Float, float);
  _PW_USE_FUNCTIONS_FOR_STREAM_DECODER(Double, double);

  {
    Status status;
    span<char> str_out;
    span<std::byte> bytes_out;
    status.Update(decoder.ReadString(str_out).status());
    status.Update(decoder.ReadBytes(bytes_out).status());
    status.Update(decoder.GetLengthDelimitedPayloadBounds().status());
    [[maybe_unused]] volatile Result<uint32_t> field_number =
        decoder.FieldNumber();
    [[maybe_unused]] volatile protobuf::StreamDecoder::BytesReader
        bytes_reader = decoder.GetBytesReader();
    [[maybe_unused]] volatile bool ok = status.ok();
  }
}

#define _PW_USE_FUNCTIONS_FOR_DECODER(type_camel_case, underlying_type) \
  do {                                                                  \
    Status status;                                                      \
    underlying_type val;                                                \
    status.Update(decoder.PW_CONCAT(Read, type_camel_case)(&val));      \
    [[maybe_unused]] volatile bool ok = status.ok();                    \
  } while (0)

void CodeToPullInProtoDecoder() {
  std::array<std::byte, 3> buffer = {
      std::byte(0x01), std::byte(0xff), std::byte(0x08)};
  protobuf::Decoder decoder(buffer);
  decoder.Next().IgnoreError();

  _PW_USE_FUNCTIONS_FOR_DECODER(Int32, int32_t);
  _PW_USE_FUNCTIONS_FOR_DECODER(Uint32, uint32_t);
  _PW_USE_FUNCTIONS_FOR_DECODER(Int64, int64_t);
  _PW_USE_FUNCTIONS_FOR_DECODER(Uint64, uint64_t);

  _PW_USE_FUNCTIONS_FOR_DECODER(Sint32, int32_t);
  _PW_USE_FUNCTIONS_FOR_DECODER(Sint64, int64_t);

  _PW_USE_FUNCTIONS_FOR_DECODER(Bool, bool);

  _PW_USE_FUNCTIONS_FOR_DECODER(Fixed32, uint32_t);
  _PW_USE_FUNCTIONS_FOR_DECODER(Fixed64, uint64_t);
  _PW_USE_FUNCTIONS_FOR_DECODER(Sfixed32, int32_t);
  _PW_USE_FUNCTIONS_FOR_DECODER(Sfixed64, int64_t);

  _PW_USE_FUNCTIONS_FOR_DECODER(Float, float);
  _PW_USE_FUNCTIONS_FOR_DECODER(Double, double);

  {
    Status status;
    std::string_view str_out;
    span<const std::byte> bytes_out;
    status.Update(decoder.ReadString(&str_out));
    status.Update(decoder.ReadBytes(&bytes_out));
    decoder.Reset(buffer);
    [[maybe_unused]] volatile uint32_t field_number = decoder.FieldNumber();
    [[maybe_unused]] volatile bool ok = status.ok();
  }
}

}  // namespace

void BloatWithBase() {
  CodeToSetUpSizeReportEnvironment();
  Dependencies();
}

void BloatWithEncoder() {
  BloatWithBase();
  CodeToPullInProtoEncoder();
}

void BloatWithTableEncoder() {
  BloatWithBase();
  CodeToPullInTableEncoder();
}

void BloatWithTableDecoder() {
  BloatWithBase();
  CodeToPullInTableDecoder();
}

void BloatWithStreamDecoder() {
  BloatWithBase();
  CodeToPullInProtoStreamDecoder();
}

void BloatWithDecoder() {
  BloatWithBase();
  CodeToPullInProtoDecoder();
}

}  // namespace pw::protobuf_size_report
