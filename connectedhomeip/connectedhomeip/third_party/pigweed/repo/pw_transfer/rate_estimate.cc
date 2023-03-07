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

#include "pw_transfer/rate_estimate.h"

namespace pw::transfer {

size_t RateEstimate::GetRateBytesPerSecond() const {
  if (!start_time_.has_value()) {
    return 0;
  }

  auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
      chrono::SystemClock::now() - start_time_.value());
  if (elapsed_ms.count() == 0) {
    return 0;
  }

  constexpr unsigned int kMillsecondsPerSecond = 1000;
  return (static_cast<uint64_t>(bytes_transferred_) * kMillsecondsPerSecond) /
         elapsed_ms.count();
}

}  // namespace pw::transfer
