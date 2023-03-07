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
#pragma once

#include <array>
#include <cstring>
#include <limits>
#include <type_traits>

#include "pw_assert/assert.h"
#include "pw_containers/vector.h"
#include "pw_protobuf/internal/codegen.h"
#include "pw_protobuf/wire_format.h"
#include "pw_span/span.h"
#include "pw_status/status.h"
#include "pw_status/status_with_size.h"
#include "pw_stream/stream.h"
#include "pw_varint/stream.h"
#include "pw_varint/varint.h"

namespace pw::protobuf {

// A low-level, event-based protobuf wire format decoder that operates on a
// stream.
//
// The decoder processes an encoded message by iterating over its fields. The
// caller can extract the values of any fields it cares about.
//
// The decoder does not provide any in-memory data structures to represent a
// protobuf message's data. More sophisticated APIs can be built on top of the
// low-level decoder to provide additional functionality, if desired.
//
// **NOTE**
// This decoder is intended to be used for protobuf messages which are too large
// to fit in memory. For smaller messages, prefer the MemoryDecoder, which is
// much more efficient.
//
// Example usage:
//
//   stream::Reader& my_stream = GetProtoStream();
//   StreamDecoder decoder(my_stream);
//
//   while (decoder.Next().ok()) {
//     // FieldNumber() will always be valid if Next() returns OK.
//     switch (decoder.FieldNumber().value()) {
//       case 1:
//         Result<uint32_t> result = decoder.ReadUint32();
//         if (result.ok()) {
//           DoSomething(result.value());
//         }
//         break;
//       // ... and other fields.
//     }
//   }
//
class StreamDecoder {
 public:
  // stream::Reader for a bytes field in a streamed proto message.
  //
  // Shares the StreamDecoder's reader, limiting it to the bounds of a bytes
  // field. If the StreamDecoder's reader does not supporting seeking, this
  // will also not.
  class BytesReader : public stream::RelativeSeekableReader {
   public:
    ~BytesReader() override { decoder_.CloseBytesReader(*this); }

    constexpr size_t field_size() const { return end_offset_ - start_offset_; }

   private:
    friend class StreamDecoder;

    constexpr BytesReader(StreamDecoder& decoder,
                          size_t start_offset,
                          size_t end_offset)
        : decoder_(decoder),
          start_offset_(start_offset),
          end_offset_(end_offset),
          status_(OkStatus()) {}

    constexpr BytesReader(StreamDecoder& decoder, Status status)
        : decoder_(decoder),
          start_offset_(0),
          end_offset_(0),
          status_(status) {}

    StatusWithSize DoRead(ByteSpan destination) final;
    Status DoSeek(ptrdiff_t offset, Whence origin) final;

    StreamDecoder& decoder_;
    size_t start_offset_;
    size_t end_offset_;
    Status status_;
  };

  constexpr StreamDecoder(stream::Reader& reader)
      : StreamDecoder(reader, std::numeric_limits<size_t>::max()) {}

  // Allow the maximum length of the protobuf to be specified to the decoder
  // for streaming situations. When constructed in this way, the decoder will
  // consume any remaining bytes when it goes out of scope.
  constexpr StreamDecoder(stream::Reader& reader, size_t length)
      : reader_(reader),
        stream_bounds_({0, length}),
        position_(0),
        current_field_(kInitialFieldKey),
        delimited_field_size_(0),
        delimited_field_offset_(0),
        parent_(nullptr),
        field_consumed_(true),
        nested_reader_open_(false),
        status_(OkStatus()) {}

  StreamDecoder(const StreamDecoder& other) = delete;
  StreamDecoder& operator=(const StreamDecoder& other) = delete;

  ~StreamDecoder();

  // Advances to the next field in the proto.
  //
  // If Next() returns OK, there is guaranteed to be a valid protobuf field at
  // the current position, which can then be consumed through one of the Read*
  // methods.
  //
  // Return values:
  //
  //             OK: Advanced to a valid proto field.
  //   OUT_OF_RANGE: Reached the end of the proto message.
  //      DATA_LOSS: Invalid protobuf data.
  //
  Status Next();

