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

// Defines functions for implementing seeking in a stream.
#pragma once

#include <cstddef>

#include "pw_stream/stream.h"

namespace pw::stream {

// Adds a seek offset to the specified origin.
constexpr ptrdiff_t ResolveSeekOffset(ptrdiff_t offset,
                                      Stream::Whence origin,
                                      size_t end_position,
                                      size_t current_position) {
  switch (origin) {
    case Stream::kBeginning:
      return offset;
    case Stream::kCurrent:
      return static_cast<ptrdiff_t>(current_position) + offset;
    case Stream::kEnd:
    default:
      return static_cast<ptrdiff_t>(end_position) + offset;
  }
}

// Implements seek for a class that supports absolute position changes. The
// new position is calculated and assigned to the provided position variable.
//
// Returns OUT_OF_RANGE for seeks to a negative position or past the end.
constexpr Status CalculateSeek(ptrdiff_t offset,
                               Stream::Whence origin,
                               size_t end_position,
                               size_t& current_position) {
  const ptrdiff_t new_position =
      ResolveSeekOffset(offset, origin, end_position, current_position);

  if (new_position < 0 || static_cast<size_t>(new_position) > end_position) {
    return Status::OutOfRange();
  }

  current_position = static_cast<size_t>(new_position);
  return OkStatus();
}

}  // namespace pw::stream
