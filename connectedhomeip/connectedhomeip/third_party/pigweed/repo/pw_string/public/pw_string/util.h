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

#include <cctype>
#include <cstddef>
#include <string_view>

#include "pw_assert/assert.h"
#include "pw_polyfill/language_feature_macros.h"
#include "pw_result/result.h"
#include "pw_span/span.h"
#include "pw_status/status.h"
#include "pw_status/status_with_size.h"
#include "pw_string/internal/length.h"
#include "pw_string/string.h"

namespace pw {
namespace string {
namespace internal {

PW_CONSTEXPR_CPP20 inline StatusWithSize CopyToSpan(
    const std::string_view& source, span<char> dest) {
  if (dest.empty()) {
    return StatusWithSize::ResourceExhausted();
  }

  const size_t copied = source.copy(dest.data(), dest.size() - 1);
  dest[copied] = '\0';

  return StatusWithSize(
      copied == source.size() ? OkStatus() : Status::ResourceExhausted(),
      copied);
}

}  // namespace internal

// Safe alternative to the string_view constructor to avoid the risk of an
// unbounded implicit or explicit use of strlen.
//
// This is strongly recommended over using something like C11's strnlen_s as
// a string_view does not require null-termination.
constexpr std::string_view ClampedCString(span<const char> str) {
  return std::string_view(str.data(),
                          internal::ClampedLength(str.data(), str.size()));
}

constexpr std::string_view ClampedCString(const char* str, size_t max_len) {
  return ClampedCString(span<const char>(str, max_len));
}

// Safe alternative to strlen to calculate the null-terminated length of the
// string within the specified span, excluding the null terminator. Like C11's
// strnlen_s, the scan for the null-terminator is bounded.
//
// Returns:
//   null-terminated length of the string excluding the null terminator.
//   OutOfRange - if the string is not null-terminated.
//
// Precondition: The string shall be at a valid pointer.
constexpr Result<size_t> NullTerminatedLength(span<const char> str) {
  PW_DASSERT(str.data() != nullptr);

  const size_t length = internal::ClampedLength(str.data(), str.size());
  if (length == str.size()) {
    return Status::OutOfRange();
  }

  return length;
}

constexpr Result<size_t> NullTerminatedLength(const char* str, size_t max_len) {
  return NullTerminatedLength(span<const char>(str, max_len));
}

// Copies the source string to the dest, truncating if the full string does not
// fit. Always null terminates if dest.size() or num > 0.
//
// Returns the number of characters written, excluding the null terminator. If
// the string is truncated, the status is ResourceExhausted.
//
// Precondition: The destination and source shall not overlap.
// Precondition: The source shall be a valid pointer.
template <typename Span>
PW_CONSTEXPR_CPP20 inline StatusWithSize Copy(const std::string_view& source,
                                              Span&& dest) {
  static_assert(
      !std::is_base_of_v<InlineString<>, std::decay_t<Span>>,
      "Do not use pw::string::Copy() with pw::InlineString<>. Instead, use "
      "pw::InlineString<>'s assignment operator or assign() function, or "
      "pw::string::Append().");
  return internal::CopyToSpan(source, std::forward<Span>(dest));
}

template <typename Span>
PW_CONSTEXPR_CPP20 inline StatusWithSize Copy(const char* source, Span&& dest) {
  PW_DASSERT(source != nullptr);
  return Copy(ClampedCString(source, std::size(dest)),
              std::forward<Span>(dest));
}

PW_CONSTEXPR_CPP20 inline StatusWithSize Copy(const char* source,
                                              char* dest,
                                              size_t num) {
  return Copy(source, span<char>(dest, num));
}

// Assigns a std::string_view to a pw::InlineString, truncating if it does not
// fit. pw::InlineString's assign() function asserts if the string's requested
// size exceeds its capacity; pw::string::Assign() returns a Status instead.
//
// Returns:
//    OK - the entire std::string_view was copied to the end of the InlineString
//    RESOURCE_EXHAUSTED - the std::string_view was truncated to fit
inline Status Assign(InlineString<>& string, const std::string_view& view) {
  const size_t chars_copied =
      std::min(view.size(), static_cast<size_t>(string.capacity()));
  string.assign(view, 0, static_cast<string_impl::size_type>(chars_copied));
  return chars_copied == view.size() ? OkStatus() : Status::ResourceExhausted();
}

inline Status Assign(InlineString<>& string, const char* c_string) {
  PW_DASSERT(c_string != nullptr);
  // Clamp to capacity + 1 so strings larger than the capacity yield an error.
  return Assign(string, ClampedCString(c_string, string.capacity() + 1));
}

// Appends a std::string_view to a pw::InlineString, truncating if it does not
// fit. pw::InlineString's append() function asserts if the string's requested
// size exceeds its capacity; pw::string::Append() returns a Status instead.
//
// Returns:
//    OK - the entire std::string_view was assigned
//    RESOURCE_EXHAUSTED - the std::string_view was truncated to fit
inline Status Append(InlineString<>& string, const std::string_view& view) {
  const size_t chars_copied = std::min(
      view.size(), static_cast<size_t>(string.capacity() - string.size()));
  string.append(view, 0, static_cast<string_impl::size_type>(chars_copied));
  return chars_copied == view.size() ? OkStatus() : Status::ResourceExhausted();
}

inline Status Append(InlineString<>& string, const char* c_string) {
  PW_DASSERT(c_string != nullptr);
  // Clamp to capacity + 1 so strings larger than the capacity yield an error.
  return Append(string, ClampedCString(c_string, string.capacity() + 1));
}

// Copies source string to the dest with same behavior as Copy, with the
// difference that any non-printable characters are changed to '.'.
PW_CONSTEXPR_CPP20 inline StatusWithSize PrintableCopy(
    const std::string_view& source, span<char> dest) {
  StatusWithSize copy_result = Copy(source, dest);
  for (size_t i = 0; i < copy_result.size(); i++) {
    dest[i] = std::isprint(dest[i]) ? dest[i] : '.';
  }

  return copy_result;
}

}  // namespace string
}  // namespace pw
