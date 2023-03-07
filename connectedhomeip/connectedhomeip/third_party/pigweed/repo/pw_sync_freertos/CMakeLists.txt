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

pw_add_module_config(pw_sync_freertos_CONFIG)

pw_add_library(pw_sync_freertos.config INTERFACE
  HEADERS
    public/pw_sync_freertos/config.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_third_party.freertos
    ${pw_sync_freertos_CONFIG}
)

# TODO(ewout): Add system_clock backend compatibility check like in GN.

# This target provides the backend for pw::sync::BinarySemaphore.
pw_add_library(pw_sync_freertos.binary_semaphore STATIC
  HEADERS
    public/pw_sync_freertos/binary_semaphore_inline.h
    public/pw_sync_freertos/binary_semaphore_native.h
    public_overrides/pw_sync_backend/binary_semaphore_inline.h
    public_overrides/pw_sync_backend/binary_semaphore_native.h
  PUBLIC_INCLUDES
    public
    public_overrides
  PUBLIC_DEPS
    pw_assert
    pw_chrono.system_clock
    pw_chrono_freertos.system_clock
    pw_interrupt.context
    pw_sync.binary_semaphore.facade
    pw_third_party.freertos
  SOURCES
    binary_semaphore.cc
)

# This target provides the backend for pw::sync::CountingSemaphore.
pw_add_library(pw_sync_freertos.counting_semaphore STATIC
  HEADERS
    public/pw_sync_freertos/counting_semaphore_inline.h
    public/pw_sync_freertos/counting_semaphore_native.h
    public_overrides/pw_sync_backend/counting_semaphore_inline.h
    public_overrides/pw_sync_backend/counting_semaphore_native.h
  PUBLIC_INCLUDES
    public
    public_overrides
  PUBLIC_DEPS
    pw_assert
    pw_chrono.system_clock
    pw_chrono_freertos.system_clock
    pw_interrupt.context
    pw_sync.counting_semaphore.facade
    pw_third_party.freertos
  SOURCES
    counting_semaphore.cc
)

# This target provides the backend for pw::sync::Mutex.
pw_add_library(pw_sync_freertos.mutex INTERFACE
  HEADERS
    public/pw_sync_freertos/mutex_inline.h
    public/pw_sync_freertos/mutex_native.h
    public_overrides/pw_sync_backend/mutex_inline.h
    public_overrides/pw_sync_backend/mutex_native.h
  PUBLIC_INCLUDES
    public
    public_overrides
  PUBLIC_DEPS
    pw_assert
    pw_interrupt.context
    pw_sync.mutex.facade
    pw_third_party.freertos
)

# This target provides the backend for pw::sync::TimedMutex.
pw_add_library(pw_sync_freertos.timed_mutex STATIC
  HEADERS
    public/pw_sync_freertos/timed_mutex_inline.h
    public_overrides/pw_sync_backend/timed_mutex_inline.h
  PUBLIC_INCLUDES
    public
    public_overrides
  PUBLIC_DEPS
    pw_chrono.system_clock
    pw_sync.timed_mutex.facade
  SOURCES
    timed_mutex.cc
  PRIVATE_DEPS
    pw_assert
    pw_chrono_freertos.system_clock
    pw_interrupt.context
    pw_third_party.freertos
)

# This target provides the backend for pw::sync::ThreadNotification.
pw_add_library(pw_sync_freertos.thread_notification STATIC
  HEADERS
    public/pw_sync_freertos/thread_notification_inline.h
    public/pw_sync_freertos/thread_notification_native.h
    public_overrides/thread_notification/pw_sync_backend/thread_notification_inline.h
    public_overrides/thread_notification/pw_sync_backend/thread_notification_native.h
  PUBLIC_INCLUDES
    public
    public_overrides/thread_notification
  PUBLIC_DEPS
    pw_assert
    pw_interrupt.context
    pw_polyfill
    pw_sync.interrupt_spin_lock
    pw_sync.lock_annotations
    pw_sync.thread_notification.facade
    pw_third_party.freertos
  SOURCES
    thread_notification.cc
  PRIVATE_DEPS
    pw_sync_freertos.config
)

# This target provides the backend for pw::sync::TimedThreadNotification.
pw_add_library(pw_sync_freertos.timed_thread_notification STATIC
  HEADERS
    public/pw_sync_freertos/timed_thread_notification_inline.h
    public_overrides/timed_thread_notification/pw_sync_backend/timed_thread_notification_inline.h
  PUBLIC_INCLUDES
    public
    public_overrides/timed_thread_notification
  PUBLIC_DEPS
    pw_chrono.system_clock
    pw_sync.interrupt_spin_lock
    pw_sync.timed_thread_notification.facade
  SOURCES
    timed_thread_notification.cc
  PRIVATE_DEPS
    pw_sync_freertos.config
    pw_assert
    pw_chrono_freertos.system_clock
    pw_interrupt.context
    pw_third_party.freertos
)

# This target provides the backend for pw::sync::InterruptSpinLock.
pw_add_library(pw_sync_freertos.interrupt_spin_lock STATIC
  HEADERS
    public/pw_sync_freertos/interrupt_spin_lock_inline.h
    public/pw_sync_freertos/interrupt_spin_lock_native.h
    public_overrides/pw_sync_backend/interrupt_spin_lock_inline.h
    public_overrides/pw_sync_backend/interrupt_spin_lock_native.h
  PUBLIC_INCLUDES
    public
    public_overrides
  PUBLIC_DEPS
    pw_sync.interrupt_spin_lock.facade
    pw_third_party.freertos
  SOURCES
    interrupt_spin_lock.cc
  PRIVATE_DEPS
    pw_assert
    pw_interrupt.context
)
