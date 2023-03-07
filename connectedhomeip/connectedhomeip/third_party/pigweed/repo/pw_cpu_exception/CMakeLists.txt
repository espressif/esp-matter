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
include($ENV{PW_ROOT}/pw_cpu_exception/backend.cmake)

pw_add_facade(pw_cpu_exception.entry INTERFACE
  BACKEND
    pw_cpu_exception.entry_BACKEND
  HEADERS
    public/pw_cpu_exception/entry.h
    public/pw_cpu_exception/state.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_preprocessor
)

pw_add_facade(pw_cpu_exception.handler STATIC
  BACKEND
    pw_cpu_exception.handler_BACKEND
  HEADERS
    public/pw_cpu_exception/handler.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_cpu_exception.entry
    pw_preprocessor
  SOURCES
    start_exception_handler.cc
)

pw_add_facade(pw_cpu_exception.support INTERFACE
  BACKEND
    pw_cpu_exception.support_BACKEND
  HEADERS
    public/pw_cpu_exception/support.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_cpu_exception.entry
)

pw_add_library(pw_cpu_exception.basic_handler STATIC
  SOURCES
    basic_handler.cc
  PRIVATE_DEPS
    pw_cpu_exception.entry
    pw_log
)
