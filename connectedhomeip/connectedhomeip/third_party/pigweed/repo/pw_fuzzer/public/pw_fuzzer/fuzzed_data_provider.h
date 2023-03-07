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

// Simply forwards to <fuzzer/FuzzedDataProvider.h> when available from clang,
// or provides a simple stub implementation.
#pragma once

#if defined(__clang__)
#include <fuzzer/FuzzedDataProvider.h>
#else  // !defined(__clang__)

// If a fuzzer wants to use FuzzedDataProvider to build a fuzz target unit
// test without clang, it can use this trivial class with the same signature.
// The behavior will NOT be the same as a fuzzer unit test built by clang for
// non-trivial inputs. This means non-clang fuzzer unit tests will not be
// effective regression tests if given a seed corpus. These non-clang tests are
// still useful, however, as they will guarantee the fuzzer code can compile and
// link.
//
// The methods of this class are intentionally undocumented. To see the
// documentation for each, consult the real header file, e.g. in
// .cipd/pigweed/lib/clang/11.0.0/include/fuzzer/FuzzedDataProvider.h
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "pw_log/log.h"

class FuzzedDataProvider {
 public:
  FuzzedDataProvider(const uint8_t* /* data */, size_t /* size */) {
    PW_LOG_INFO("Fuzzing is disabled for the current compiler.");
    PW_LOG_INFO("Using trivial stub implementation for FuzzedDataProvider.");
  }

  ~FuzzedDataProvider() = default;

  template <typename T>
  std::vector<T> ConsumeBytes(size_t /* num_bytes */) {
    return std::vector<T>{};
  }

  template <typename T>
  std::vector<T> ConsumeBytesWithTerminator(size_t /* num_bytes */,
                                            T terminator = 0) {
    return std::vector<T>{terminator};
  }

  template <typename T>
  std::vector<T> ConsumeRemainingBytes() {
    return std::vector<T>{};
  }

  std::string ConsumeBytesAsString(size_t /* num_bytes */) {
    return std::string{};
  }

  std::string ConsumeRandomLengthString(size_t /* max_length */) {
    return std::string{};
  }

  std::string ConsumeRandomLengthString() { return std::string{}; }

  std::string ConsumeRemainingBytesAsString() { return std::string{}; }

  template <typename T>
  T ConsumeIntegral() {
    return T(0);
  }

  template <typename T>
  T ConsumeIntegralInRange(T min, T /* max */) {
    return T(min);
  }

  template <typename T>
  T ConsumeFloatingPoint() {
    return T(0.0);
  }

  template <typename T>
  T ConsumeFloatingPointInRange(T min, T /* max */) {
    return T(min);
  }

  template <typename T>
  T ConsumeProbability() {
    return T(0.0);
  }

  bool ConsumeBool() { return false; }

  template <typename T>
  T ConsumeEnum() {
    return static_cast<T>(0);
  }

  template <typename T, size_t kSize>
  T PickValueInArray(const T (&array)[kSize]) {
    static_assert(kSize > 0, "The array must be non empty.");
    return array[0];
  }

  template <typename T>
  T PickValueInArray(std::initializer_list<const T> list) {
    static_assert(list.size() > 0, "The list must be non empty.");
    return *list.begin();
  }

  size_t ConsumeData(void* /* destination */, size_t /* num_bytes */) {
    return 0;
  }

  size_t remaining_bytes() { return 0; }
};

#endif  // defined(__clang__)
