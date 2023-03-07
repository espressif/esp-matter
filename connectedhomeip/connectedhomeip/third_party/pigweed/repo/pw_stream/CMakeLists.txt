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

pw_add_library(pw_stream STATIC
  HEADERS
    public/pw_stream/memory_stream.h
    public/pw_stream/null_stream.h
    public/pw_stream/seek.h
    public/pw_stream/stream.h
  PUBLIC_INCLUDES
    public
  SOURCES
    memory_stream.cc
  PUBLIC_DEPS
    pw_assert
    pw_bytes
    pw_polyfill
    pw_result
    pw_span
    pw_status
)
if(Zephyr_FOUND AND CONFIG_PIGWEED_STREAM)
  zephyr_link_libraries(pw_stream)
endif()

pw_add_library(pw_stream.socket_stream STATIC
  HEADERS
    public/pw_stream/socket_stream.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_stream
  SOURCES
    socket_stream.cc
  PRIVATE_DEPS
    pw_log
)

pw_add_library(pw_stream.sys_io_stream INTERFACE
  HEADERS
    public/pw_stream/sys_io_stream.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_stream
    pw_sys_io
)

pw_add_library(pw_stream.std_file_stream STATIC
  HEADERS
    public/pw_stream/std_file_stream.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_stream
  SOURCES
    std_file_stream.cc
)

pw_add_library(pw_stream.interval_reader STATIC
  HEADERS
    public/pw_stream/interval_reader.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_assert
    pw_status
    pw_stream
  SOURCES
    interval_reader.cc
)

pw_add_test(pw_stream.memory_stream_test
  SOURCES
    memory_stream_test.cc
  PRIVATE_DEPS
    pw_stream
  GROUPS
    modules
    pw_stream
)

pw_add_test(pw_stream.seek_test
  SOURCES
    seek_test.cc
  PRIVATE_DEPS
    pw_stream
  GROUPS
    modules
    pw_stream
)

pw_add_test(pw_stream.stream_test
  SOURCES
    stream_test.cc
  PRIVATE_DEPS
    pw_stream
  GROUPS
    modules
    pw_stream
)

pw_add_test(pw_stream.interval_reader_test
  SOURCES
    interval_reader_test.cc
  PRIVATE_DEPS
    pw_stream.interval_reader
  GROUPS
    modules
    pw_stream
)
