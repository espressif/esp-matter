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

# This target provides the backend for pw::thread::Id & pw::this_thread::get_id.
pw_add_library(pw_thread_stl.id INTERFACE
  HEADERS
    public/pw_thread_stl/id_inline.h
    public/pw_thread_stl/id_native.h
    id_public_overrides/pw_thread_backend/id_inline.h
    id_public_overrides/pw_thread_backend/id_native.h
  PUBLIC_INCLUDES
    public
    id_public_overrides
  PUBLIC_DEPS
    pw_thread.id.facade
)

# This target provides the backend for pw::thread::Thread with joining
# joining capability.
pw_add_library(pw_thread_stl.thread INTERFACE
  HEADERS
    public/pw_thread_stl/options.h
    public/pw_thread_stl/thread_inline.h
    public/pw_thread_stl/thread_native.h
    thread_public_overrides/pw_thread_backend/thread_inline.h
    thread_public_overrides/pw_thread_backend/thread_native.h
  PUBLIC_INCLUDES
    public
    thread_public_overrides
  PUBLIC_DEPS
    pw_thread.thread.facade
)


# This target provides the backend for pw::this_thread::sleep_{for,until}.
pw_add_library(pw_thread_stl.sleep INTERFACE
  HEADERS
    public/pw_thread_stl/sleep_inline.h
    sleep_public_overrides/pw_thread_backend/sleep_inline.h
  PUBLIC_INCLUDES
    public
    sleep_public_overrides
  PUBLIC_DEPS
    pw_chrono.system_clock
    pw_thread.sleep.facade
)

# This target provides the backend for pw::this_thread::yield.
pw_add_library(pw_thread_stl.yield INTERFACE
  HEADERS
    public/pw_thread_stl/yield_inline.h
    yield_public_overrides/pw_thread_backend/yield_inline.h
  PUBLIC_INCLUDES
    public
    yield_public_overrides
  PUBLIC_DEPS
    pw_thread.yield.facade
)

pw_add_library(pw_thread_stl.test_threads STATIC
  PUBLIC_DEPS
    pw_thread.test_threads
  SOURCES
    test_threads.cc
  PRIVATE_DEPS
    pw_thread.thread
)

if(("${pw_thread.thread_BACKEND}" STREQUAL "pw_thread_stl.thread") AND
   (NOT "${pw_thread.sleep_BACKEND}" STREQUAL ""))
  pw_add_test(pw_thread_stl.thread_backend_test
    PRIVATE_DEPS
      pw_thread_stl.test_threads
      pw_thread.thread_facade_test
    GROUPS
      modules
      pw_thread_stl
  )
endif()
