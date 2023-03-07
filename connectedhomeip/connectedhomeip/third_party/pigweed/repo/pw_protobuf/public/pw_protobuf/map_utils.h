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
//
// The header provides a set of helper utils for protobuf related operations.
// The APIs may not be finalized yet.

#pragma once

#include <cstddef>
#include <string_view>

#include "pw_assert/check.h"
#include "pw_protobuf/stream_decoder.h"
#include "pw_status/status.h"
#include "pw_status/try.h"
#include "pw_stream/stream.h"

namespace pw::protobuf {

// The function writes an entry for the proto map<string, bytes> field type.
//
// Args:
//   field_number - The field number for the map.
//   key - The string payload for the key value of the entry.
//   key_size - Number of bytes in the key.
//   value - The value payload for the entry.
//   value_size - Number of bytes in the value.
//   stream_pipe_buffer - A non-zero size buffer for the function to read and
//     store data from the reader and write to the given writer.
//   writer - The output writer to write to.
//
// Returns:
// OK - Entry is successfully written.
// RESOURCE_EXHAUSTED - Entry would exceed write limit.
// INVALID_ARGUMENT - Field number is invalid.
//
// Since all length-delimited fields can be treated as `bytes`,
// it can be used to write any string to length-delimited field map, i.e.
// map<string, message>, map<string, bytes> etc.
Status WriteProtoStringToBytesMapEntry(uint32_t field_number,
                                       stream::Reader& key,
                                       size_t key_size,
                                       stream::Reader& value,
                                       size_t value_size,
                                       ByteSpan stream_pipe_buffer,
                                       stream::Writer& writer);

}  // namespace pw::protobuf
