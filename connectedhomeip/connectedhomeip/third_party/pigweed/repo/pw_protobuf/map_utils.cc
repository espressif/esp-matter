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

#include <cstddef>

#include "pw_bytes/span.h"
#include "pw_protobuf/encoder.h"
#include "pw_protobuf/serialized_size.h"
#include "pw_stream/stream.h"

namespace pw::protobuf {

// Note that a map<string, bytes> is essentially
//
// message Entry {
//   string key = 1;
//   bytes value = 2;
// }
//
// message Msg {
//   repeated Entry map_field = <field_number>;
// }
Status WriteProtoStringToBytesMapEntry(uint32_t field_number,
                                       stream::Reader& key,
                                       size_t key_size,
                                       stream::Reader& value,
                                       size_t value_size,
                                       ByteSpan stream_pipe_buffer,
                                       stream::Writer& writer) {
  constexpr uint32_t kMapKeyFieldNumber = 1;
  constexpr uint32_t kMapValueFieldNumber = 2;

  if (!protobuf::ValidFieldNumber(field_number) ||
      key_size >= std::numeric_limits<uint32_t>::max() ||
      value_size >= std::numeric_limits<uint32_t>::max()) {
    return Status::InvalidArgument();
  }

  Result<size_t> key_field_size = protobuf::SizeOfField(
      kMapKeyFieldNumber, protobuf::WireType::kDelimited, key_size);
  PW_TRY(key_field_size.status());

  Result<size_t> value_field_size = protobuf::SizeOfField(
      kMapValueFieldNumber, protobuf::WireType::kDelimited, value_size);
  PW_TRY(value_field_size.status());

  size_t entry_payload_total_size =
      key_field_size.value() + value_field_size.value();

  Result<size_t> entry_field_total_size = protobuf::SizeOfField(
      field_number, protobuf::WireType::kDelimited, entry_payload_total_size);
  PW_TRY(entry_field_total_size.status());

  if (entry_field_total_size.value() > writer.ConservativeWriteLimit()) {
    return Status::ResourceExhausted();
  }

  // Write field key and length prefix for nested message `Entry`
  PW_TRY(protobuf::WriteLengthDelimitedKeyAndLengthPrefix(
      field_number, entry_payload_total_size, writer));

  protobuf::StreamEncoder encoder(writer, {});

  // Write Entry::key
  PW_TRY(encoder.WriteStringFromStream(
      kMapKeyFieldNumber, key, key_size, stream_pipe_buffer));
  // Write Entry::value
  PW_TRY(encoder.WriteBytesFromStream(
      kMapValueFieldNumber, value, value_size, stream_pipe_buffer));

  return OkStatus();
}

}  // namespace pw::protobuf
