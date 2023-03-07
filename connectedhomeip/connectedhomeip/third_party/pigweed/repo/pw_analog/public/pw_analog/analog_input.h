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
#pragma once

#include "pw_chrono/system_clock.h"
#include "pw_result/result.h"

namespace pw::analog {

// Base interface for getting ADC samples from one ADC channel in a thread
// safe manner.
//
// The ADC backend interface is up to the user to define and implement for now.
// This gives the flexibility for the ADC driver implementation.
//
// AnalogInput controls a specific input/channel where the ADC peripheral may be
// shared across multiple channels that may be controlled by multiple threads.
// The implementer of this pure virtual interface is responsible for ensuring
// thread safety and access at the driver level.
class AnalogInput {
 public:
  // Limits struct that specifies the min and max of the sample range.
  // These values do not change at run time.
  struct Limits {
    int32_t min;
    int32_t max;
  };

  virtual ~AnalogInput() = default;

  // Blocks until the specified timeout duration has elapsed or the ADC sample
  // has been returned, whichever comes first.
  //
  // This method is thread safe.
  //
  // Returns:
  //   Sample.
  //   ResourceExhuasted: ADC peripheral in use.
  //   DeadlineExceedded: Timed out waiting for a sample.
  //   Other statuses left up to the implementer.
  Result<int32_t> TryReadFor(chrono::SystemClock::duration timeout) {
    return TryReadUntil(chrono::SystemClock::TimePointAfterAtLeast(timeout));
  }

  // Blocks until the deadline time has been reached or the ADC sample
  // has been returned, whichever comes first.
  //
  // This method is thread safe.
  //
  // Returns:
  //   Sample.
  //   ResourceExhuasted: ADC peripheral in use.
  //   DeadlineExceedded: Timed out waiting for a sample.
  //   Other statuses left up to the implementer.
  virtual Result<int32_t> TryReadUntil(
      chrono::SystemClock::time_point deadline) = 0;

  // Returns the range of the ADC sample.
  // These values do not change at run time.
  virtual Limits GetLimits() const = 0;
};

}  // namespace pw::analog