  // Returns the field number of the current field.
  //
  // Can only be called after a successful call to Next() and before any
  // Read*() operation.
  constexpr Result<uint32_t> FieldNumber() const {
    if (field_consumed_) {
      return Status::FailedPrecondition();
    }

    return status_.ok() ? current_field_.field_number()
                        : Result<uint32_t>(status_);
  }

  //
  // TODO(frolv): Add Status Read*(T& value) APIs alongside the Result<T> ones.
  //

  // Reads a proto int32 value from the current position.
  Result<int32_t> ReadInt32() {
    return ReadVarintField<int32_t>(internal::VarintType::kNormal);
  }

  // Reads repeated int32 values from the current position using packed
  // encoding.
  //
  // Returns the number of values read. In the case of error, the return value
  // indicates the number of values successfully read, in addition to the error.
  StatusWithSize ReadPackedInt32(span<int32_t> out) {
    return ReadPackedVarintField(
        as_writable_bytes(out), sizeof(int32_t), internal::VarintType::kNormal);
  }

  // Reads repeated int32 values from the current position into the vector,
  // supporting either repeated single field elements or packed encoding.
  Status ReadRepeatedInt32(pw::Vector<int32_t>& out) {
    return ReadRepeatedVarintField<int32_t>(out, internal::VarintType::kNormal);
  }

  // Reads a proto uint32 value from the current position.
  Result<uint32_t> ReadUint32() {
    return ReadVarintField<uint32_t>(internal::VarintType::kUnsigned);
  }

  // Reads repeated uint32 values from the current position using packed
  // encoding.
  //
  // Returns the number of values read. In the case of error, the return value
  // indicates the number of values successfully read, in addition to the error.
  StatusWithSize ReadPackedUint32(span<uint32_t> out) {
    return ReadPackedVarintField(as_writable_bytes(out),
                                 sizeof(uint32_t),
                                 internal::VarintType::kUnsigned);
  }

  // Reads repeated uint32 values from the current position into the vector,
  // supporting either repeated single field elements or packed encoding.
  Status ReadRepeatedUint32(pw::Vector<uint32_t>& out) {
    return ReadRepeatedVarintField<uint32_t>(out,
                                             internal::VarintType::kUnsigned);
  }

  // Reads a proto int64 value from the current position.
  Result<int64_t> ReadInt64() {
    return ReadVarintField<int64_t>(internal::VarintType::kNormal);
  }

  // Reads repeated int64 values from the current position using packed
  // encoding.
  //
  // Returns the number of values read. In the case of error, the return value
  // indicates the number of values successfully read, in addition to the
  // error.
  StatusWithSize ReadPackedInt64(span<int64_t> out) {
    return ReadPackedVarintField(
        as_writable_bytes(out), sizeof(int64_t), internal::VarintType::kNormal);
  }

  // Reads repeated int64 values from the current position into the vector,
  // supporting either repeated single field elements or packed encoding.
  Status ReadRepeatedInt64(pw::Vector<int64_t>& out) {
    return ReadRepeatedVarintField<int64_t>(out, internal::VarintType::kNormal);
  }

  // Reads a proto uint64 value from the current position.
  Result<uint64_t> ReadUint64() {
    return ReadVarintField<uint64_t>(internal::VarintType::kUnsigned);
  }

  // Reads repeated uint64 values from the current position using packed
  // encoding.
  //
  // Returns the number of values read. In the case of error, the return value
  // indicates the number of values successfully read, in addition to the
  // error.
  StatusWithSize ReadPackedUint64(span<uint64_t> out) {
    return ReadPackedVarintField(as_writable_bytes(out),
                                 sizeof(uint64_t),
                                 internal::VarintType::kUnsigned);
  }

  // Reads repeated uint64 values from the current position into the vector,
  // supporting either repeated single field elements or packed encoding.
  Status ReadRepeatedUint64(pw::Vector<uint64_t>& out) {
    return ReadRepeatedVarintField<uint64_t>(out,
                                             internal::VarintType::kUnsigned);
  }

  // Reads a proto sint32 value from the current position.
  Result<int32_t> ReadSint32() {
    return ReadVarintField<int32_t>(internal::VarintType::kZigZag);
  }

