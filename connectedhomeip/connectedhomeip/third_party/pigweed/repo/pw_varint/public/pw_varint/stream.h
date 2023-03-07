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

#include <cstdint>
#include <limits>

#include "pw_status/status_with_size.h"
#include "pw_stream/stream.h"

namespace pw {
namespace varint {

// Reads a varint-encoded value from a pw::stream. If reading into a signed
// integer, the value is ZigZag decoded.
//
// Returns the number of bytes read from the stream if successful, OutOfRange
// if the varint does not fit in a int64_t / uint64_t or if the input is
// exhausted before the number terminates. Reads a maximum of 10 bytes or
// max_size, whichever is smaller.
StatusWithSize Read(stream::Reader& reader,
                    int64_t* output,
                    size_t max_size = std::numeric_limits<size_t>::max());
StatusWithSize Read(stream::Reader& reader,
                    uint64_t* output,
                    size_t max_size = std::numeric_limits<size_t>::max());

}  // namespace varint
}  // namespace pw
