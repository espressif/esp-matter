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
#pragma once

#include <algorithm>
#include <cstdarg>
#include <cstddef>
#include <cstring>
#include <string_view>
#include <type_traits>
#include <utility>

#include "pw_preprocessor/compiler.h"
#include "pw_span/span.h"
#include "pw_status/status.h"
#include "pw_status/status_with_size.h"
#include "pw_string/string.h"
#include "pw_string/to_string.h"

namespace pw {

// StringBuilder facilitates building formatted strings in a fixed-size buffer.
// StringBuilders are always null terminated (unless they are constructed with
// an empty buffer) and never overflow. Status is tracked for each operation and
// an overall status is maintained, which reflects the most recent error.
//
// A StringBuilder does not own the buffer it writes to. It can be used to write
// strings to any buffer. The StringBuffer template class, defined below,
// allocates a buffer alongside a StringBuilder.
//
// StringBuilder supports C++-style << output, similar to std::ostringstream. It
// also supports std::string-like append functions and printf-style output.
//
// Support for custom types is added by overloading operator<< in the same
// namespace as the custom type. For example:
//
//   namespace my_project {
//
//   struct MyType {
//     int foo;
//     const char* bar;
//   };
//
//   pw::StringBuilder& operator<<(pw::StringBuilder& sb, const MyType& value) {
//     return sb << "MyType(" << value.foo << ", " << value.bar << ')';
//   }
//
//   }  // namespace my_project
//
// The ToString template function can be specialized to support custom types
// with StringBuilder, though overloading operator<< is generally preferred. For
// example:
//
//   namespace pw {
//
//   template <>
//   StatusWithSize ToString<MyStatus>(MyStatus value, span<char> buffer) {
//     return Copy(MyStatusString(value), buffer);
//   }
//
//   }  // namespace pw
//
class StringBuilder {
 public:
  // Creates an empty StringBuilder.
  explicit constexpr StringBuilder(span<char> buffer)
      : buffer_(buffer), size_(&inline_size_), inline_size_(0) {
    NullTerminate();
  }

  explicit StringBuilder(span<std::byte> buffer)
      : StringBuilder(
            {reinterpret_cast<char*>(buffer.data()), buffer.size_bytes()}) {}

  explicit constexpr StringBuilder(InlineString<>& string)
      : buffer_(string.data(), string.max_size() + 1),
        size_(&string.length_),
        inline_size_(0) {}

  // Disallow copy/assign to avoid confusion about where the string is actually
  // stored. StringBuffers may be copied into one another.
  StringBuilder(const StringBuilder&) = delete;

  StringBuilder& operator=(const StringBuilder&) = delete;

  // Returns the contents of the string buffer. Always null-terminated.
  const char* data() const { return buffer_.data(); }
  const char* c_str() const { return data(); }

  // Returns a std::string_view of the contents of this StringBuilder. The
  // std::string_view is invalidated if the StringBuilder contents change.
  std::string_view view() const { return std::string_view(data(), size()); }

  // Allow implicit conversions to std::string_view so StringBuilders can be
  // passed into functions that take a std::string_view.
  operator std::string_view() const { return view(); }

  // Returns a span<const std::byte> representation of this StringBuffer.
  span<const std::byte> as_bytes() const {
    return span(reinterpret_cast<const std::byte*>(buffer_.data()), size());
  }

  // Returns the StringBuilder's status, which reflects the most recent error
  // that occurred while updating the string. After an update fails, the status
  // remains non-OK until it is cleared with clear() or clear_status(). Returns:
  //
  //     OK if no errors have occurred
  //     RESOURCE_EXHAUSTED if output to the StringBuilder was truncated
  //     INVALID_ARGUMENT if printf-style formatting failed
  //     OUT_OF_RANGE if an operation outside the buffer was attempted
  //
  Status status() const { return static_cast<Status::Code>(status_); }

