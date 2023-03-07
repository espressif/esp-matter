# Copyright 2022 The Pigweed Authors
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.

include($ENV{PW_ROOT}/pw_build/pigweed.cmake)

pw_add_module_config(pw_chrono_freertos_CONFIG)

pw_add_library(pw_chrono_freertos.config INTERFACE
  HEADERS
    public/pw_chrono_freertos/config.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_third_party.freertos
    ${pw_chrono_freertos_CONFIG}
)

# This target provides the backend for pw::chrono::SystemClock.
pw_add_library(pw_chrono_freertos.system_clock STATIC
  HEADERS
    public/pw_chrono_freertos/system_clock_config.h
    public/pw_chrono_freertos/system_clock_constants.h
    public_overrides/pw_chrono_backend/system_clock_config.h
  PUBLIC_INCLUDES
    public
    public_overrides
  PUBLIC_DEPS
    pw_chrono.epoch
    pw_chrono.system_clock.facade
    pw_chrono_freertos.config
    pw_third_party.freertos
  SOURCES
    system_clock.cc
  PRIVATE_DEPS
    pw_interrupt.context
    pw_sync.interrupt_spin_lock
)

# This target provides the backend for pw::chrono::SystemTimer.
pw_add_library(pw_chrono_freertos.system_timer STATIC
  HEADERS
    public/pw_chrono_freertos/system_timer_inline.h
    public/pw_chrono_freertos/system_timer_native.h
    public_overrides/pw_chrono_backend/system_timer_inline.h
    public_overrides/pw_chrono_backend/system_timer_native.h
  PUBLIC_INCLUDES
    public
    public_overrides
  PUBLIC_DEPS
    pw_chrono.system_clock
    pw_chrono.system_timer.facade
    pw_third_party.freertos
  SOURCES
    system_timer.cc
  PRIVATE_DEPS
    pw_assert
    pw_chrono_freertos.system_clock
)
