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

#include <cstdint>

#include "pw_function/function.h"
#include "pw_protobuf/wire_format.h"
#include "pw_span/span.h"
#include "pw_status/status.h"

namespace pw::protobuf {
namespace internal {

// Varints can be encoded as an unsigned type, a signed type with normal
// encoding, or a signed type with zigzag encoding.
enum class VarintType {
  kUnsigned = 0,
  kNormal = 1,
  kZigZag = 2,
};

// Represents a field in a code generated message struct that can be the target
// for decoding or source of encoding.
//
// An instance of this class exists for every field in every protobuf in the
// binary, thus it is size critical to ensure efficiency while retaining enough
// information to describe the layout of the generated message struct.
//
// Limitations imposed:
//  - Element size of a repeated fields must be no larger than 15 bytes.
//    (8 byte int64/fixed64/double is the largest supported element).
//  - Individual field size (including repeated and nested messages) must be no
//    larger than 64 KB. (This is already the maximum size of pw::Vector).
//
// A complete codegen struct is represented by a span<MessageField>,
// holding a pointer to the MessageField members themselves, and the number of
// fields in the struct. These spans are global data, one span per protobuf
// message (including the size), and one MessageField per field in the message.
//
// Nested messages are handled with a pointer from the MessageField in the
// parent to a pointer to the (global data) span. Since the size of the nested
// message is stored as part of the global span, the cost of a nested message
// is only the size of a pointer to that span.
class MessageField {
 public:
  static constexpr unsigned int kMaxFieldSize = (1u << 16) - 1;

  constexpr MessageField(uint32_t field_number,
                         WireType wire_type,
                         size_t elem_size,
                         VarintType varint_type,
                         bool is_string,
                         bool is_fixed_size,
                         bool is_repeated,
                         bool is_optional,
                         bool use_callback,
                         size_t field_offset,
                         size_t field_size,
                         const span<const MessageField>* nested_message_fields)
      : field_number_(field_number),
        field_info_(
            static_cast<unsigned int>(wire_type) << kWireTypeShift |
            elem_size << kElemSizeShift |
            static_cast<unsigned int>(varint_type) << kVarintTypeShift |
            is_string << kIsStringShift | is_fixed_size << kIsFixedSizeShift |
            is_repeated << kIsRepeatedShift | is_optional << kIsOptionalShift |
            use_callback << kUseCallbackShift | field_size << kFieldSizeShift),
        field_offset_(field_offset),
        nested_message_fields_(nested_message_fields) {}

  constexpr uint32_t field_number() const { return field_number_; }
  constexpr WireType wire_type() const {
    return static_cast<WireType>((field_info_ >> kWireTypeShift) &
                                 kWireTypeMask);
  }
  constexpr size_t elem_size() const {
    return (field_info_ >> kElemSizeShift) & kElemSizeMask;
  }
  constexpr VarintType varint_type() const {
    return static_cast<VarintType>((field_info_ >> kVarintTypeShift) &
                                   kVarintTypeMask);
  }
  constexpr bool is_string() const {
    return (field_info_ >> kIsStringShift) & 1;
  }
  constexpr bool is_fixed_size() const {
    return (field_info_ >> kIsFixedSizeShift) & 1;
  }
  constexpr bool is_repeated() const {
    return (field_info_ >> kIsRepeatedShift) & 1;
  }
  constexpr bool is_optional() const {
    return (field_info_ >> kIsOptionalShift) & 1;
  }
  constexpr bool use_callback() const {
    return (field_info_ >> kUseCallbackShift) & 1;
  }
  constexpr size_t field_offset() const { return field_offset_; }
  constexpr size_t field_size() const {
    return (field_info_ >> kFieldSizeShift) & kFieldSizeMask;
  }
  constexpr const span<const MessageField>* nested_message_fields() const {
    return nested_message_fields_;
  }