  // Reads repeated sint32 values from the current position using packed
  // encoding.
  //
  // Returns the number of values read. In the case of error, the return value
  // indicates the number of values successfully read, in addition to the
  // error.
  StatusWithSize ReadPackedSint32(span<int32_t> out) {
    return ReadPackedVarintField(
        as_writable_bytes(out), sizeof(int32_t), internal::VarintType::kZigZag);
  }

  // Reads repeated sint32 values from the current position into the vector,
  // supporting either repeated single field elements or packed encoding.
  Status ReadRepeatedSint32(pw::Vector<int32_t>& out) {
    return ReadRepeatedVarintField<int32_t>(out, internal::VarintType::kZigZag);
  }

  // Reads a proto sint64 value from the current position.
  Result<int64_t> ReadSint64() {
    return ReadVarintField<int64_t>(internal::VarintType::kZigZag);
  }

  // Reads repeated int64 values from the current position using packed
  // encoding.
  //
  // Returns the number of values read. In the case of error, the return value
  // indicates the number of values successfully read, in addition to the
  // error.
  StatusWithSize ReadPackedSint64(span<int64_t> out) {
    return ReadPackedVarintField(
        as_writable_bytes(out), sizeof(int64_t), internal::VarintType::kZigZag);
  }

  // Reads repeated sint64 values from the current position into the vector,
  // supporting either repeated single field elements or packed encoding.
  Status ReadRepeatedSint64(pw::Vector<int64_t>& out) {
    return ReadRepeatedVarintField<int64_t>(out, internal::VarintType::kZigZag);
  }

  // Reads a proto bool value from the current position.
  Result<bool> ReadBool() {
    return ReadVarintField<bool>(internal::VarintType::kUnsigned);
  }

  // Reads repeated bool values from the current position using packed
  // encoding.
  //
  // Returns the number of values read. In the case of error, the return value
  // indicates the number of values successfully read, in addition to the
  // error.
  StatusWithSize ReadPackedBool(span<bool> out) {
    return ReadPackedVarintField(
        as_writable_bytes(out), sizeof(bool), internal::VarintType::kUnsigned);
  }

  // Reads repeated bool values from the current position into the vector,
  // supporting either repeated single field elements or packed encoding.
  Status ReadRepeatedBool(pw::Vector<bool>& out) {
    return ReadRepeatedVarintField<bool>(out, internal::VarintType::kUnsigned);
  }

  // Reads a proto fixed32 value from the current position.
  Result<uint32_t> ReadFixed32() { return ReadFixedField<uint32_t>(); }

  // Reads repeated fixed32 values from the current position using packed
  // encoding.
  //
  // Returns the number of values read.
  StatusWithSize ReadPackedFixed32(span<uint32_t> out) {
    return ReadPackedFixedField(as_writable_bytes(out), sizeof(uint32_t));
  }

  // Reads repeated fixed32 values from the current position into the vector,
  // supporting either repeated single field elements or packed encoding.
  Status ReadRepeatedFixed32(pw::Vector<uint32_t>& out) {
    return ReadRepeatedFixedField<uint32_t>(out);
  }

  // Reads a proto fixed64 value from the current position.
  Result<uint64_t> ReadFixed64() { return ReadFixedField<uint64_t>(); }

  // Reads repeated fixed64 values from the current position using packed
  // encoding.
  //
  // Returns the number of values read.
  StatusWithSize ReadPackedFixed64(span<uint64_t> out) {
    return ReadPackedFixedField(as_writable_bytes(out), sizeof(uint64_t));
  }

  // Reads repeated fixed64 values from the current position into the vector,
  // supporting either repeated single field elements or packed encoding.
  Status ReadRepeatedFixed64(pw::Vector<uint64_t>& out) {
    return ReadRepeatedFixedField<uint64_t>(out);
  }

  // Reads a proto sfixed32 value from the current position.
  Result<int32_t> ReadSfixed32() { return ReadFixedField<int32_t>(); }

  // Reads repeated sfixed32 values from the current position using packed
  // encoding.
  //
  // Returns the number of values read.
  StatusWithSize ReadPackedSfixed32(span<int32_t> out) {
    return ReadPackedFixedField(as_writable_bytes(out), sizeof(int32_t));
  }

