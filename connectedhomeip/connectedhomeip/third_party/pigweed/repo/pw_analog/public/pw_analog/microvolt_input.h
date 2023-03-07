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
#include "pw_analog/analog_input.h"
#include "pw_chrono/system_clock.h"
#include "pw_result/result.h"
#include "pw_status/try.h"

namespace pw::analog {

// The common interface for obtaining voltage samples in microvolts. This
// interface represents a single voltage input or channel. Users will need to
// supply their own ADC driver implementation in order to configure and enable
// the ADC peripheral in order to provide the reference voltages and to
// configure and enable the ADC peripheral where needed. Users are responsible
// for managing multithreaded access to the ADC driver if the ADC services
// multiple channels.
class MicrovoltInput : public AnalogInput {
 public:
  // Specifies the max and min microvolt range the analog input can measure.
  // The reference voltage difference cannot be bigger than sizeof(int32_t)
  // which should be just above 2000V.
  // * These values do not change at run time.
  // * Inversion of min/max is supported.
  struct References {
    int32_t max_voltage_uv;  // Microvolts at AnalogInput::Limits::max
    int32_t min_voltage_uv;  // Microvolts at AnalogInput::Limits::min.
  };

  ~MicrovoltInput() override = default;

  // Blocks until the specified timeout duration has elapsed or the voltage
  // sample has been returned, whichever comes first.
  //
  // This method is thread safe.
  //
  // Returns:
  //   Microvolts (uV).
  //   ResourceExhuasted: ADC peripheral in use.
  //   DeadlineExceedded: Timed out waiting for a sample.
  //   Other statuses left up to the implementer.
  Result<int32_t> TryReadMicrovoltsFor(chrono::SystemClock::duration timeout) {
    return TryReadMicrovoltsUntil(
        chrono::SystemClock::TimePointAfterAtLeast(timeout));
  }

  // Blocks until the deadline time has been reached or the voltage sample has
  // been returned, whichever comes first.
  //
  // This method is thread safe.
  //
  // Returns:
  //   Microvolts (uV).
  //   ResourceExhuasted: ADC peripheral in use.
  //   DeadlineExceedded: Timed out waiting for a sample.
  //   Other statuses left up to the implementer.
  Result<int32_t> TryReadMicrovoltsUntil(
      chrono::SystemClock::time_point deadline) {
    PW_TRY_ASSIGN(const int32_t sample, TryReadUntil(deadline));

    const References reference = GetReferences();
    const AnalogInput::Limits limits = GetLimits();

    constexpr int64_t kMaxReferenceDiffUv = std::numeric_limits<int32_t>::max();

    if (std::abs(static_cast<int64_t>(reference.max_voltage_uv) -
                 static_cast<int64_t>(reference.min_voltage_uv)) >
        kMaxReferenceDiffUv) {
      return pw::Status::Internal();
    }

    return (((static_cast<int64_t>(sample) - static_cast<int64_t>(limits.min)) *
             (reference.max_voltage_uv - reference.min_voltage_uv)) /
            (limits.max - limits.min)) +
           reference.min_voltage_uv;
  }

 private:
  // Returns the reference voltage needed to calculate the voltage.
  // These values do not change at run time.
  virtual References GetReferences() const = 0;
};

}  // namespace pw::analog
