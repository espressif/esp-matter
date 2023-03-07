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

#include <string_view>

#include "pw_bytes/array.h"
#include "pw_checksum/crc16_ccitt.h"
#include "pw_perf_test/perf_test.h"
#include "pw_span/internal/span_impl.h"
#include "pw_span/span.h"

namespace pw::checksum {
namespace {

constexpr std::string_view kString =
    "In the beginning the Universe was created. This has made a lot of "
    "people very angry and been widely regarded as a bad move.";
constexpr auto kBytes = bytes::Initialized<1000>([](size_t i) { return i; });

void CcittCalculationTest(span<const std::byte> input) {
  Crc16Ccitt::Calculate(input);
}

PW_PERF_TEST_SIMPLE(CcittCalculationBytes, CcittCalculationTest, kBytes);
PW_PERF_TEST_SIMPLE(CcittCalculationString,
                    CcittCalculationTest,
                    as_bytes(span(kString)));

}  // namespace
}  // namespace pw::checksum
