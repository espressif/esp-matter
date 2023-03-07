# Copyright 2020 The Pigweed Authors
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
include($ENV{PW_ROOT}/pw_chrono/backend.cmake)
include($ENV{PW_ROOT}/pw_sync/backend.cmake)
include($ENV{PW_ROOT}/pw_protobuf_compiler/proto.cmake)

pw_add_library(pw_chrono.epoch INTERFACE
  HEADERS
    public/pw_chrono/epoch.h
  PUBLIC_INCLUDES
    public
)

pw_add_facade(pw_chrono.system_clock STATIC
  BACKEND
    pw_chrono.system_clock_BACKEND
  HEADERS
    public/pw_chrono/internal/system_clock_macros.h
    public/pw_chrono/system_clock.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_chrono.epoch
    pw_preprocessor
  SOURCES
    system_clock.cc
)

pw_add_facade(pw_chrono.system_timer INTERFACE
  BACKEND
    pw_chrono.system_timer_BACKEND
  HEADERS
    public/pw_chrono/system_timer.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_chrono.system_clock
    pw_function
)

# Dependency injectable implementation of pw::chrono::SystemClock::Interface.
pw_add_library(pw_chrono.simulated_system_clock INTERFACE
  HEADERS
    public/pw_chrono/simulated_system_clock.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_chrono.system_clock
    pw_sync.interrupt_spin_lock
)

pw_proto_library(pw_chrono.protos
  SOURCES
    chrono.proto
  PREFIX
    pw_chrono_protos
)

if((NOT "${pw_chrono.system_clock_BACKEND}" STREQUAL "") AND
   (NOT "${pw_sync.interrupt_spin_lock_BACKEND}" STREQUAL ""))
  pw_add_test(pw_chrono.simulated_system_clock_test
    SOURCES
      simulated_system_clock_test.cc
    PRIVATE_DEPS
      pw_chrono.simulated_system_clock
    GROUPS
      modules
      pw_chrono
  )
endif()

if(NOT "${pw_chrono.system_clock_BACKEND}" STREQUAL "")
  pw_add_test(pw_chrono.system_clock_facade_test
    SOURCES
      system_clock_facade_test.cc
      system_clock_facade_test_c.c
    PRIVATE_DEPS
      pw_chrono.system_clock
      pw_preprocessor
    GROUPS
      modules
      pw_chrono
  )
endif()

if(NOT "${pw_chrono.system_timer_BACKEND}" STREQUAL "")
  pw_add_test(pw_chrono.system_timer_facade_test
    SOURCES
      system_timer_facade_test.cc
    PRIVATE_DEPS
      pw_chrono.system_timer
      pw_sync.thread_notification
    GROUPS
      modules
      pw_chrono
  )
endif()
