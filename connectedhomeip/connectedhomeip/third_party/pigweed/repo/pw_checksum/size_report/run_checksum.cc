// Copyright 2022 The Pigweed Authors
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

#include <cstring>

#include "pw_bloat/bloat_this_binary.h"
#include "pw_log/log.h"
#include "pw_preprocessor/compiler.h"
#include "pw_span/span.h"

#ifdef USE_CRC16_CHECKSUM
#include "pw_checksum/crc16_ccitt.h"
using TheChecksum = pw::checksum::Crc16Ccitt;
#endif

#ifdef USE_CRC32_8BIT_CHECKSUM
#include "pw_checksum/crc32.h"
using TheChecksum = pw::checksum::Crc32EightBit;
#endif

#ifdef USE_CRC32_4BIT_CHECKSUM
#include "pw_checksum/crc32.h"
using TheChecksum = pw::checksum::Crc32FourBit;
#endif

#ifdef USE_CRC32_1BIT_CHECKSUM
#include "pw_checksum/crc32.h"
using TheChecksum = pw::checksum::Crc32OneBit;
#endif

namespace pw::checksum {

#ifdef USE_NOOP_CHECKSUM
class NoOpChecksum {
 public:
  static uint32_t Calculate(span<const std::byte>) { return arbitrary_value; }

  // Don't inline to prevent the compiler from optimizing out the checksum.
  PW_NO_INLINE void Update(span<const std::byte>) {}

  PW_NO_INLINE void Update(std::byte) {}

  PW_NO_INLINE uint32_t value() const { return arbitrary_value; }
  void clear() {}

 private:
  // static volatile uint32_t arbitrary_value;
  const static uint32_t arbitrary_value = 10;
};
using TheChecksum = NoOpChecksum;
#endif

// Fletcher16 is a simple checksum that shouldn't be used in production, but is
// interesting from a size comparison perspective.
#ifdef USE_FLETCHER16_CHECKSUM
class Fletcher16 {
 public:
  Fletcher16() : sum1_(0), sum2_(0) {}

  // Don't inline to prevent the compiler from optimizing out the checksum.
  PW_NO_INLINE static uint32_t Calculate(span<const std::byte> data) {
    Fletcher16 checksum;
    checksum.Update(data);
    return checksum.value();
  }

  PW_NO_INLINE void Update(span<const std::byte> data) {
    for (std::byte b : data) {
      sum1_ = static_cast<uint16_t>((sum1_ + static_cast<uint16_t>(b)) % 255u);
      sum2_ = static_cast<uint16_t>((sum2_ + sum1_) % 255u);
    }
  }
  PW_NO_INLINE void Update(std::byte) {}
  PW_NO_INLINE uint32_t value() const { return (sum2_ << 8) | sum1_; };
  void clear() {}

 private:
  uint16_t sum1_ = 0;
  uint16_t sum2_ = 0;
};
using TheChecksum = Fletcher16;
#endif

char buffer[128];
char* volatile get_buffer = buffer;
volatile unsigned get_size;

unsigned RunChecksum() {
  // Trick the optimizer and also satisfy the type checker.
  get_size = sizeof(buffer);
  char* local_buffer = get_buffer;
  unsigned local_size = get_size;

  // Calculate the checksum and stash it in a volatile variable so the compiler
  // can't optimize it out.
  TheChecksum checksum;
  checksum.Update(pw::as_bytes(span(local_buffer, local_size)));
  uint32_t value = static_cast<uint32_t>(checksum.value());
  *get_buffer = static_cast<char>(value);
  return 0;
}

}  // namespace pw::checksum

int main() {
  pw::bloat::BloatThisBinary();
  return pw::checksum::RunChecksum();
}