  // Reads repeated sfixed32 values from the current position into the vector,
  // supporting either repeated single field elements or packed encoding.
  Status ReadRepeatedSfixed32(pw::Vector<int32_t>& out) {
    return ReadRepeatedFixedField<int32_t>(out);
  }

  // Reads a proto sfixed64 value from the current position.
  Result<int64_t> ReadSfixed64() { return ReadFixedField<int64_t>(); }

  // Reads repeated sfixed64 values from the current position using packed
  // encoding.
  //
  // Returns the number of values read.
  StatusWithSize ReadPackedSfixed64(span<int64_t> out) {
    return ReadPackedFixedField(as_writable_bytes(out), sizeof(int64_t));
  }

  // Reads repeated sfixed64 values from the current position into the vector,
  // supporting either repeated single field elements or packed encoding.
  Status ReadRepeatedSfixed64(pw::Vector<int64_t>& out) {
    return ReadRepeatedFixedField<int64_t>(out);
  }

  // Reads a proto float value from the current position.
  Result<float> ReadFloat() {
    static_assert(sizeof(float) == sizeof(uint32_t),
                  "Float and uint32_t must be the same size for protobufs");
    return ReadFixedField<float>();
  }

  // Reads repeated float values from the current position using packed
  // encoding.
  //
  // Returns the number of values read.
  StatusWithSize ReadPackedFloat(span<float> out) {
    static_assert(sizeof(float) == sizeof(uint32_t),
                  "Float and uint32_t must be the same size for protobufs");
    return ReadPackedFixedField(as_writable_bytes(out), sizeof(float));
  }

  // Reads repeated float values from the current position into the vector,
  // supporting either repeated single field elements or packed encoding.
  Status ReadRepeatedFloat(pw::Vector<float>& out) {
    return ReadRepeatedFixedField<float>(out);
  }

  // Reads a proto double value from the current position.
  Result<double> ReadDouble() {
    static_assert(sizeof(double) == sizeof(uint64_t),
                  "Double and uint64_t must be the same size for protobufs");
    return ReadFixedField<double>();
  }

  // Reads repeated double values from the current position using packed
  // encoding.
  //
  // Returns the number of values read.
  StatusWithSize ReadPackedDouble(span<double> out) {
    static_assert(sizeof(double) == sizeof(uint64_t),
                  "Double and uint64_t must be the same size for protobufs");
    return ReadPackedFixedField(as_writable_bytes(out), sizeof(double));
  }

  // Reads repeated double values from the current position into the vector,
  // supporting either repeated single field elements or packed encoding.
  Status ReadRepeatedDouble(pw::Vector<double>& out) {
    return ReadRepeatedFixedField<double>(out);
  }

  // Reads a proto string value from the current position. The string is
  // copied into the provided buffer and the read size is returned. The copied
  // string will NOT be null terminated; this should be done manually if
  // desired.
  //
  // If the buffer is too small to fit the string value, RESOURCE_EXHAUSTED is
  // returned and no data is read. The decoder's position remains on the
  // string field.
  StatusWithSize ReadString(span<char> out) {
    return ReadBytes(as_writable_bytes(out));
  }

  // Reads a proto bytes value from the current position. The value is copied
  // into the provided buffer and the read size is returned.
  //
  // If the buffer is too small to fit the bytes value, RESOURCE_EXHAUSTED is
  // returned and no data is read. The decoder's position remains on the bytes
  // field.
  //
  // For larger bytes values that won't fit into memory, use GetBytesReader()
  // to acquire a stream::Reader to the bytes instead.
  StatusWithSize ReadBytes(span<std::byte> out) {
    return ReadDelimitedField(out);
  }

  // Returns a stream::Reader to a bytes (or string) field at the current
  // position in the protobuf.
  //
  // The BytesReader shares the same stream as the decoder, using RAII to manage
  // ownership of the stream. The decoder cannot be used while the BytesStream
  // is alive.
  //
  //   StreamDecoder decoder(my_stream);
  //
  //   while (decoder.Next().ok()) {
  //     switch (decoder.FieldNumber()) {
  //
  //       // Bytes field.
  //       case 1: {
  //         // The BytesReader is created within a new C++ scope. While it is
  //         // alive, the decoder cannot be used.
  //         StreamDecoder::BytesReader reader = decoder.GetBytesReader();
  //
  //         // Do stuff with the reader.
  //         reader.Read(&some_buffer);
  //
  //         // At the end of the scope, the reader is destructed and the
  //         // decoder becomes usable again.
  //         break;
  //       }
  //     }
  //   }
  //
  // The returned decoder is seekable if the stream's decoder is seekable.
  BytesReader GetBytesReader();

