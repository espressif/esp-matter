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
include($ENV{PW_ROOT}/pw_thread/backend.cmake)

pw_add_module_config(pw_thread_CONFIG)

pw_add_library(pw_thread.config INTERFACE
  HEADERS
    public/pw_thread/config.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    ${pw_thread_CONFIG}
)

pw_add_facade(pw_thread.id INTERFACE
  BACKEND
    pw_thread.id_BACKEND
  HEADERS
    public/pw_thread/id.h
  PUBLIC_INCLUDES
    public
)

pw_add_facade(pw_thread.sleep STATIC
  BACKEND
    pw_thread.sleep_BACKEND
  HEADERS
    public/pw_thread/sleep.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_chrono.system_clock
    pw_preprocessor
  SOURCES
    sleep.cc
)

pw_add_facade(pw_thread.thread STATIC
  BACKEND
    pw_thread.thread_BACKEND
  HEADERS
    public/pw_thread/detached_thread.h
    public/pw_thread/thread.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_thread.thread_core
    pw_thread.id
  SOURCES
    thread.cc
)

pw_add_library(pw_thread.thread_core INTERFACE
  HEADERS
    public/pw_thread/thread_core.h
  PUBLIC_INCLUDES
    public
)

pw_add_facade(pw_thread.yield STATIC
  BACKEND
    pw_thread.yield_BACKEND
  HEADERS
    public/pw_thread/yield.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_preprocessor
  SOURCES
    yield.cc
)

pw_add_library(pw_thread.snapshot STATIC
  HEADERS
    public/pw_thread/snapshot.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_bytes
    pw_function
    pw_protobuf
    pw_status
    pw_thread.protos.pwpb
  SOURCES
    snapshot.cc
  PRIVATE_DEPS
    pw_thread.config
    pw_log
)

pw_proto_library(pw_thread.protos
  SOURCES
    pw_thread_protos/thread.proto
  DEPS
    pw_tokenizer.proto
)

if(NOT "${pw_thread.id_BACKEND}" STREQUAL "")
  pw_add_test(pw_thread.id_facade_test
    SOURCES
      id_facade_test.cc
    PRIVATE_DEPS
      pw_thread.id
    GROUPS
      modules
      pw_thread
  )
endif()

if((NOT "${pw_thread.id_BACKEND}" STREQUAL "") AND
   (NOT "${pw_thread.sleep_BACKEND}" STREQUAL ""))
  pw_add_test(pw_thread.sleep_facade_test
    SOURCES
      sleep_facade_test.cc
      sleep_facade_test_c.c
    PRIVATE_DEPS
      pw_chrono.system_clock
      pw_thread.id
      pw_thread.sleep
    GROUPS
      modules
      pw_thread
  )
endif()

pw_add_library(pw_thread.test_threads INTERFACE
  HEADERS
    public/pw_thread/test_threads.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_thread.thread
)

# To instantiate this facade test based on a selected backend to provide
# test_threads you can create a pw_add_test target which depends on this
# target and a target which provides the implementation of
# test_threads. See pw_thread_stl.thread_backend_test as an example.
pw_add_library(pw_thread.thread_facade_test STATIC
  SOURCES
    thread_facade_test.cc
  PRIVATE_DEPS
    pw_thread.id
    pw_thread.sleep
    pw_thread.test_threads
    pw_thread.thread
    pw_sync.binary_semaphore
    pw_unit_test
)

if((NOT "${pw_thread.id_BACKEND}" STREQUAL "") AND
   (NOT "${pw_thread.yield_BACKEND}" STREQUAL ""))
  pw_add_test(pw_thread.yield_facade_test
    SOURCES
      yield_facade_test.cc
      yield_facade_test_c.c
    PRIVATE_DEPS
      pw_thread.id
      pw_thread.yield
    GROUPS
      modules
      pw_thread
  )
endif()
