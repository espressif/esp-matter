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

// decode.h defines classes that implement tokenized string decoding. These
// classes should not be used directly; instead decode tokenized messages with
// the Detokenizer class, defined in pw_tokenizer/detokenize.h.
#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "pw_span/span.h"

// Decoding errors are marked with prefix and suffix so that they stand out from
// the rest of the decoded strings. These macros are used to build decoding
// error strings.
#define PW_TOKENIZER_ARG_DECODING_ERROR_PREFIX "<["
#define PW_TOKENIZER_ARG_DECODING_ERROR_SUFFIX "]>"
#define PW_TOKENIZER_ARG_DECODING_ERROR(message) \
  PW_TOKENIZER_ARG_DECODING_ERROR_PREFIX message \
      PW_TOKENIZER_ARG_DECODING_ERROR_SUFFIX

namespace pw::tokenizer {

// The status of an argument that was decoded from an encoded tokenized string.
// This enum should match the values in decode.py's DecodedArg class.
class ArgStatus {
 public:
  // The Code enum tracks issues arise when decoding a tokenized string
  // argument. Each value is one bit, and an ArgStatus will have multiple bits
  // set if multiple issues are encountered.
  enum Code : unsigned {
    kOk = 0,           // Decoding was successful.
    kMissing = 1,      // The argument was not present in the data.
    kTruncated = 2,    // The argument was truncated during encoding.
    kDecodeError = 4,  // An error occurred while decoding the argument.
    kSkipped = 8,      // Argument was skipped due to a previous error.
  };

  constexpr ArgStatus(Code code = kOk) : status_(code) {}

  // Sets additional status bits.
  constexpr void Update(ArgStatus status) { status_ |= status.status_; }

  // True if no decoding errors occurred. Truncated is considered OK, since
  // encoding and decoding still occurs successfully when a string is truncated.
  constexpr bool ok() const { return status_ == kOk || status_ == kTruncated; }

  // Checks if an error flag is set in the status.
  constexpr bool HasError(Code code) const { return (status_ & code) != 0u; }

 private:
  // Since multiple Code bits may be set in an ArgStatus, the status is stored
  // as an unsigned instead of a Code.
  unsigned status_;
};

// An argument decoded from an encoded tokenized message.
class DecodedArg {
 public:
  // Constructs a DecodedArg from a decoded value. The value is formatted into a
  // string using the provided format string. The number of bytes that were
  // decoded to get the value are provided in raw_size_bytes.
  template <typename ArgumentType>
  static DecodedArg FromValue(const char* format_string,
                              ArgumentType value,
                              size_t raw_size_bytes,
                              ArgStatus arg_status = ArgStatus::kOk);

  // Constructs a DecodedArg that represents a string literal in the format
  // string (plain text or % character).
  DecodedArg(const std::string& literal)
      : value_(literal), raw_data_size_bytes_(0) {}

  // Constructs a DecodedArg that encountered an error during decoding.
  DecodedArg(ArgStatus error,
             const std::string_view& spec,
             size_t raw_size_bytes = 0u,
             const std::string_view& value = {});

  // This argument's value as a string. If an error occurred while decoding this
  // argument, value() will be an error message.
  const std::string& value() const { return value_; }

  // Returns the conversion specification for this argument (e.g. %02x). This is
  // empty for literals or "%%".
  const std::string& spec() const { return spec_; }

  // True if this argument decoded successfully.
  bool ok() const { return status_.ok(); }

  // How many bytes this arg occupied in the encoded arguments.
  size_t raw_size_bytes() const { return raw_data_size_bytes_; }

 private:
  DecodedArg(const char* format, size_t raw_size_bytes, ArgStatus status)
      : spec_(format), raw_data_size_bytes_(raw_size_bytes), status_(status) {}

  std::string value_;
  std::string spec_;
  size_t raw_data_size_bytes_;
  ArgStatus status_;
};

// Represents a segment of a printf-style format string. Each StringSegment
// contains either literal text or a format specifier.
class StringSegment {
 public:
  // Parses a format specifier from the text and returns a StringSegment that
  // represents it. Returns an empty StringSegment if no valid format specifier
  // was found.
  static StringSegment ParseFormatSpec(const char* format);

  // Creates a StringSegment that represents a piece of plain text.
  StringSegment(const std::string_view& text) : StringSegment(text, kLiteral) {}

