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

pw_add_library(pw_varint STATIC
  HEADERS
    public/pw_varint/varint.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_preprocessor
    pw_span
  SOURCES
    varint.cc
)
if(Zephyr_FOUND AND CONFIG_PIGWEED_VARINT)
  zephyr_link_libraries(pw_varint)
endif()

pw_add_library(pw_varint.stream STATIC
  HEADERS
    public/pw_varint/stream.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_status
    pw_stream
  SOURCES
    stream.cc
  PRIVATE_DEPS
    pw_varint
)

pw_add_test(pw_varint.varint_test
  SOURCES
    varint_test.cc
    varint_test_c.c
  PRIVATE_DEPS
    pw_varint
  GROUPS
    modules
    pw_varint
)

pw_add_test(pw_varint.stream_test
  SOURCES
    stream_test.cc
  PRIVATE_DEPS
    pw_stream
    pw_varint.stream
  GROUPS
    modules
    pw_varint
)
