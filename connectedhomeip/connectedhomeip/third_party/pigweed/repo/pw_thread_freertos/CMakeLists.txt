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
include($ENV{PW_ROOT}/pw_thread_freertos/backend.cmake)

pw_add_module_config(pw_thread_freertos_CONFIG)

pw_add_library(pw_thread_freertos.config INTERFACE
  HEADERS
    public/pw_thread_freertos/config.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_third_party.freertos
    ${pw_thread_freertos_CONFIG}
)

# This target provides the backend for pw::thread::Id & pw::this_thread::get_id.
pw_add_library(pw_thread_freertos.id INTERFACE
  HEADERS
    public/pw_thread_freertos/id_inline.h
    public/pw_thread_freertos/id_native.h
    id_public_overrides/pw_thread_backend/id_inline.h
    id_public_overrides/pw_thread_backend/id_native.h
  PUBLIC_INCLUDES
    public
    id_public_overrides
  PUBLIC_DEPS
    pw_assert
    pw_interrupt.context
    pw_third_party.freertos
    pw_thread.id.facade
)

# This target provides the backend for pw::this_thread::sleep_{for,until}.
pw_add_library(pw_thread_freertos.sleep STATIC
  HEADERS
    public/pw_thread_freertos/sleep_inline.h
    sleep_public_overrides/pw_thread_backend/sleep_inline.h
  PUBLIC_INCLUDES
    public
    sleep_public_overrides
  PUBLIC_DEPS
    pw_chrono.system_clock
    pw_thread.sleep.facade
  SOURCES
    sleep.cc
  PRIVATE_DEPS
    pw_assert
    pw_chrono_freertos.system_clock
    pw_third_party.freertos
    pw_thread.id
)

# This target provides the backend for pw::thread::Thread and the headers needed
# for thread creation.
pw_add_library(pw_thread_freertos.thread STATIC
  HEADERS
    public/pw_thread_freertos/context.h
    public/pw_thread_freertos/options.h
    public/pw_thread_freertos/thread_inline.h
    public/pw_thread_freertos/thread_native.h
    thread_public_overrides/pw_thread_backend/thread_inline.h
    thread_public_overrides/pw_thread_backend/thread_native.h
  PUBLIC_INCLUDES
    public
    thread_public_overrides
  PUBLIC_DEPS
    pw_assert
    pw_span
    pw_string
    pw_third_party.freertos
    pw_thread.id
    pw_thread.thread.facade
    pw_thread_freertos.config
  SOURCES
    thread.cc
)

# This target provides the backend for pw::this_thread::yield.
pw_add_library(pw_thread_freertos.yield INTERFACE
  HEADERS
    public/pw_thread_freertos/yield_inline.h
    yield_public_overrides/pw_thread_backend/yield_inline.h
  PUBLIC_INCLUDES
    public
    yield_public_overrides
  PUBLIC_DEPS
    pw_assert
    pw_third_party.freertos
    pw_thread.id
    pw_thread.yield.facade
)

pw_add_library(pw_thread_freertos.util STATIC
  HEADERS
    public/pw_thread_freertos/util.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_third_party.freertos
    pw_function
    pw_span
    pw_status
  SOURCES
    util.cc
  PRIVATE_DEPS
    pw_log
)

pw_add_library(pw_thread_freertos.snapshot STATIC
  HEADERS
    public/pw_thread_freertos/snapshot.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_function
    pw_span
    pw_protobuf
    pw_status
    pw_third_party.freertos
    pw_thread.protos.pwpb
    pw_thread.snapshot
    pw_thread_freertos.config
  SOURCES
    snapshot.cc
  PRIVATE_DEPS
    pw_thread_freertos.freertos_tsktcb
    pw_thread_freertos.util
    pw_log
)

pw_add_facade(pw_thread_freertos.freertos_tsktcb INTERFACE
  BACKEND
    pw_thread_freertos.freertos_tsktcb_BACKEND
  HEADERS
    public/pw_thread_freertos/freertos_tsktcb.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_third_party.freertos
)
