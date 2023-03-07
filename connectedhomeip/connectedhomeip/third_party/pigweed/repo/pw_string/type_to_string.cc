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

#include "pw_string/type_to_string.h"

#include <cmath>
#include <cstddef>
#include <cstring>
#include <limits>

namespace pw::string {
namespace {

// Powers of 10 (except 0) as an array. This table is fairly large (160 B), but
// avoids having to recalculate these values for each DecimalDigitCount call.
constexpr std::array<uint64_t, 20> kPowersOf10{
    0ull,
    10ull,                    // 10^1
    100ull,                   // 10^2
    1000ull,                  // 10^3
    10000ull,                 // 10^4
    100000ull,                // 10^5
    1000000ull,               // 10^6
    10000000ull,              // 10^7
    100000000ull,             // 10^8
    1000000000ull,            // 10^9
    10000000000ull,           // 10^10
    100000000000ull,          // 10^11
    1000000000000ull,         // 10^12
    10000000000000ull,        // 10^13
    100000000000000ull,       // 10^14
    1000000000000000ull,      // 10^15
    10000000000000000ull,     // 10^16
    100000000000000000ull,    // 10^17
    1000000000000000000ull,   // 10^18
    10000000000000000000ull,  // 10^19
};

StatusWithSize HandleExhaustedBuffer(span<char> buffer) {
  if (!buffer.empty()) {
    buffer[0] = '\0';
  }
  return StatusWithSize::ResourceExhausted();
}

}  // namespace

uint_fast8_t DecimalDigitCount(uint64_t integer) {
  // This fancy piece of code takes the log base 2, then approximates the
  // change-of-base formula by multiplying by 1233 / 4096.
  // TODO(hepler): Replace __builtin_clzll with std::countl_zeros in C++20.
  const uint_fast8_t log_10 = static_cast<uint_fast8_t>(
      (64 - __builtin_clzll(integer | 1)) * 1233 >> 12);

  // Adjust the estimated log base 10 by comparing against the power of 10.
  return log_10 + (integer < kPowersOf10[log_10] ? 0u : 1u);
}

// std::to_chars is available for integers in recent versions of GCC. I looked
// into switching to std::to_chars instead of this implementation. std::to_chars
// increased binary size by 160 B on an -Os build (even after removing
// DecimalDigitCount and its table). I didn't measure performance, but I don't
// think std::to_chars will be faster, so I kept this implementation for now.
template <>
StatusWithSize IntToString(uint64_t value, span<char> buffer) {
  constexpr uint32_t base = 10;
  constexpr uint32_t max_uint32_base_power = 1'000'000'000;
  constexpr uint_fast8_t max_uint32_base_power_exponent = 9;

  const uint_fast8_t total_digits = DecimalDigitCount(value);

  if (total_digits >= buffer.size()) {
    return HandleExhaustedBuffer(buffer);
  }

  buffer[total_digits] = '\0';

  uint_fast8_t remaining = total_digits;
  while (remaining > 0u) {
    uint32_t lower_digits;     // the value of the lower digits to write
    uint_fast8_t digit_count;  // the number of lower digits to write

    // 64-bit division is slow on 32-bit platforms, so print large numbers in
    // 32-bit chunks to minimize the number of 64-bit divisions.
    if (value <= std::numeric_limits<uint32_t>::max()) {
      lower_digits = static_cast<uint32_t>(value);
      digit_count = remaining;
    } else {
      lower_digits = value % max_uint32_base_power;
      digit_count = max_uint32_base_power_exponent;
      value /= max_uint32_base_power;
    }

    // Write the specified number of digits, with leading 0s.
    for (uint_fast8_t i = 0; i < digit_count; ++i) {
      buffer[--remaining] = lower_digits % base + '0';
      lower_digits /= base;
    }
  }
  return StatusWithSize(total_digits);
}

StatusWithSize IntToHexString(uint64_t value,
                              span<char> buffer,
                              uint_fast8_t min_width) {
  const uint_fast8_t digits = std::max(HexDigitCount(value), min_width);

  if (digits >= buffer.size()) {
    return HandleExhaustedBuffer(buffer);
  }

  for (int i = digits - 1; i >= 0; --i) {
    buffer[i] = "0123456789abcdef"[value & 0xF];
    value >>= 4;
  }

  buffer[digits] = '\0';
  return StatusWithSize(digits);
}

template <>
StatusWithSize IntToString(int64_t value, span<char> buffer) {
  if (value >= 0) {
    return IntToString<uint64_t>(value, buffer);
  }

  // Write as an unsigned number, but leave room for the leading minus sign.
  auto result = IntToString<uint64_t>(
      std::abs(value), buffer.empty() ? buffer : buffer.subspan(1));

  if (result.ok()) {
    buffer[0] = '-';
    return StatusWithSize(result.size() + 1);
  }

  return HandleExhaustedBuffer(buffer);
}

StatusWithSize FloatAsIntToString(float value, span<char> buffer) {
  // If it's finite and fits in an int64_t, print it as a rounded integer.
  if (std::isfinite(value) &&
      std::abs(value) <
          static_cast<float>(std::numeric_limits<int64_t>::max())) {
    return IntToString<int64_t>(static_cast<int64_t>(std::roundf(value)),
                                buffer);
  }

  // Otherwise, print inf or NaN, if they fit.
  if (const size_t written = 3 + std::signbit(value); written < buffer.size()) {
    char* out = buffer.data();
    if (std::signbit(value)) {
      *out++ = '-';
    }
    std::memcpy(out, std::isnan(value) ? "NaN" : "inf", sizeof("NaN"));
    return StatusWithSize(written);
  }

  return HandleExhaustedBuffer(buffer);
}

StatusWithSize BoolToString(bool value, span<char> buffer) {
  return CopyEntireStringOrNull(value ? "true" : "false", buffer);
}

StatusWithSize PointerToString(const void* pointer, span<char> buffer) {
  if (pointer == nullptr) {
    return CopyEntireStringOrNull(kNullPointerString, buffer);
  }
  return IntToHexString(reinterpret_cast<uintptr_t>(pointer), buffer);
}

StatusWithSize CopyEntireStringOrNull(const std::string_view& value,
                                      span<char> buffer) {
  if (value.size() >= buffer.size()) {
    return HandleExhaustedBuffer(buffer);
  }

  std::memcpy(buffer.data(), value.data(), value.size());
  buffer[value.size()] = '\0';
  return StatusWithSize(value.size());
}

}  // namespace pw::string