  // Returns status() and size() as a StatusWithSize.
  StatusWithSize status_with_size() const {
    return StatusWithSize(status(), size());
  }

  // The status from the last operation. May be OK while status() is not OK.
  Status last_status() const { return static_cast<Status::Code>(last_status_); }

  // True if status() is OkStatus().
  bool ok() const { return status().ok(); }

  // True if the string is empty.
  bool empty() const { return size() == 0u; }

  // Returns the current length of the string, excluding the null terminator.
  size_t size() const { return *size_; }

  // Returns the maximum length of the string, excluding the null terminator.
  size_t max_size() const { return buffer_.empty() ? 0u : buffer_.size() - 1; }

  // Clears the string and resets its error state.
  void clear();

  // Sets the statuses to OkStatus();
  void clear_status() {
    status_ = static_cast<unsigned char>(OkStatus().code());
    last_status_ = static_cast<unsigned char>(OkStatus().code());
  }

  // Appends a single character. Stets the status to RESOURCE_EXHAUSTED if the
  // character cannot be added because the buffer is full.
  void push_back(char ch) { append(1, ch); }

  // Removes the last character. Sets the status to OUT_OF_RANGE if the buffer
  // is empty (in which case the unsigned overflow is intentional).
  void pop_back() PW_NO_SANITIZE("unsigned-integer-overflow") {
    resize(size() - 1);
  }

  // Appends the provided character count times.
  StringBuilder& append(size_t count, char ch);

  // Appends count characters from str to the end of the StringBuilder. If count
  // exceeds the remaining space in the StringBuffer, max_size() - size()
  // characters are appended and the status is set to RESOURCE_EXHAUSTED.
  //
  // str is not considered null-terminated and may contain null characters.
  StringBuilder& append(const char* str, size_t count);

  // Appends characters from the null-terminated string to the end of the
  // StringBuilder. If the string's length exceeds the remaining space in the
  // buffer, max_size() - size() characters are copied and the status is set to
  // RESOURCE_EXHAUSTED.
  //
  // This function uses string::Length instead of std::strlen to avoid unbounded
  // reads if the string is not null terminated.
  StringBuilder& append(const char* str);

  // Appends a std::string_view to the end of the StringBuilder.
  StringBuilder& append(const std::string_view& str);

  // Appends a substring from the std::string_view to the StringBuilder. Copies
  // up to count characters starting from pos to the end of the StringBuilder.
  // If pos > str.size(), sets the status to OUT_OF_RANGE.
  StringBuilder& append(const std::string_view& str,
                        size_t pos,
                        size_t count = std::string_view::npos);

  // Appends to the end of the StringBuilder using the << operator. This enables
  // C++ stream-style formatted to StringBuilders.
  template <typename T>
  StringBuilder& operator<<(const T& value) {
    // For std::string_view-compatible types, use the append function, which
    // gives smaller code size.
    if constexpr (std::is_convertible_v<T, std::string_view>) {
      append(value);
    } else if constexpr (std::is_convertible_v<T, span<const std::byte>>) {
      WriteBytes(value);
    } else {
      HandleStatusWithSize(ToString(value, buffer_.subspan(size())));
    }
    return *this;
  }

  // Provide a few additional operator<< overloads that reduce code size.
  StringBuilder& operator<<(bool value) {
    return append(value ? "true" : "false");
  }

  StringBuilder& operator<<(char value) {
    push_back(value);
    return *this;
  }

  StringBuilder& operator<<(std::nullptr_t) {
    return append(string::kNullPointerString);
  }

  StringBuilder& operator<<(Status status) { return *this << status.str(); }

  // Appends a printf-style string to the end of the StringBuilder. If the
  // formatted string does not fit, the results are truncated and the status is
  // set to RESOURCE_EXHAUSTED.
  //
  // Internally, calls string::Format, which calls std::vsnprintf.
  PW_PRINTF_FORMAT(2, 3) StringBuilder& Format(const char* format, ...);

