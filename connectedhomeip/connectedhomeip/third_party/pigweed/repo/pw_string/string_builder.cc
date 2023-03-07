// Copyright 2019 The Pigweed Authors
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

#include "pw_string/string_builder.h"

#include <cstdio>

#include "pw_string/format.h"
#include "pw_string/util.h"

namespace pw {

void StringBuilder::clear() {
  *size_ = 0;
  NullTerminate();
  status_ = StatusCode(OkStatus());
  last_status_ = StatusCode(OkStatus());
}

StringBuilder& StringBuilder::append(size_t count, char ch) {
  char* const append_destination = buffer_.data() + size();
  std::fill_n(append_destination, ResizeAndTerminate(count), ch);
  return *this;
}

StringBuilder& StringBuilder::append(const char* str, size_t count) {
  char* const append_destination = buffer_.data() + size();
  std::copy_n(str, ResizeAndTerminate(count), append_destination);
  return *this;
}

StringBuilder& StringBuilder::append(const char* str) {
  // Use buffer_.size() - size() as the maximum length so that strings too long
  // to fit in the buffer will request one character too many, which sets the
  // status to RESOURCE_EXHAUSTED.
  return append(string::ClampedCString(str, buffer_.size() - size()));
}

StringBuilder& StringBuilder::append(const std::string_view& str) {
  return append(str.data(), str.size());
}

StringBuilder& StringBuilder::append(const std::string_view& str,
                                     size_t pos,
                                     size_t count) {
  if (pos > str.size()) {
    SetErrorStatus(Status::OutOfRange());
    return *this;
  }

  return append(str.data() + pos, std::min(str.size() - pos, count));
}

size_t StringBuilder::ResizeAndTerminate(size_t chars_to_append) {
  const size_t copied = std::min(chars_to_append, max_size() - size());
  *size_ += copied;
  NullTerminate();

  if (buffer_.empty() || chars_to_append != copied) {
    SetErrorStatus(Status::ResourceExhausted());
  } else {
    last_status_ = StatusCode(OkStatus());
  }
  return copied;
}

void StringBuilder::resize(size_t new_size) {
  if (new_size <= size()) {
    *size_ = static_cast<InlineString<>::size_type>(new_size);
    NullTerminate();
    last_status_ = StatusCode(OkStatus());
  } else {
    SetErrorStatus(Status::OutOfRange());
  }
}

StringBuilder& StringBuilder::Format(const char* format, ...) {
  va_list args;
  va_start(args, format);
  FormatVaList(format, args);
  va_end(args);

  return *this;
}

StringBuilder& StringBuilder::FormatVaList(const char* format, va_list args) {
  HandleStatusWithSize(
      string::FormatVaList(buffer_.subspan(size()), format, args));
  return *this;
}

void StringBuilder::WriteBytes(span<const std::byte> data) {
  if (size() + data.size() * 2 > max_size()) {
    SetErrorStatus(Status::ResourceExhausted());
  } else {
    for (std::byte val : data) {
      *this << val;
    }
  }
}

void StringBuilder::CopySizeAndStatus(const StringBuilder& other) {
  *size_ = static_cast<InlineString<>::size_type>(other.size());
  status_ = other.status_;
  last_status_ = other.last_status_;
}

void StringBuilder::HandleStatusWithSize(StatusWithSize written) {
  const Status status = written.status();
  last_status_ = StatusCode(status);
  if (!status.ok()) {
    status_ = StatusCode(status);
  }

  *size_ += written.size();
}

void StringBuilder::SetErrorStatus(Status status) {
  last_status_ = StatusCode(status);
  status_ = StatusCode(status);
}

}  // namespace pw