  constexpr bool operator==(uint32_t field_number) const {
    return field_number == field_number_;
  }

 private:
  // field_info_ packs multiple fields into a single word as follows:
  //
  //   wire_type      : 3
  //   varint_type    : 2
  //   is_string      : 1
  //   is_fixed_size  : 1
  //   is_repeated    : 1
  //   use_callback   : 1
  //   -
  //   elem_size      : 4
  //   is_optional    : 1
  //   [unused space] : 2
  //   -
  //   field_size     : 16
  //
  // The protobuf field type is spread among a few fields (wire_type,
  // varint_type, is_string, elem_size). The exact field type (e.g. int32, bool,
  // message, etc.), from which all of that information can be derived, can be
  // represented in 4 bits. If more bits are needed in the future, these could
  // be consolidated into a single field type enum.
  static constexpr unsigned int kWireTypeShift = 29u;
  static constexpr unsigned int kWireTypeMask = (1u << 3) - 1;
  static constexpr unsigned int kVarintTypeShift = 27u;
  static constexpr unsigned int kVarintTypeMask = (1u << 2) - 1;
  static constexpr unsigned int kIsStringShift = 26u;
  static constexpr unsigned int kIsFixedSizeShift = 25u;
  static constexpr unsigned int kIsRepeatedShift = 24u;
  static constexpr unsigned int kUseCallbackShift = 23u;
  static constexpr unsigned int kElemSizeShift = 19u;
  static constexpr unsigned int kElemSizeMask = (1u << 4) - 1;
  static constexpr unsigned int kIsOptionalShift = 16u;
  static constexpr unsigned int kFieldSizeShift = 0u;
  static constexpr unsigned int kFieldSizeMask = kMaxFieldSize;

  uint32_t field_number_;
  uint32_t field_info_;
  size_t field_offset_;
  // TODO(b/234875722): Could be replaced by a class MessageDescriptor*
  const span<const MessageField>* nested_message_fields_;
};
static_assert(sizeof(MessageField) <= sizeof(size_t) * 4,
              "MessageField should be four words or less");

}  // namespace internal

// Callback for a structure member that cannot be represented by a data type.
// Holds either a callback for encoding a field, or a callback for decoding
// a field.
template <typename StreamEncoder, typename StreamDecoder>
union Callback {
  constexpr Callback() : encode_() {}
  ~Callback() { encode_ = nullptr; }

  // Set the encoder callback.
  void SetEncoder(Function<Status(StreamEncoder& encoder)>&& encode) {
    encode_ = std::move(encode);
  }

  // Set the decoder callback.
  void SetDecoder(Function<Status(StreamDecoder& decoder)>&& decode) {
    decode_ = std::move(decode);
  }

  // Allow moving of callbacks by moving the member.
  constexpr Callback(Callback&& other) = default;
  constexpr Callback& operator=(Callback&& other) = default;

  // Copying a callback does not copy the functions.
  constexpr Callback(const Callback&) : encode_() {}
  constexpr Callback& operator=(const Callback&) {
    encode_ = nullptr;
    return *this;
  }

 private:
  friend StreamDecoder;
  friend StreamEncoder;

  // Called by StreamEncoder to encode the structure member.
  // Returns OkStatus() if this has not been set by the caller, the default
  // behavior of a field without an encoder is the same as default-initialized
  // field.
  Status Encode(StreamEncoder& encoder) const {
    if (encode_) {
      return encode_(encoder);
    }
    return OkStatus();
  }

  // Called by StreamDecoder to decode the structure member when the field
  // is present. Returns DataLoss() if this has not been set by the caller.
  Status Decode(StreamDecoder& decoder) const {
    if (decode_) {
      return decode_(decoder);
    }
    return Status::DataLoss();
  }

  Function<Status(StreamEncoder& encoder)> encode_;
  Function<Status(StreamDecoder& decoder)> decode_;
};

}  // namespace pw::protobuf