  // Appends a vsnprintf-style string with va_list arguments to the end of the
  // StringBuilder. If the formatted string does not fit, the results are
  // truncated and the status is set to RESOURCE_EXHAUSTED.
  //
  // Internally, calls string::Format, which calls std::vsnprintf.
  PW_PRINTF_FORMAT(2, 0)
  StringBuilder& FormatVaList(const char* format, va_list args);

  // Sets the StringBuilder's size. This function only truncates; if
  // new_size > size(), it sets status to OUT_OF_RANGE and does nothing.
  void resize(size_t new_size);

 protected:
  // Functions to support StringBuffer copies.
  constexpr StringBuilder(span<char> buffer, const StringBuilder& other)
      : buffer_(buffer),
        size_(&inline_size_),
        inline_size_(*other.size_),
        status_(other.status_),
        last_status_(other.last_status_) {}

  void CopySizeAndStatus(const StringBuilder& other);

 private:
  // Statuses are stored as an unsigned char so they pack into a single word.
  static constexpr unsigned char StatusCode(Status status) {
    return static_cast<unsigned char>(status.code());
  }

  void WriteBytes(span<const std::byte> data);

  size_t ResizeAndTerminate(size_t chars_to_append);

  void HandleStatusWithSize(StatusWithSize written);

  constexpr void NullTerminate() {
    if (!buffer_.empty()) {
      buffer_[size()] = '\0';
    }
  }

  void SetErrorStatus(Status status);

  const span<char> buffer_;

  InlineString<>::size_type* size_;

  // Place the inline_size_, status_, and last_status_ members together and use
  // unsigned char for the status codes so these members can be packed into a
  // single word.
  InlineString<>::size_type inline_size_;
  unsigned char status_ = StatusCode(OkStatus());
  unsigned char last_status_ = StatusCode(OkStatus());
};

// StringBuffers declare a buffer along with a StringBuilder. StringBuffer can
// be used as a statically allocated replacement for std::ostringstream or
// std::string. For example:
//
//   StringBuffer<32> str;
//   str << "The answer is " << number << "!";  // with number = 42
//   str.c_str();  // null terminated C string "The answer is 42."
//   str.view();   // std::string_view of "The answer is 42."
//
template <size_t kSizeBytes>
class StringBuffer : public StringBuilder {
 public:
  StringBuffer() : StringBuilder(buffer_) {}

  // StringBuffers of the same size may be copied and assigned into one another.
  StringBuffer(const StringBuffer& other) : StringBuilder(buffer_, other) {
    CopyContents(other);
  }

  // A smaller StringBuffer may be copied or assigned into a larger one.
  template <size_t kOtherSizeBytes>
  StringBuffer(const StringBuffer<kOtherSizeBytes>& other)
      : StringBuilder(buffer_, other) {
    static_assert(StringBuffer<kOtherSizeBytes>::max_size() <= max_size(),
                  "A StringBuffer cannot be copied into a smaller buffer");
    CopyContents(other);
  }

  template <size_t kOtherSizeBytes>
  StringBuffer& operator=(const StringBuffer<kOtherSizeBytes>& other) {
    assign<kOtherSizeBytes>(other);
    return *this;
  }

  StringBuffer& operator=(const StringBuffer& other) {
    assign<kSizeBytes>(other);
    return *this;
  }

  template <size_t kOtherSizeBytes>
  StringBuffer& assign(const StringBuffer<kOtherSizeBytes>& other) {
    static_assert(StringBuffer<kOtherSizeBytes>::max_size() <= max_size(),
                  "A StringBuffer cannot be copied into a smaller buffer");
    CopySizeAndStatus(other);
    CopyContents(other);
    return *this;
  }

  // Returns the maximum length of the string, excluding the null terminator.
  static constexpr size_t max_size() { return kSizeBytes - 1; }

