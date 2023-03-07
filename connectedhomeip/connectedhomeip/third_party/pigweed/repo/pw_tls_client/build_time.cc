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

#include "build_time.h"

#include <sys/time.h>

namespace {
constexpr uint64_t kMicrosecondsPerSecond = 1'000'000;
}

#if __cplusplus
extern "C" {
#endif

time_t __wrap_time(time_t* t) {
  time_t ret =
      static_cast<time_t>(kBuildTimeMicrosecondsUTC / kMicrosecondsPerSecond);
  if (t) {
    *t = ret;
  }
  return ret;
}

int __wrap_gettimeofday(struct timeval* tv, void* tz) {
  // The use of the timezone structure is obsolete (see docs "man
  // gettimeofday"). Thus we don't consider it.
  (void)tz;
  tv->tv_sec = kBuildTimeMicrosecondsUTC / kMicrosecondsPerSecond;
  tv->tv_usec = kBuildTimeMicrosecondsUTC % kMicrosecondsPerSecond;
  return 0;
}

#if __cplusplus
}
#endif
