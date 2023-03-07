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
//
// This tests the system installed C standard library version of memset.
//
// Note: We have caught real production bugs with these tests. Do not assume
// your vendor's C library is correct! For standard C functions like memset and
// memcpy, there are compiler intrisics which assume that the C standard is
// followed. If the implemention of memset or memcpy does not exactly follow
// the standard, subtle and hard to track down bugs can be the result.

#include <array>
#include <cstring>
#include <numeric>

#include "gtest/gtest.h"
#include "pw_containers/algorithm.h"

namespace pw {
namespace {

// From the ISO C standard:
// http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf
//
// Section 7.21.6.1: memset(void *s, int c, size_t n)
//
//   void* memset(void* buffer,
//                int character,
//                size_t num_bytes);
//
//   Copy c into the first n bytes of s.
//   Returns buffer, a copy of the destination pointer.
//

TEST(Memset, EmptyCase) {
  std::array<char, 5> arr{'h', 'e', 'l', 'l', 'o'};
  void* ret = memset(arr.data(), 0, 0);

  // Destination buffer returned.
  EXPECT_EQ(ret, arr.data());

  // Destination buffer untouched.
  constexpr std::array<char, 5> kExpected{'h', 'e', 'l', 'l', 'o'};
  EXPECT_TRUE(pw::containers::Equal(arr, kExpected));
}

TEST(Memset, OneCharacter) {
  std::array<char, 5> arr{'h', 'e', 'l', 'l', 'o'};
  void* ret = memset(arr.data(), 0, 1);

  // Ensure the destination buffer is returned.
  EXPECT_EQ(ret, arr.data());

  // Ensure the destination buffer is untouched.
  constexpr std::array<char, 5> kExpected{0, 'e', 'l', 'l', 'o'};
  EXPECT_TRUE(pw::containers::Equal(arr, kExpected));
}

// Now do a detailed case with more values. Span both word sizes and alignments
// to ensure we hit some edge cases.
TEST(Memset, MultipleSizesMultipleAlignments) {
  constexpr int kMaxBytes = 64;
  std::array<char, kMaxBytes> arr;

  constexpr int kMaxAlignment = 16;

  // Avoid 0 sentinel to prevent interaction with uninitialized memory.
  constexpr char kSentinel = 3;
  constexpr char kIotaStart = kSentinel + 7;

  // Try different alignments.
  for (int alignment = 0; alignment < kMaxAlignment; ++alignment) {
    // Try different memset sizes.
    for (int write_size = 0; write_size < (kMaxBytes - kMaxAlignment);
         ++write_size) {
      // Fill entire array with incrementing integers; starting above sentinel.
      std::iota(arr.begin(), arr.end(), kIotaStart);

      // Memset the first write_size bytes, with our sentinel
      void* write_head = &arr[alignment];
      const void* ret = memset(write_head, kSentinel, write_size);

      // Check destination buffer returned.
      EXPECT_EQ(ret, write_head);

      for (int j = 0; j < kMaxBytes; ++j) {
        if (j < alignment) {
          // First part of destination buffer untouched; should match iota.
          EXPECT_EQ(arr[j], kIotaStart + j);
        } else if (j < alignment + write_size) {
          // Second part is set to the sentinel value.
          EXPECT_EQ(arr[j], kSentinel);
        } else {
          // Third part is back to the iota content.
          EXPECT_EQ(arr[j], kIotaStart + j);
        }
      }
    }
  }
}

}  // namespace
}  // namespace pw
