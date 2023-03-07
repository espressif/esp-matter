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

#include "pw_log_tokenized/config.h"

namespace pw {
namespace log_tokenized {
namespace internal {

// Internal class for managing the metadata bit fields.
template <typename T, unsigned kBits, unsigned kShift>
struct BitField {
 public:
  static constexpr T Get(T value) { return (value >> kShift) & kMask; }
  static constexpr T Shift(T value) {
    return (value <= kMask ? value : T(0)) << kShift;
  }

 private:
  static constexpr T kMask = (T(1) << kBits) - 1;
};

template <typename T, unsigned kShift>
class BitField<T, 0, kShift> {
 public:
  static constexpr T Get(T) { return 0; }
  static constexpr T Shift(T) { return 0; }
};

// This class, which is aliased to pw::log_tokenized::Metadata below, is used to
// access the log metadata packed into the tokenizer's payload argument.
template <unsigned kLevelBits,
          unsigned kLineBits,
          unsigned kFlagBits,
          unsigned kModuleBits,
          typename T = uintptr_t>
class GenericMetadata {
 public:
  template <T log_level = 0, T module = 0, T flags = 0, T line = 0>
  static constexpr GenericMetadata Set() {
    static_assert(log_level < (1 << kLevelBits), "The level is too large!");
    static_assert(line < (1 << kLineBits), "The line number is too large!");
    static_assert(flags < (1 << kFlagBits), "The flags are too large!");
    static_assert(module < (1 << kModuleBits), "The module is too large!");

    return GenericMetadata(BitsFromMetadata(log_level, module, flags, line));
  }

  // Only use this constructor for creating metadata from runtime values. This
  // constructor is unable to warn at compilation when values will not fit in
  // the specified bit field widths.
  constexpr GenericMetadata(T log_level, T module, T flags, T line)
      : value_(BitsFromMetadata(log_level, module, flags, line)) {}

  constexpr GenericMetadata(T value) : value_(value) {}

  // The log level of this message.
  constexpr T level() const { return Level::Get(value_); }

  // The line number of the log call. The first line in a file is 1. If the line
  // number is 0, it was too large to be stored.
  constexpr T line_number() const { return Line::Get(value_); }

  // The flags provided to the log call.
  constexpr T flags() const { return Flags::Get(value_); }

  // The 16 bit tokenized version of the module name (PW_LOG_MODULE_NAME).
  constexpr T module() const { return Module::Get(value_); }

  // The underlying packed metadata.
  constexpr T value() const { return value_; }

 private:
  using Level = BitField<T, kLevelBits, 0>;
  using Line = BitField<T, kLineBits, kLevelBits>;
  using Flags = BitField<T, kFlagBits, kLevelBits + kLineBits>;
  using Module = BitField<T, kModuleBits, kLevelBits + kLineBits + kFlagBits>;

  static constexpr T BitsFromMetadata(T log_level, T module, T flags, T line) {
    return Level::Shift(log_level) | Module::Shift(module) |
           Flags::Shift(flags) | Line::Shift(line);
  }

  T value_;

  static_assert(kLevelBits + kLineBits + kFlagBits + kModuleBits <=
                sizeof(value_) * 8);
};

}  // namespace internal

using Metadata = internal::GenericMetadata<PW_LOG_TOKENIZED_LEVEL_BITS,
                                           PW_LOG_TOKENIZED_LINE_BITS,
                                           PW_LOG_TOKENIZED_FLAG_BITS,
                                           PW_LOG_TOKENIZED_MODULE_BITS>;

}  // namespace log_tokenized
}  // namespace pw
