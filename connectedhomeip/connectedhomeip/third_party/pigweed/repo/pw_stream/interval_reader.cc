// Copyright 2020 The Pigweed Authors
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

#include "pw_stream/interval_reader.h"

#include "pw_assert/check.h"

namespace pw::stream {

StatusWithSize IntervalReader::DoRead(ByteSpan destination) {
  if (!source_reader_) {
    return StatusWithSize(Status::FailedPrecondition(), 0);
  }

  if (!status_.ok()) {
    return StatusWithSize(status_, 0);
  }

  if (current_ == end_) {
    return StatusWithSize::OutOfRange();
  }

  // Seek the source reader to the `current_` offset of this IntervalReader
  // before reading.
  Status status = source_reader_->Seek(current_, Whence::kBeginning);
  if (!status.ok()) {
    return StatusWithSize(status, 0);
  }

  size_t to_read = std::min(destination.size(), end_ - current_);
  Result<ByteSpan> res = source_reader_->Read(destination.first(to_read));
  if (!res.ok()) {
    return StatusWithSize(res.status(), 0);
  }

  current_ += res.value().size();
  return StatusWithSize(res.value().size());
}

Status IntervalReader::DoSeek(ptrdiff_t offset, Whence origin) {
  ptrdiff_t absolute_position = std::numeric_limits<ptrdiff_t>::min();

  // Convert from the position within the interval to the position within the
  // source reader stream.
  switch (origin) {
    case Whence::kBeginning:
      absolute_position = offset + start_;
      break;

    case Whence::kCurrent:
      absolute_position = current_ + offset;
      break;

    case Whence::kEnd:
      absolute_position = end_ + offset;
      break;
  }

  if (absolute_position < 0) {
    return Status::InvalidArgument();
  }

  if (static_cast<size_t>(absolute_position) < start_ ||
      static_cast<size_t>(absolute_position) > end_) {
    return Status::InvalidArgument();
  }

  current_ = absolute_position;
  return OkStatus();
}

}  // namespace pw::stream
