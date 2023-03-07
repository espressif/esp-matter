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

#include <cstdint>
#include <string_view>

#include "pw_bytes/array.h"
#include "pw_checksum/crc32.h"
#include "pw_perf_test/perf_test.h"
#include "pw_span/internal/span_impl.h"
#include "pw_span/span.h"

namespace pw::checksum {
namespace {

constexpr std::string_view kString =
    "In the beginning the Universe was created. This has made a lot of "
    "people very angry and been widely regarded as a bad move.";
constexpr auto kBytes = bytes::Array<1, 2, 3, 4, 5, 6, 7, 8, 9>();

void Crc32OneBitTest(perf_test::State& state, span<const std::byte> data) {
  while (state.KeepRunning()) {
    Crc32OneBit::Calculate(data);
  }
}

void Crc32FourBitTest(perf_test::State& state, span<const std::byte> data) {
  while (state.KeepRunning()) {
    Crc32FourBit::Calculate(data);
  }
}

void Crc32EightBitTest(perf_test::State& state, span<const std::byte> data) {
  while (state.KeepRunning()) {
    Crc32EightBit::Calculate(data);
  }
}

PW_PERF_TEST(CrcOneBitStringTest, Crc32OneBitTest, as_bytes(span(kString)));
PW_PERF_TEST(CrcFourBitStringTest, Crc32FourBitTest, as_bytes(span(kString)));
PW_PERF_TEST(CrcEightBitStringTest, Crc32EightBitTest, as_bytes(span(kString)));

PW_PERF_TEST(CrcOneBitBytesTest, Crc32OneBitTest, kBytes);
PW_PERF_TEST(CrcFourBitBytesTest, Crc32FourBitTest, kBytes);
PW_PERF_TEST(CrcEightBitBytesTest, Crc32EightBitTest, kBytes);

}  // namespace
}  // namespace pw::checksum