  // Returns a decoder to a nested protobuf message located at the current
  // position.
  //
  // The nested decoder shares the same stream as its parent, using RAII to
  // manage ownership of the stream. The parent decoder cannot be used while the
  // nested one is alive.
  //
  // See the example in GetBytesReader() above for RAII semantics and usage.
  StreamDecoder GetNestedDecoder();

  struct Bounds {
    size_t low;
    size_t high;
  };

  // Get the interval of the payload part of a length-delimited field. That is,
  // the interval exluding the field key and the length prefix. The bounds are
  // relative to the given reader.
  Result<Bounds> GetLengthDelimitedPayloadBounds();

 protected:
  // Specialized move constructor used only for codegen.
  //
  // Postcondition: The other decoder is invalidated and cannot be used as it
  //     acts like a parent decoder with an active child decoder.
  constexpr StreamDecoder(StreamDecoder&& other)
      : reader_(other.reader_),
        stream_bounds_(other.stream_bounds_),
        position_(other.position_),
        current_field_(other.current_field_),
        delimited_field_size_(other.delimited_field_size_),
        delimited_field_offset_(other.delimited_field_offset_),
        parent_(other.parent_),
        field_consumed_(other.field_consumed_),
        nested_reader_open_(other.nested_reader_open_),
        status_(other.status_) {
    PW_ASSERT(!nested_reader_open_);
    // Make the nested decoder look like it has an open child to block reads for
    // the remainder of the object's life, and an invalid status to ensure it
    // doesn't advance the stream on destruction.
    other.nested_reader_open_ = true;
    other.parent_ = nullptr;
    other.status_ = pw::Status::Cancelled();
  }

  // Reads proto values from the stream and decodes them into the structure
  // contained within message according to the description of fields in table.
  //
  // This is called by codegen subclass Read() functions that accept a typed
  // struct Message reference, using the appropriate codegen MessageField table
  // corresponding to that type.
  Status Read(span<std::byte> message,
              span<const internal::MessageField> table);

 private:
  friend class BytesReader;

  // The FieldKey class can't store an invalid key, so pick a random large key
  // to set as the initial value. This will be overwritten the first time Next()
  // is called, and FieldKey() fails if Next() is not called first -- ensuring
  // that users will never see this value.
  static constexpr FieldKey kInitialFieldKey =
      FieldKey(20000, WireType::kVarint);

  constexpr StreamDecoder(stream::Reader& reader,
                          StreamDecoder* parent,
                          size_t low,
                          size_t high)
      : reader_(reader),
        stream_bounds_({low, high}),
        position_(parent->position_),
        current_field_(kInitialFieldKey),
        delimited_field_size_(0),
        delimited_field_offset_(0),
        parent_(parent),
        field_consumed_(true),
        nested_reader_open_(false),
        status_(OkStatus()) {}

  // Creates an unusable decoder in an error state. This is required as
  // GetNestedEncoder does not have a way to report an error in its API.
  constexpr StreamDecoder(stream::Reader& reader,
                          StreamDecoder* parent,
                          Status status)
      : reader_(reader),
        stream_bounds_({0, std::numeric_limits<size_t>::max()}),
        position_(0),
        current_field_(kInitialFieldKey),
        delimited_field_size_(0),
        delimited_field_offset_(0),
        parent_(parent),
        field_consumed_(true),
        nested_reader_open_(false),
        status_(status) {
    PW_ASSERT(!status.ok());
  }

  Status Advance(size_t end_position);

  size_t RemainingBytes() {
    return stream_bounds_.high < std::numeric_limits<size_t>::max()
               ? stream_bounds_.high - position_
               : std::numeric_limits<size_t>::max();
  }

