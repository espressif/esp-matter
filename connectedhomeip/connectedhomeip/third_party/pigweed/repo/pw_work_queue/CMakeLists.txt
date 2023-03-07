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

pw_add_library(pw_work_queue STATIC
  HEADERS
    public/pw_work_queue/internal/circular_buffer.h
    public/pw_work_queue/work_queue.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_sync.interrupt_spin_lock
    pw_sync.lock_annotations
    pw_sync.thread_notification
    pw_thread.thread
    pw_function
    pw_metric
    pw_span
    pw_status
  SOURCES
    work_queue.cc
)

pw_add_library(pw_work_queue.test_thread INTERFACE
  HEADERS
    public/pw_work_queue/test_thread.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_thread.thread
)

# To instantiate this test based on a selected thread backend to provide
# test_thread you can create a pw_add_test which depends on this
# pw_add_library and a pw_add_library which provides the implementation of
# test_thread. See pw_work_queue.stl_work_queue_test as an example.
pw_add_library(pw_work_queue.work_queue_test STATIC
  SOURCES
    work_queue_test.cc
  PRIVATE_DEPS
    pw_work_queue
    pw_work_queue.test_thread
    pw_log
    pw_unit_test
)

pw_add_library(pw_work_queue.stl_test_thread STATIC
  SOURCES
    stl_test_thread.cc
  PRIVATE_DEPS
    pw_work_queue.test_thread
    pw_thread.thread
    pw_thread_stl.thread
)

if("${pw_thread.thread_BACKEND}" STREQUAL "pw_thread_stl.thread")
  pw_add_test(pw_work_queue.stl_work_queue_test
    PRIVATE_DEPS
      pw_work_queue.stl_test_thread
      pw_work_queue.work_queue_test
    GROUPS
      modules
      pw_work_queue
  )
endif()
