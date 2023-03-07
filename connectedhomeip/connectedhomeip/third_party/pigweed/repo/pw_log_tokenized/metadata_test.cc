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

#include "pw_log_tokenized/metadata.h"

#include "gtest/gtest.h"

namespace pw::log_tokenized {
namespace {

TEST(Metadata, NoLineBits) {
  using NoLineBits = internal::GenericMetadata<6, 0, 10, 16>;

  constexpr NoLineBits test1 = NoLineBits::Set<0, 0, 0>();
  static_assert(test1.level() == 0);
  static_assert(test1.module() == 0);
  static_assert(test1.flags() == 0);
  static_assert(test1.line_number() == 0);

  constexpr NoLineBits test2 = NoLineBits::Set<3, 2, 1>();
  static_assert(test2.level() == 3);
  static_assert(test2.module() == 2);
  static_assert(test2.flags() == 1);
  static_assert(test2.line_number() == 0);

  constexpr NoLineBits test3 = NoLineBits::Set<63, 65535, 1023>();
  static_assert(test3.level() == 63);
  static_assert(test3.module() == 65535);
  static_assert(test3.flags() == 1023);
  static_assert(test3.line_number() == 0);
}

TEST(Metadata, NoFlagBits) {
  using NoFlagBits = internal::GenericMetadata<3, 13, 0, 16>;

  constexpr NoFlagBits test1 = NoFlagBits::Set<0, 0, 0, 0>();
  static_assert(test1.level() == 0);
  static_assert(test1.module() == 0);
  static_assert(test1.flags() == 0);
  static_assert(test1.line_number() == 0);

  constexpr NoFlagBits test2 = NoFlagBits::Set<3, 2, 0, 1>();
  static_assert(test2.level() == 3);
  static_assert(test2.module() == 2);
  static_assert(test2.flags() == 0);
  static_assert(test2.line_number() == 1);

  constexpr NoFlagBits test3 = NoFlagBits::Set<7, 65535, 0, (1 << 13) - 1>();
  static_assert(test3.level() == 7);
  static_assert(test3.module() == 65535);
  static_assert(test3.flags() == 0);
  static_assert(test3.line_number() == (1 << 13) - 1);
}

TEST(Metadata, EncodedValue_Zero) {
  constexpr Metadata test1 = Metadata::Set<0, 0, 0, 0>();
  static_assert(test1.value() == 0);
}

TEST(Metadata, EncodedValue_Nonzero) {
  constexpr size_t kExpectedLevel = 3;
  constexpr size_t kExpectedLine = 2022;
  constexpr size_t kExpectedFlags = 0b10;
  constexpr size_t kExpectedModule = 1337;
  constexpr size_t kExpectedValue =
      (kExpectedLevel) | (kExpectedLine << PW_LOG_TOKENIZED_LEVEL_BITS) |
      (kExpectedFlags << (PW_LOG_TOKENIZED_LEVEL_BITS +
                          PW_LOG_TOKENIZED_LINE_BITS)) |
      (kExpectedModule << (PW_LOG_TOKENIZED_LEVEL_BITS +
                           PW_LOG_TOKENIZED_LINE_BITS +
                           PW_LOG_TOKENIZED_FLAG_BITS));
  constexpr Metadata test = Metadata::
      Set<kExpectedLevel, kExpectedModule, kExpectedFlags, kExpectedLine>();
  static_assert(test.value() == kExpectedValue);
}

TEST(Metadata, EncodedValue_NonzeroConstructor) {
  constexpr size_t kExpectedLevel = 1;
  constexpr size_t kExpectedLine = 99;
  constexpr size_t kExpectedFlags = 0b11;
  constexpr size_t kExpectedModule = 8900;
  constexpr size_t kExpectedValue =
      (kExpectedLevel) | (kExpectedLine << PW_LOG_TOKENIZED_LEVEL_BITS) |
      (kExpectedFlags << (PW_LOG_TOKENIZED_LEVEL_BITS +
                          PW_LOG_TOKENIZED_LINE_BITS)) |
      (kExpectedModule << (PW_LOG_TOKENIZED_LEVEL_BITS +
                           PW_LOG_TOKENIZED_LINE_BITS +
                           PW_LOG_TOKENIZED_FLAG_BITS));
  constexpr Metadata test =
      Metadata(kExpectedLevel, kExpectedModule, kExpectedFlags, kExpectedLine);
  static_assert(test.value() == kExpectedValue);
}

TEST(Metadata, EncodedValue_Overflow) {
  constexpr size_t kExpectedLevel = 144;
  constexpr Metadata test = Metadata(kExpectedLevel, 0, 0, 0);
  static_assert(test.value() == 0);
}

}  // namespace
}  // namespace pw::log_tokenized
