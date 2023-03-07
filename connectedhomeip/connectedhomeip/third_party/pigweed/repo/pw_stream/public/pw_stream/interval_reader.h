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

#include "pw_assert/assert.h"
#include "pw_status/status.h"
#include "pw_stream/stream.h"

namespace pw::stream {

// A reader wrapper that reads from a sub-interval of a given seekable
// source reader. The IntervalReader tracks and maintains its own read offset.
// It seeks the source reader to its current read offset before reading. In
// this way, multiple IntervalReaders can share the same source reader without
// interfering with each other.
class IntervalReader : public SeekableReader {
 public:
  constexpr IntervalReader() : status_(Status::Unavailable()) {}

  // Create an IntervalReader with an error status.
  constexpr IntervalReader(Status status) : status_(status) {
    PW_ASSERT(!status.ok());
  }

  // source_reader -- The source reader to read from.
  // start -- starting offset to read in `source_reader`
  // end -- ending offset in `source_reader`.
  constexpr IntervalReader(SeekableReader& source_reader,
                           size_t start,
                           size_t end)
      : source_reader_(&source_reader),
        start_(start),
        end_(end),
        current_(start) {}

  // Reset the read offset to the start of the interval
  IntervalReader& Reset() {
    current_ = start_;
    return *this;
  }

  // Move the read offset to the end of the interval;
  IntervalReader& Exhaust() {
    current_ = end_;
    return *this;
  }

  // Get a reference to the source reader.
  SeekableReader& source_reader() { return *source_reader_; }
  size_t start() const { return start_; }
  size_t end() const { return end_; }
  size_t current() const { return current_; }
  size_t interval_size() const { return end_ - start_; }
  bool ok() const { return status_.ok(); }
  Status status() const { return status_; }

  // For iterator comparison in Message.
  bool operator==(const IntervalReader& other) const {
    return source_reader_ == other.source_reader_ && start_ == other.start_ &&
           end_ == other.end_ && current_ == other.current_;
  }

 private:
  StatusWithSize DoRead(ByteSpan destination) final;
  Status DoSeek(ptrdiff_t offset, Whence origin) final;
  size_t DoTell() final { return current_ - start_; }
  size_t ConservativeLimit(LimitType limit) const override {
    if (limit == LimitType::kRead) {
      return end_ - current_;
    }
    return 0;
  }

  SeekableReader* source_reader_ = nullptr;
  size_t start_ = 0;
  size_t end_ = 0;
  size_t current_ = 0;
  Status status_ = OkStatus();
};

}  // namespace pw::stream
