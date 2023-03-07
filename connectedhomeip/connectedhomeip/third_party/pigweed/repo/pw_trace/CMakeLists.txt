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
include($ENV{PW_ROOT}/pw_trace/backend.cmake)

pw_add_facade(pw_trace INTERFACE
  BACKEND
    pw_trace_BACKEND
  HEADERS
    public/pw_trace/internal/trace_internal.h
    public/pw_trace/trace.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_preprocessor
)

pw_add_library(pw_trace.null INTERFACE
  HEADERS
    public/pw_trace/internal/null.h
    public_overrides/pw_trace_backend/trace_backend.h
  PUBLIC_INCLUDES
    public
    public_overrides
  PUBLIC_DEPS
    pw_preprocessor
)

pw_add_test(pw_trace.pw_trace_null_test
  SOURCES
    trace_null_test.cc
    trace_null_test_c.c
  PRIVATE_DEPS
    pw_trace.null
  GROUPS
    modules
    pw_trace
)

if(NOT "${pw_trace_BACKEND}" STREQUAL "")
  pw_add_test(pw_trace.trace_backend_compile_test
   SOURCES
    trace_backend_compile_test.cc
    trace_backend_compile_test_c.c
   PRIVATE_DEPS
    pw_trace
  GROUPS
    modules
    pw_trace
  )
endif()