  void CloseBytesReader(BytesReader& reader);
  void CloseNestedDecoder(StreamDecoder& nested);

  Status ReadFieldKey();
  Status SkipField();

  Status ReadVarintField(span<std::byte> out, internal::VarintType decode_type);

  StatusWithSize ReadOneVarint(span<std::byte> out,
                               internal::VarintType decode_type);

  template <typename T>
  Result<T> ReadVarintField(internal::VarintType decode_type) {
    static_assert(
        std::is_same_v<T, bool> || std::is_same_v<T, uint32_t> ||
            std::is_same_v<T, int32_t> || std::is_same_v<T, uint64_t> ||
            std::is_same_v<T, int64_t>,
        "Protobuf varints must be of type bool, uint32_t, int32_t, uint64_t, "
        "or int64_t");

    T result;
    if (Status status =
            ReadVarintField(as_writable_bytes(span(&result, 1)), decode_type);
        !status.ok()) {
      return status;
    }

    return result;
  }

  Status ReadFixedField(span<std::byte> out);

  template <typename T>
  Result<T> ReadFixedField() {
    static_assert(
        sizeof(T) == sizeof(uint32_t) || sizeof(T) == sizeof(uint64_t),
        "Protobuf fixed-size fields must be 32- or 64-bit");

    T result;
    if (Status status = ReadFixedField(as_writable_bytes(span(&result, 1)));
        !status.ok()) {
      return status;
    }

    return result;
  }

  StatusWithSize ReadDelimitedField(span<std::byte> out);

  StatusWithSize ReadPackedFixedField(span<std::byte> out, size_t elem_size);

  StatusWithSize ReadPackedVarintField(span<std::byte> out,
                                       size_t elem_size,
                                       internal::VarintType decode_type);

  template <typename T>
  Status ReadRepeatedFixedField(pw::Vector<T>& out) {
    if (out.full()) {
      return Status::ResourceExhausted();
    }
    const size_t old_size = out.size();
    if (current_field_.wire_type() == WireType::kDelimited) {
      out.resize(out.capacity());
      const auto sws = ReadPackedFixedField(
          as_writable_bytes(span(out.data() + old_size, out.size() - old_size)),
          sizeof(T));
      out.resize(old_size + sws.size());
      return sws.status();
    } else {
      out.resize(old_size + 1);
      const auto status = ReadFixedField(as_writable_bytes(
          span(out.data() + old_size, out.size() - old_size)));
      if (!status.ok()) {
        out.resize(old_size);
      }
      return status;
    }
  }

  template <typename T>
  Status ReadRepeatedVarintField(pw::Vector<T>& out,
                                 internal::VarintType decode_type) {
    if (out.full()) {
      return Status::ResourceExhausted();
    }
    const size_t old_size = out.size();
    if (current_field_.wire_type() == WireType::kDelimited) {
      out.resize(out.capacity());
      const auto sws = ReadPackedVarintField(
          as_writable_bytes(span(out.data() + old_size, out.size() - old_size)),
          sizeof(T),
          decode_type);
      out.resize(old_size + sws.size());
      return sws.status();
    } else {
      out.resize(old_size + 1);
      const auto status = ReadVarintField(
          as_writable_bytes(span(out.data() + old_size, out.size() - old_size)),
          decode_type);
      if (!status.ok()) {
        out.resize(old_size);
      }
      return status;
    }
  }

  template <typename Container>
  Status ReadStringOrBytesField(std::byte* raw_container) {
    auto& container = *reinterpret_cast<Container*>(raw_container);
    if (container.capacity() < delimited_field_size_) {
      return Status::ResourceExhausted();
    }
    container.resize(container.capacity());
    const auto sws = ReadDelimitedField(as_writable_bytes(span(container)));
    container.resize(sws.size());
    return sws.status();
  }

  Status CheckOkToRead(WireType type);

  stream::Reader& reader_;
  Bounds stream_bounds_;
  size_t position_;

  FieldKey current_field_;
  size_t delimited_field_size_;
  size_t delimited_field_offset_;

  StreamDecoder* parent_;

  bool field_consumed_;
  bool nested_reader_open_;

  Status status_;

  friend class Message;
};

}  // namespace pw::protobuf
