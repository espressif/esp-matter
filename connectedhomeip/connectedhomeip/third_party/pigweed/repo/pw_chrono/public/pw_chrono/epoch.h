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
#pragma once

namespace pw::chrono {

enum class Epoch {
  // The epoch is unkown and possibly even undefined in case the clock is not
  // always enabled and the epoch may reset over time.
  kUnknown,

  kTimeSinceBoot,

  // Time since 00:00:00 UTC, Thursday, 1 January 1970, including leap seconds.
  kUtcWallClock,

  // Time since 00:00:00, 6 January 1980 UTC. Leap seconds are not inserted into
  // GPS. Thus, every time a leap second is inserted into UTC, UTC falls another
  // second behind GPS.
  kGpsWallClock,

  // Time since 00:00:00, 1 January 1958, and is offset 10 seconds ahead of UTC
  // at that date (i.e., its epoch, 1958-01-01 00:00:00 TAI, is 1957-12-31
  // 23:59:50 UTC). Leap seconds are not inserted into TAI. Thus, every time a
  // leap second is inserted into UTC, UTC falls another second behind TAI.
  kTaiWallClock,
};

}  // namespace pw::chrono
