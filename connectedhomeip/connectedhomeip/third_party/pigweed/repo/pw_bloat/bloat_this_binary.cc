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

#include "pw_bloat/bloat_this_binary.h"

#include <cstring>

#include "pw_assert/assert.h"
#include "pw_assert/check.h"
#include "pw_log/log.h"

namespace pw::bloat {

char* volatile non_optimizable_pointer;

void BloatThisBinary() {
  [[maybe_unused]] volatile unsigned counter = 0;

  // In case someone accidentally ends up flashing and running a bloat
  // executable on their device, loop forever instead of running this code.
  volatile bool clearly_false_condition = true;
  while (clearly_false_condition) {
    counter = counter + 1;
  }

  // This code uses standard C/C++ functions such as memcpy to prevent them from
  // showing up in size report deltas against a barebones base executable.
  //
  // This is done using garbage memory addresses as it consistently prevents the
  // compiler from optimizing out parts of the code. Other approaches, such as a
  // buffer, occasionally ran into optimization issues.
  const char* s = "The quick brown fox jumps over the lazy dog.";

  // Making the copy size large forces the compiler to generate a memcpy
  // function instead of inlining it.
  constexpr int kRandomLargeNumber = 2398;
  std::memcpy(non_optimizable_pointer,
              non_optimizable_pointer + std::strlen(s),
              kRandomLargeNumber);

  std::memmove(non_optimizable_pointer + 18,
               non_optimizable_pointer,
               kRandomLargeNumber);

  *non_optimizable_pointer = std::strlen(non_optimizable_pointer);

  // This code ensures users do not have to pay for the base cost of using
  // asserts and logging.
  PW_ASSERT(*non_optimizable_pointer);
  PW_DASSERT(*non_optimizable_pointer);
  PW_CHECK_INT_GE(*non_optimizable_pointer, 0, "Ensure this logic stays");
  PW_DCHECK_INT_GE(*non_optimizable_pointer, 0, "Ensure this logic stays");
  PW_LOG_INFO("We care about optimizing: %d", *non_optimizable_pointer);
}

}  // namespace pw::bloat
