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

// The C implementation of this macro requires a C99 compound literal. In C++,
// avoid the compound literal in case -Wc99-extensions is enabled.
#ifdef __cplusplus
#define _PW_SYSTEM_CLOCK_DURATION(num_ticks) \
  (pw_chrono_SystemClock_Duration{.ticks = (num_ticks)})
#else
#define _PW_SYSTEM_CLOCK_DURATION(num_ticks) \
  ((pw_chrono_SystemClock_Duration){.ticks = (num_ticks)})
#endif  // __cplusplus

// clang-format off

// ticks_ceil = ((count * clock_period_den + time_unit_num - 1) * time_unit_den) /
//              (clock_period_num * time_unit_num)
#define _PW_SYSTEM_CLOCK_TIME_TO_DURATION_CEIL(                                                                                                    \
    count, time_unit_seconds_numerator, time_unit_seconds_denominator)                                                                             \
  _PW_SYSTEM_CLOCK_DURATION(                                                                                                                       \
      (((int64_t)(count) * PW_CHRONO_SYSTEM_CLOCK_PERIOD_SECONDS_DENOMINATOR + time_unit_seconds_numerator - 1) * time_unit_seconds_denominator) / \
       (PW_CHRONO_SYSTEM_CLOCK_PERIOD_SECONDS_NUMERATOR * time_unit_seconds_numerator))

// ticks_floor = (count * clock_period_den * time_unit_den) /
//               (clock_period_num * time_unit_num)
#define _PW_SYSTEM_CLOCK_TIME_TO_DURATION_FLOOR(                                                               \
    count, time_unit_seconds_numerator, time_unit_seconds_denominator)                                         \
  _PW_SYSTEM_CLOCK_DURATION(                                                                                   \
      ((int64_t)(count) * PW_CHRONO_SYSTEM_CLOCK_PERIOD_SECONDS_DENOMINATOR * time_unit_seconds_denominator) / \
      (PW_CHRONO_SYSTEM_CLOCK_PERIOD_SECONDS_NUMERATOR * time_unit_seconds_numerator))


#define PW_SYSTEM_CLOCK_MS_CEIL(milliseconds) \
  _PW_SYSTEM_CLOCK_TIME_TO_DURATION_CEIL(milliseconds, 1000, 1)
#define PW_SYSTEM_CLOCK_S_CEIL(seconds) \
  _PW_SYSTEM_CLOCK_TIME_TO_DURATION_CEIL(seconds,         1, 1)
#define PW_SYSTEM_CLOCK_MIN_CEIL(minutes) \
  _PW_SYSTEM_CLOCK_TIME_TO_DURATION_CEIL(minutes,         1, 60)
#define PW_SYSTEM_CLOCK_H_CEIL(hours) \
  _PW_SYSTEM_CLOCK_TIME_TO_DURATION_CEIL(hours,           1, 60 * 60)

#define PW_SYSTEM_CLOCK_MS_FLOOR(milliseconds) \
  _PW_SYSTEM_CLOCK_TIME_TO_DURATION_FLOOR(milliseconds, 1000, 1)
#define PW_SYSTEM_CLOCK_S_FLOOR(seconds) \
  _PW_SYSTEM_CLOCK_TIME_TO_DURATION_FLOOR(seconds,         1, 1)
#define PW_SYSTEM_CLOCK_MIN_FLOOR(minutes) \
  _PW_SYSTEM_CLOCK_TIME_TO_DURATION_FLOOR(minutes,         1, 60)
#define PW_SYSTEM_CLOCK_H_FLOOR(hours) \
  _PW_SYSTEM_CLOCK_TIME_TO_DURATION_FLOOR(hours,           1, 60 * 60)

// clang-format on

#define PW_SYSTEM_CLOCK_MS(milliseconds) PW_SYSTEM_CLOCK_MS_CEIL(milliseconds)
#define PW_SYSTEM_CLOCK_S(seconds) PW_SYSTEM_CLOCK_S_CEIL(seconds)
#define PW_SYSTEM_CLOCK_MIN(minutes) PW_SYSTEM_CLOCK_MIN_CEIL(minutes)
#define PW_SYSTEM_CLOCK_H(hours) PW_SYSTEM_CLOCK_H_CEIL(hours)