  // Returns a StringBuffer<kSizeBytes>& instead of a generic StringBuilder& for
  // append calls and stream-style operations.
  template <typename... Args>
  StringBuffer& append(Args&&... args) {
    StringBuilder::append(std::forward<Args>(args)...);
    return *this;
  }

  template <typename T>
  StringBuffer& operator<<(T&& value) {
    static_cast<StringBuilder&>(*this) << std::forward<T>(value);
    return *this;
  }

 private:
  template <size_t kOtherSize>
  void CopyContents(const StringBuffer<kOtherSize>& other) {
    std::memcpy(buffer_, other.data(), other.size() + 1);  // include the \0
  }

  static_assert(kSizeBytes >= 1u, "StringBuffers must be at least 1 byte long");
  char buffer_[kSizeBytes];
};

namespace string_internal {

// Internal code for determining the default size of StringBuffers created with
// MakeString.
//
// StringBuffers created with MakeString default to at least 24 bytes. This is
// large enough to fit the largest 64-bit integer (20 digits plus a \0), rounded
// up to the nearest multiple of 4.
inline constexpr size_t kDefaultMinimumStringBufferSize = 24;

// By default, MakeString uses a buffer size large enough to fit all string
// literal arguments. ArgLength uses this value as an estimate of the number of
// characters needed to represent a non-string argument.
inline constexpr size_t kDefaultArgumentSize = 4;

// Returns a string literal's length or kDefaultArgumentSize for non-strings.
template <typename T>
constexpr size_t ArgLength() {
  using Arg = std::remove_reference_t<T>;

  // If the argument is an array of const char, assume it is a string literal.
  if constexpr (std::is_array_v<Arg>) {
    using Element = std::remove_reference_t<decltype(std::declval<Arg>()[0])>;

    if constexpr (std::is_same_v<Element, const char>) {
      return std::extent_v<Arg> > 0u ? std::extent_v<Arg> - 1 : size_t(0);
    }
  }

  return kDefaultArgumentSize;
}

// This function returns the default string buffer size used by MakeString.
template <typename... Args>
constexpr size_t DefaultStringBufferSize() {
  return std::max((size_t(1) + ... + ArgLength<Args>()),
                  kDefaultMinimumStringBufferSize);
}

// Internal version of MakeString with const reference arguments instead of
// deduced types, which include the lengths of string literals. Having this
// function can reduce code size.
template <size_t kBufferSize, typename... Args>
auto InitializeStringBuffer(const Args&... args) {
  return (StringBuffer<kBufferSize>() << ... << args);
}

}  // namespace string_internal

// Makes a StringBuffer with a string version of a series of values. This is
// useful for creating and initializing a StringBuffer or for conveniently
// getting a null-terminated string. For example:
//
//     LOG_INFO("The MAC address is %s", MakeString(mac_address).c_str());
//
// By default, the buffer size is 24 bytes, large enough to fit any 64-bit
// integer. If string literal arguments are provided, the default size will be
// large enough to fit them and a null terminator, plus 4 additional bytes for
// each argument. To use a fixed buffer size, set the kBufferSize template
// argument. For example:
//
//   // Creates a default-size StringBuffer (10 + 10 + 4 + 1 + 1 = 26 bytes).
//   auto sb = MakeString("1234567890", "1234567890", number, "!");
//
//   // Creates a 32-byte StringBuffer.
//   auto sb = MakeString<32>("1234567890", "1234567890", number, "!");
//
// Keep in mind that each argument to MakeString expands to a function call.
// MakeString may increase code size more than an equivalent pw::string::Format
// (or std::snprintf) call.
template <size_t kBufferSize = 0u, typename... Args>
auto MakeString(Args&&... args) {
  constexpr size_t kSize =
      kBufferSize == 0u ? string_internal::DefaultStringBufferSize<Args...>()
                        : kBufferSize;
  return string_internal::InitializeStringBuffer<kSize>(args...);
}

}  // namespace pw
