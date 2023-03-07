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

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <optional>

#include "pw_assert/check.h"
#include "pw_bytes/span.h"
#include "pw_protobuf/internal/codegen.h"
#include "pw_protobuf/serialized_size.h"
#include "pw_protobuf/stream_decoder.h"
#include "pw_protobuf/wire_format.h"
#include "pw_span/span.h"
#include "pw_status/status.h"
#include "pw_status/try.h"
#include "pw_stream/memory_stream.h"
#include "pw_stream/stream.h"
#include "pw_string/string.h"
#include "pw_varint/varint.h"

namespace pw::protobuf {

using internal::VarintType;

StreamEncoder StreamEncoder::GetNestedEncoder(uint32_t field_number,
                                              bool write_when_empty) {
  PW_CHECK(!nested_encoder_open());
  PW_CHECK(ValidFieldNumber(field_number));

  nested_field_number_ = field_number;

  // Pass the unused space of the scratch buffer to the nested encoder to use
  // as their scratch buffer.
  size_t key_size =
      varint::EncodedSize(FieldKey(field_number, WireType::kDelimited));
  size_t reserved_size = key_size + config::kMaxVarintSize;
  size_t max_size = std::min(memory_writer_.ConservativeWriteLimit(),
                             writer_.ConservativeWriteLimit());
  // Account for reserved bytes.
  max_size = max_size > reserved_size ? max_size - reserved_size : 0;
  // Cap based on max varint size.
  max_size = std::min(varint::MaxValueInBytes(config::kMaxVarintSize),
                      static_cast<uint64_t>(max_size));

  ByteSpan nested_buffer;
  if (max_size > 0) {
    nested_buffer = ByteSpan(
        memory_writer_.data() + reserved_size + memory_writer_.bytes_written(),
        max_size);
  } else {
    nested_buffer = ByteSpan();
  }
  return StreamEncoder(*this, nested_buffer, write_when_empty);
}

void StreamEncoder::CloseEncoder() {
  // If this was an invalidated StreamEncoder which cannot be used, permit the
  // object to be cleanly destructed by doing nothing.
  if (nested_field_number_ == kFirstReservedNumber) {
    return;
  }

  PW_CHECK(
      !nested_encoder_open(),
      "Tried to destruct a proto encoder with an active submessage encoder");

  if (parent_ != nullptr) {
    parent_->CloseNestedMessage(*this);
  }
}

void StreamEncoder::CloseNestedMessage(StreamEncoder& nested) {
  PW_DCHECK_PTR_EQ(nested.parent_,
                   this,
                   "CloseNestedMessage() called on the wrong Encoder parent");

  // Make the nested encoder look like it has an open child to block writes for
  // the remainder of the object's life.
  nested.nested_field_number_ = kFirstReservedNumber;
  nested.parent_ = nullptr;
  // Temporarily cache the field number of the child so we can re-enable
  // writing to this encoder.
  uint32_t temp_field_number = nested_field_number_;
  nested_field_number_ = 0;

  // TODO(amontanez): If a submessage fails, we could optionally discard
  // it and continue happily. For now, we'll always invalidate the entire
  // encoder if a single submessage fails.
  status_.Update(nested.status_);
  if (!status_.ok()) {
    return;
  }

  if (varint::EncodedSize(nested.memory_writer_.bytes_written()) >
      config::kMaxVarintSize) {
    status_ = Status::OutOfRange();
    return;
  }

  if (!nested.memory_writer_.bytes_written() && !nested.write_when_empty_) {
    return;
  }

  status_ = WriteLengthDelimitedField(temp_field_number,
                                      nested.memory_writer_.WrittenData());
}

Status StreamEncoder::WriteVarintField(uint32_t field_number, uint64_t value) {
  PW_TRY(UpdateStatusForWrite(
      field_number, WireType::kVarint, varint::EncodedSize(value)));

  WriteVarint(FieldKey(field_number, WireType::kVarint))
      .IgnoreError();  // TODO(b/242598609): Handle Status properly
  return WriteVarint(value);
}

Status StreamEncoder::WriteLengthDelimitedField(uint32_t field_number,
                                                ConstByteSpan data) {
  PW_TRY(UpdateStatusForWrite(field_number, WireType::kDelimited, data.size()));
  status_.Update(WriteLengthDelimitedKeyAndLengthPrefix(
      field_number, data.size(), writer_));
  PW_TRY(status_);
  if (Status status = writer_.Write(data); !status.ok()) {
    status_ = status;
  }
  return status_;
}

Status StreamEncoder::WriteLengthDelimitedFieldFromStream(
    uint32_t field_number,
    stream::Reader& bytes_reader,
    size_t num_bytes,
    ByteSpan stream_pipe_buffer) {
  PW_CHECK_UINT_GT(
      stream_pipe_buffer.size(), 0, "Transfer buffer cannot be 0 size");
  PW_TRY(UpdateStatusForWrite(field_number, WireType::kDelimited, num_bytes));
  status_.Update(
      WriteLengthDelimitedKeyAndLengthPrefix(field_number, num_bytes, writer_));
  PW_TRY(status_);

  // Stream data from `bytes_reader` to `writer_`.
  // TODO(pwbug/468): move the following logic to pw_stream/copy.h at a later
  // time.
  for (size_t bytes_written = 0; bytes_written < num_bytes;) {
    const size_t chunk_size_bytes =
        std::min(num_bytes - bytes_written, stream_pipe_buffer.size_bytes());
    const Result<ByteSpan> read_result =
        bytes_reader.Read(stream_pipe_buffer.data(), chunk_size_bytes);
    status_.Update(read_result.status());
    PW_TRY(status_);

    status_.Update(writer_.Write(read_result.value()));
    PW_TRY(status_);

    bytes_written += read_result.value().size();
  }

  return OkStatus();
}

Status StreamEncoder::WriteFixed(uint32_t field_number, ConstByteSpan data) {
  WireType type =
      data.size() == sizeof(uint32_t) ? WireType::kFixed32 : WireType::kFixed64;

  PW_TRY(UpdateStatusForWrite(field_number, type, data.size()));

  WriteVarint(FieldKey(field_number, type))
      .IgnoreError();  // TODO(b/242598609): Handle Status properly
  if (Status status = writer_.Write(data); !status.ok()) {
    status_ = status;
  }
  return status_;
}

Status StreamEncoder::WritePackedFixed(uint32_t field_number,
                                       span<const std::byte> values,
                                       size_t elem_size) {
  if (values.empty()) {
    return status_;
  }

  PW_CHECK_NOTNULL(values.data());
  PW_DCHECK(elem_size == sizeof(uint32_t) || elem_size == sizeof(uint64_t));

  PW_TRY(UpdateStatusForWrite(
      field_number, WireType::kDelimited, values.size_bytes()));
  WriteVarint(FieldKey(field_number, WireType::kDelimited))
      .IgnoreError();  // TODO(b/242598609): Handle Status properly
  WriteVarint(values.size_bytes())
      .IgnoreError();  // TODO(b/242598609): Handle Status properly

  for (auto val_start = values.begin(); val_start != values.end();
       val_start += elem_size) {
    // Allocates 8 bytes so both 4-byte and 8-byte types can be encoded as
    // little-endian for serialization.
    std::array<std::byte, sizeof(uint64_t)> data;
    if (endian::native == endian::little) {
      std::copy(val_start, val_start + elem_size, std::begin(data));
    } else {
      std::reverse_copy(val_start, val_start + elem_size, std::begin(data));
    }
    status_.Update(writer_.Write(span(data).first(elem_size)));
    PW_TRY(status_);
  }
  return status_;
}

Status StreamEncoder::UpdateStatusForWrite(uint32_t field_number,
                                           WireType type,
                                           size_t data_size) {
  PW_CHECK(!nested_encoder_open());
  PW_TRY(status_);

  if (!ValidFieldNumber(field_number)) {
    return status_ = Status::InvalidArgument();
  }

  const Result<size_t> field_size = SizeOfField(field_number, type, data_size);
  status_.Update(field_size.status());
  PW_TRY(status_);

  if (field_size.value() > writer_.ConservativeWriteLimit()) {
    status_ = Status::ResourceExhausted();
  }

  return status_;
}

Status StreamEncoder::Write(span<const std::byte> message,
                            span<const internal::MessageField> table) {
  PW_CHECK(!nested_encoder_open());
  PW_TRY(status_);

  for (const auto& field : table) {
    // Calculate the span of bytes corresponding to the structure field to
    // read from.
    const auto values =
        message.subspan(field.field_offset(), field.field_size());
    PW_CHECK(values.begin() >= message.begin() &&
             values.end() <= message.end());

    // If the field is using callbacks, interpret the input field accordingly
    // and allow the caller to provide custom handling.
    if (field.use_callback()) {
      const Callback<StreamEncoder, StreamDecoder>* callback =
          reinterpret_cast<const Callback<StreamEncoder, StreamDecoder>*>(
              values.data());
      PW_TRY(callback->Encode(*this));
      continue;
    }

    switch (field.wire_type()) {
      case WireType::kFixed64:
      case WireType::kFixed32: {
        // Fixed fields call WriteFixed() for singular case and
        // WritePackedFixed() for repeated fields.
        PW_CHECK(field.elem_size() == (field.wire_type() == WireType::kFixed32
                                           ? sizeof(uint32_t)
                                           : sizeof(uint64_t)),
                 "Mismatched message field type and size");
        if (field.is_fixed_size()) {
          PW_CHECK(field.is_repeated(), "Non-repeated fixed size field");
          if (static_cast<size_t>(
                  std::count(values.begin(), values.end(), std::byte{0})) <
              values.size()) {
            PW_TRY(WritePackedFixed(
                field.field_number(), values, field.elem_size()));
          }
        } else if (field.is_repeated()) {
          // The struct member for this field is a vector of a type
          // corresponding to the field element size. Cast to the correct
          // vector type so we're not performing type aliasing (except for
          // unsigned vs signed which is explicitly allowed).
          if (field.elem_size() == sizeof(uint64_t)) {
            const auto* vector =
                reinterpret_cast<const pw::Vector<const uint64_t>*>(
                    values.data());
            if (!vector->empty()) {
              PW_TRY(WritePackedFixed(
                  field.field_number(),
                  as_bytes(span(vector->data(), vector->size())),
                  field.elem_size()));
            }
          } else if (field.elem_size() == sizeof(uint32_t)) {
            const auto* vector =
                reinterpret_cast<const pw::Vector<const uint32_t>*>(
                    values.data());
            if (!vector->empty()) {
              PW_TRY(WritePackedFixed(
                  field.field_number(),
                  as_bytes(span(vector->data(), vector->size())),
                  field.elem_size()));
            }
          }
        } else if (field.is_optional()) {
          // The struct member for this field is a std::optional of a type
          // corresponding to the field element size. Cast to the correct
          // optional type so we're not performing type aliasing (except for
          // unsigned vs signed which is explicitly allowed), and write from
          // a temporary.
          if (field.elem_size() == sizeof(uint64_t)) {
            const auto* optional =
                reinterpret_cast<const std::optional<uint64_t>*>(values.data());
            if (optional->has_value()) {
              uint64_t value = optional->value();
              PW_TRY(
                  WriteFixed(field.field_number(), as_bytes(span(&value, 1))));
            }
          } else if (field.elem_size() == sizeof(uint32_t)) {
            const auto* optional =
                reinterpret_cast<const std::optional<uint32_t>*>(values.data());
            if (optional->has_value()) {
              uint32_t value = optional->value();
              PW_TRY(
                  WriteFixed(field.field_number(), as_bytes(span(&value, 1))));
            }
          }
        } else {
          PW_CHECK(values.size() == field.elem_size(),
                   "Mismatched message field type and size");
          if (static_cast<size_t>(
                  std::count(values.begin(), values.end(), std::byte{0})) <
              values.size()) {
            PW_TRY(WriteFixed(field.field_number(), values));
          }
        }
        break;
      }
      case WireType::kVarint: {
        // Varint fields call WriteVarintField() for singular case and
        // WritePackedVarints() for repeated fields.
        PW_CHECK(field.elem_size() == sizeof(uint64_t) ||
                     field.elem_size() == sizeof(uint32_t) ||
                     field.elem_size() == sizeof(bool),
                 "Mismatched message field type and size");
        if (field.is_fixed_size()) {
          // The struct member for this field is an array of type corresponding
          // to the field element size. Cast to a span of the correct type over
          // the array so we're not performing type aliasing (except for
          // unsigned vs signed which is explicitly allowed).
          PW_CHECK(field.is_repeated(), "Non-repeated fixed size field");
          if (static_cast<size_t>(
                  std::count(values.begin(), values.end(), std::byte{0})) ==
              values.size()) {
            continue;
          }
          if (field.elem_size() == sizeof(uint64_t)) {
            PW_TRY(WritePackedVarints(
                field.field_number(),
                span(reinterpret_cast<const uint64_t*>(values.data()),
                     values.size() / field.elem_size()),
                field.varint_type()));
          } else if (field.elem_size() == sizeof(uint32_t)) {
            PW_TRY(WritePackedVarints(
                field.field_number(),
                span(reinterpret_cast<const uint32_t*>(values.data()),
                     values.size() / field.elem_size()),
                field.varint_type()));
          } else if (field.elem_size() == sizeof(bool)) {
            static_assert(sizeof(bool) == sizeof(uint8_t),
                          "bool must be same size as uint8_t");
            PW_TRY(WritePackedVarints(
                field.field_number(),
                span(reinterpret_cast<const uint8_t*>(values.data()),
                     values.size() / field.elem_size()),
                field.varint_type()));
          }
        } else if (field.is_repeated()) {
          // The struct member for this field is a vector of a type
          // corresponding to the field element size. Cast to the correct
          // vector type so we're not performing type aliasing (except for
          // unsigned vs signed which is explicitly allowed).
          if (field.elem_size() == sizeof(uint64_t)) {
            const auto* vector =
                reinterpret_cast<const pw::Vector<const uint64_t>*>(
                    values.data());
            if (!vector->empty()) {
              PW_TRY(WritePackedVarints(field.field_number(),
                                        span(vector->data(), vector->size()),
                                        field.varint_type()));
            }
          } else if (field.elem_size() == sizeof(uint32_t)) {
            const auto* vector =
                reinterpret_cast<const pw::Vector<const uint32_t>*>(
                    values.data());
            if (!vector->empty()) {
              PW_TRY(WritePackedVarints(field.field_number(),
                                        span(vector->data(), vector->size()),
                                        field.varint_type()));
            }
          } else if (field.elem_size() == sizeof(bool)) {
            static_assert(sizeof(bool) == sizeof(uint8_t),
                          "bool must be same size as uint8_t");
            const auto* vector =
                reinterpret_cast<const pw::Vector<const uint8_t>*>(
                    values.data());
            if (!vector->empty()) {
              PW_TRY(WritePackedVarints(field.field_number(),
                                        span(vector->data(), vector->size()),
                                        field.varint_type()));
            }
          }
        } else if (field.is_optional()) {
          // The struct member for this field is a std::optional of a type
          // corresponding to the field element size. Cast to the correct
          // optional type so we're not performing type aliasing (except for
          // unsigned vs signed which is explicitly allowed), and write from
          // a temporary.
          uint64_t value = 0;
          if (field.elem_size() == sizeof(uint64_t)) {
            if (field.varint_type() == VarintType::kUnsigned) {
              const auto* optional =
                  reinterpret_cast<const std::optional<uint64_t>*>(
                      values.data());
              if (!optional->has_value()) {
                continue;
              }
              value = optional->value();
            } else {
              const auto* optional =
                  reinterpret_cast<const std::optional<int64_t>*>(
                      values.data());
              if (!optional->has_value()) {
                continue;
              }
              value = field.varint_type() == VarintType::kZigZag
                          ? varint::ZigZagEncode(optional->value())
                          : optional->value();
            }
          } else if (field.elem_size() == sizeof(uint32_t)) {
            if (field.varint_type() == VarintType::kUnsigned) {
              const auto* optional =
                  reinterpret_cast<const std::optional<uint32_t>*>(
                      values.data());
              if (!optional->has_value()) {
                continue;
              }
              value = optional->value();
            } else {
              const auto* optional =
                  reinterpret_cast<const std::optional<int32_t>*>(
                      values.data());
              if (!optional->has_value()) {
                continue;
              }
              value = field.varint_type() == VarintType::kZigZag
                          ? varint::ZigZagEncode(optional->value())
                          : optional->value();
            }
          } else if (field.elem_size() == sizeof(bool)) {
            const auto* optional =
                reinterpret_cast<const std::optional<bool>*>(values.data());
            if (!optional->has_value()) {
              continue;
            }
            value = optional->value();
          }
          PW_TRY(WriteVarintField(field.field_number(), value));
        } else {
          // The struct member for this field is a scalar of a type
          // corresponding to the field element size. Cast to the correct
          // type to retrieve the value before passing to WriteVarintField()
          // so we're not performing type aliasing (except for unsigned vs
          // signed which is explicitly allowed).
          PW_CHECK(values.size() == field.elem_size(),
                   "Mismatched message field type and size");
          uint64_t value = 0;
          if (field.elem_size() == sizeof(uint64_t)) {
            if (field.varint_type() == VarintType::kZigZag) {
              value = varint::ZigZagEncode(
                  *reinterpret_cast<const int64_t*>(values.data()));
            } else if (field.varint_type() == VarintType::kNormal) {
              value = *reinterpret_cast<const int64_t*>(values.data());
            } else {
              value = *reinterpret_cast<const uint64_t*>(values.data());
            }
            if (!value) {
              continue;
            }
          } else if (field.elem_size() == sizeof(uint32_t)) {
            if (field.varint_type() == VarintType::kZigZag) {
              value = varint::ZigZagEncode(
                  *reinterpret_cast<const int32_t*>(values.data()));
            } else if (field.varint_type() == VarintType::kNormal) {
              value = *reinterpret_cast<const int32_t*>(values.data());
            } else {
              value = *reinterpret_cast<const uint32_t*>(values.data());
            }
            if (!value) {
              continue;
            }
          } else if (field.elem_size() == sizeof(bool)) {
            value = *reinterpret_cast<const bool*>(values.data());
            if (!value) {
              continue;
            }
          }
          PW_TRY(WriteVarintField(field.field_number(), value));
        }
        break;
      }
      case WireType::kDelimited: {
        // Delimited fields are always a singular case because of the
        // inability to cast to a generic vector with an element of a certain
        // size (we always need a type).
        PW_CHECK(!field.is_repeated(),
                 "Repeated delimited messages always require a callback");
        if (field.nested_message_fields()) {
          // Nested Message. Struct member is an embedded struct for the
          // nested field. Obtain a nested encoder and recursively call Write()
          // using the fields table pointer from this field.
          auto nested_encoder = GetNestedEncoder(field.field_number(),
                                                 /*write_when_empty=*/false);
          PW_TRY(nested_encoder.Write(values, *field.nested_message_fields()));
        } else if (field.is_fixed_size()) {
          // Fixed-length bytes field. Struct member is a std::array<std::byte>.
          // Call WriteLengthDelimitedField() to output it to the stream.
          PW_CHECK(field.elem_size() == sizeof(std::byte),
                   "Mismatched message field type and size");
          if (static_cast<size_t>(
                  std::count(values.begin(), values.end(), std::byte{0})) <
              values.size()) {
            PW_TRY(WriteLengthDelimitedField(field.field_number(), values));
          }
        } else {
          // bytes or string field with a maximum size. Struct member is
          // pw::Vector<std::byte> for bytes or pw::InlineString<> for string.
          // Use the contents as a span and call WriteLengthDelimitedField() to
          // output it to the stream.
          PW_CHECK(field.elem_size() == sizeof(std::byte),
                   "Mismatched message field type and size");
          if (field.is_string()) {
            PW_TRY(WriteStringOrBytes<const InlineString<>>(
                field.field_number(), values.data()));
          } else {
            PW_TRY(WriteStringOrBytes<const Vector<const std::byte>>(
                field.field_number(), values.data()));
          }
        }
        break;
      }
    }
  }

  return status_;
}

}  // namespace pw::protobuf
