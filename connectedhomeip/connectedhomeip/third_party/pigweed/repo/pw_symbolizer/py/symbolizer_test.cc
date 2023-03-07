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

#include <cinttypes>
#include <cstdint>
#include <cstdio>

namespace pw::symbolizer::test {

struct Fish {
  bool has_legs;
  bool tastes_like_chicken;
};

Fish gerald;

}  // namespace pw::symbolizer::test

void PrintExpectedObjSymbol(void* address, const char* expected) {
  uintptr_t val = reinterpret_cast<uintptr_t>(address);
  printf("{\"Address\":%" PRIuPTR ",\"Expected\":\"%s\",\"IsObj\":true}\n",
         val,
         expected);
}

namespace {

void GoWild() {
  volatile int counter = 0;
  for (int i = 0; i < 123; i++) {
    counter += i * counter;
  }
}

}  // namespace

namespace another::one {

void PrintExpectedFuncSymbol(void* address, const char* expected, int line) {
  uintptr_t val = reinterpret_cast<uintptr_t>(address);
  printf("{\"Address\":%" PRIuPTR
         ",\"Expected\":\"%s\",\"Line\": %d,\"IsObj\":false}\n",
         val,
         expected,
         line);
}

}  // namespace another::one

extern "C" long pw_extern_long = 42;

int main() {
  PrintExpectedObjSymbol(&pw::symbolizer::test::gerald,
                         "pw::symbolizer::test::gerald");
  PrintExpectedObjSymbol(&pw::symbolizer::test::gerald.tastes_like_chicken,
                         "pw::symbolizer::test::gerald");
  another::one::PrintExpectedFuncSymbol(
      reinterpret_cast<void*>(&another::one::PrintExpectedFuncSymbol),
      "another::one::PrintExpectedFuncSymbol(void*, char const*, int)",
      50);

  another::one::PrintExpectedFuncSymbol(
      reinterpret_cast<void*>(&GoWild), "(anonymous namespace)::GoWild()", 39);

  PrintExpectedObjSymbol(&pw_extern_long, "pw_extern_long");

  another::one::PrintExpectedFuncSymbol(
      reinterpret_cast<void*>(&main), "main", 63);
  return 0;
}
