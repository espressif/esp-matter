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

# This target provides the backend for pw::sync::BinarySemaphore.
pw_add_library(pw_sync_stl.binary_semaphore_backend STATIC
  HEADERS
    public/pw_sync_stl/binary_semaphore_inline.h
    public/pw_sync_stl/binary_semaphore_native.h
    public_overrides/pw_sync_backend/binary_semaphore_inline.h
    public_overrides/pw_sync_backend/binary_semaphore_native.h
  PUBLIC_INCLUDES
    public
    public_overrides
  PUBLIC_DEPS
    pw_sync.binary_semaphore.facade
  SOURCES
    binary_semaphore.cc
  PRIVATE_DEPS
    pw_assert
    pw_chrono.system_clock
)

pw_add_library(pw_sync_stl.condition_variable_backend INTERFACE
  HEADERS
    public/pw_sync_stl/condition_variable_inline.h
    public/pw_sync_stl/condition_variable_native.h
    public_overrides/pw_sync_backend/condition_variable_inline.h
    public_overrides/pw_sync_backend/condition_variable_native.h
  PUBLIC_INCLUDES
    public
    public_overrides
  PUBLIC_DEPS
    pw_sync.condition_variable.facade
)

# This target provides the backend for pw::sync::CountingSemaphore.
pw_add_library(pw_sync_stl.counting_semaphore_backend STATIC
  HEADERS
    public/pw_sync_stl/counting_semaphore_inline.h
    public/pw_sync_stl/counting_semaphore_native.h
    public_overrides/pw_sync_backend/counting_semaphore_inline.h
    public_overrides/pw_sync_backend/counting_semaphore_native.h
  PUBLIC_INCLUDES
    public
    public_overrides
  PUBLIC_DEPS
    pw_sync.counting_semaphore.facade
  SOURCES
    counting_semaphore.cc
  PRIVATE_DEPS
    pw_assert
    pw_chrono.system_clock
)

# This target provides the backend for pw::sync::Mutex.
pw_add_library(pw_sync_stl.mutex_backend STATIC
  HEADERS
    public/pw_sync_stl/mutex_inline.h
    public/pw_sync_stl/mutex_native.h
    public_overrides/pw_sync_backend/mutex_inline.h
    public_overrides/pw_sync_backend/mutex_native.h
  PUBLIC_INCLUDES
    public
    public_overrides
  PUBLIC_DEPS
    pw_sync.mutex.facade
  SOURCES
    mutex.cc
  PRIVATE_DEPS
    pw_assert
)

# This target provides the backend for pw::sync::TimedMutex.
pw_add_library(pw_sync_stl.timed_mutex_backend INTERFACE
  HEADERS
    public/pw_sync_stl/timed_mutex_inline.h
    public_overrides/pw_sync_backend/timed_mutex_inline.h
  PUBLIC_INCLUDES
    public
    public_overrides
  PUBLIC_DEPS
    pw_sync.mutex
    pw_chrono.system_clock
    pw_sync.timed_mutex.facade
)

pw_add_library(pw_sync_stl.interrupt_spin_lock INTERFACE
  HEADERS
    public/pw_sync_stl/interrupt_spin_lock_inline.h
    public/pw_sync_stl/interrupt_spin_lock_native.h
    public_overrides/pw_sync_backend/interrupt_spin_lock_inline.h
    public_overrides/pw_sync_backend/interrupt_spin_lock_native.h
  PUBLIC_INCLUDES
    public
    public_overrides
  PUBLIC_DEPS
    pw_sync.interrupt_spin_lock.facade
    pw_sync.yield_core
)