  // Returns the DecodedArg with this StringSegment decoded according to the
  // provided arguments.
  DecodedArg Decode(const span<const uint8_t>& arguments) const;

  // Skips decoding this StringSegment. Literals and %% are expanded as normal.
  DecodedArg Skip() const;

  bool empty() const { return text_.empty(); }

  const std::string& text() const { return text_; }

 private:
  enum Type {
    kLiteral,
    kPercent,  // %% format specifier
    kString,
    kSignedInt,
    kUnsigned32,
    kUnsigned64,
    kFloatingPoint,
  };

  // Varargs-promoted size of args on this machine; only needed for ints or %p.
  enum ArgSize : bool { k32Bit, k64Bit };

  template <typename T>
  static constexpr ArgSize VarargSize() {
    return sizeof(T) == sizeof(int64_t) ? k64Bit : k32Bit;
  }

  static ArgSize VarargSize(std::array<char, 2> length, char spec);

  StringSegment() : type_(kLiteral) {}

  StringSegment(const std::string_view& text, Type type)
      : StringSegment(text, type, VarargSize<void*>()) {}

  StringSegment(const std::string_view& text, Type type, ArgSize local_size)
      : text_(text), type_(type), local_size_(local_size) {}

  DecodedArg DecodeString(const span<const uint8_t>& arguments) const;

  DecodedArg DecodeInteger(const span<const uint8_t>& arguments) const;

  DecodedArg DecodeFloatingPoint(const span<const uint8_t>& arguments) const;

  std::string text_;
  Type type_;
  ArgSize local_size_;  // Arg size to use for snprintf on this machine.
};

// The result of decoding a tokenized message with a FormatString. Stores
// decoded arguments and whether there was any undecoded data. This is returned
// from a FormatString::Format call.
class DecodedFormatString {
 public:
  DecodedFormatString(std::vector<DecodedArg>&& segments,
                      size_t remaining_bytes)
      : segments_(std::move(segments)), remaining_bytes_(remaining_bytes) {}

  DecodedFormatString(const DecodedFormatString&) = default;
  DecodedFormatString(DecodedFormatString&&) = default;

  DecodedFormatString& operator=(const DecodedFormatString&) = default;
  DecodedFormatString& operator=(DecodedFormatString&&) = default;

  // Returns the decoded format string. If any argument decoding errors
  // occurred, the % conversion specifiers are included unmodified.
  std::string value() const;

  // Returns the decoded format string, with error messages for any arguments
  // that failed to decode.
  std::string value_with_errors() const;

  bool ok() const { return remaining_bytes() == 0u && decoding_errors() == 0u; }

  // Returns the number of bytes that remained after decoding.
  size_t remaining_bytes() const { return remaining_bytes_; }

  // Returns the number of arguments in the format string. %% is not included.
  size_t argument_count() const;

  // Returns the number of arguments that failed to decode.
  size_t decoding_errors() const;

 private:
  std::vector<DecodedArg> segments_;
  size_t remaining_bytes_;
};

// Represents a printf-style format string. The string is stored as a vector of
// StringSegments.
class FormatString {
 public:
  // Constructs a FormatString from a null-terminated format string.
  FormatString(const char* format_string);

  // Formats this format string according to the provided encoded arguments and
  // returns a string.
  DecodedFormatString Format(span<const uint8_t> arguments) const;

  DecodedFormatString Format(const std::string_view& arguments) const {
    return Format(span(reinterpret_cast<const uint8_t*>(arguments.data()),
                       arguments.size()));
  }

 private:
  std::vector<StringSegment> segments_;
};

// Implementation of DecodedArg::FromValue template function.
template <typename ArgumentType>
DecodedArg DecodedArg::FromValue(const char* format,
                                 ArgumentType value,
                                 size_t raw_size_bytes,
                                 ArgStatus status) {
  DecodedArg arg(format, raw_size_bytes, status);
  const int value_size = std::snprintf(nullptr, 0u, format, value);

  if (value_size < 0) {
    arg.status_.Update(ArgStatus::kDecodeError);
    return arg;
  }

  // Reserve space in the value string for the snprintf call.
  arg.value_.append(value_size + 1, '\0');

  // Print the value to the string in the reserved space, then pop off the \0.
  std::snprintf(arg.value_.data(), arg.value_.size(), format, value);
  arg.value_.pop_back();  // Remove the trailing \0.

  return arg;
}

}  // namespace pw::tokenizer
