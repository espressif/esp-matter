# Copyright 2021 The Pigweed Authors
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

pw_add_module_config(pw_multisink_CONFIG)

pw_add_library(pw_multisink.config INTERFACE
  HEADERS
    public/pw_multisink/config.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    ${pw_multisink_CONFIG}
)

pw_add_library(pw_multisink STATIC
  HEADERS
    public/pw_multisink/multisink.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_bytes
    pw_containers
    pw_function
    pw_multisink.config
    pw_result
    pw_ring_buffer
    pw_status
    pw_sync.interrupt_spin_lock
    pw_sync.lock_annotations
    pw_sync.mutex
  SOURCES
    multisink.cc
  PRIVATE_DEPS
    pw_assert
    pw_log
    pw_varint
)

pw_add_library(pw_multisink.util STATIC
  HEADERS
    public/pw_multisink/util.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_log.protos.pwpb
    pw_multisink
    pw_status
  SOURCES
    util.cc
  PRIVATE_DEPS
    pw_bytes
    pw_function
)

pw_add_library(pw_multisink.test_thread INTERFACE
  HEADERS
    public/pw_multisink/test_thread.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_thread.thread
)

# Tests that use threads.
# To instantiate this test based on a thread backend, create a pw_add_test
# target that depends on this pw_add_module_library and a pw_add_module_library
# that provides the implementaiton of pw_multisink.test_thread. See
# pw_multisink.stl_multisink_test as an example.
pw_add_library(pw_multisink.multisink_threaded_test STATIC
  SOURCES
    multisink_threaded_test.cc
  PRIVATE_DEPS
    pw_multisink
    pw_multisink.test_thread
    pw_string
    pw_thread.thread
    pw_thread.yield
    pw_unit_test
)

pw_add_test(pw_multisink.multisink_test
  SOURCES
    multisink_test.cc
  PRIVATE_DEPS
    pw_function
    pw_multisink
    pw_span
    pw_status
  GROUPS
    modules
    pw_multisink
)

pw_add_library(pw_multisink.stl_test_thread STATIC
  SOURCES
    stl_test_thread.cc
  PRIVATE_DEPS
    pw_multisink.test_thread
    pw_thread.thread
    pw_thread_stl.thread
)

if("${pw_thread.thread_BACKEND}" STREQUAL "pw_thread_stl.thread")
  pw_add_test(pw_multisink.stl_multisink_threaded_test
    PRIVATE_DEPS
      pw_span
      pw_multisink.multisink_threaded_test
      pw_multisink.stl_test_thread
    GROUPS
      modules
      pw_multisink
  )
endif()
