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

pw_add_library(pw_bytes STATIC
  HEADERS
    public/pw_bytes/array.h
    public/pw_bytes/byte_builder.h
    public/pw_bytes/endian.h
    public/pw_bytes/span.h
    public/pw_bytes/units.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_bytes.bit
    pw_containers.iterator
    pw_polyfill
    pw_preprocessor
    pw_span
    pw_status
  SOURCES
    byte_builder.cc
)
if(Zephyr_FOUND AND CONFIG_PIGWEED_BYTES)
  zephyr_link_libraries(pw_bytes)
endif()

pw_add_library(pw_bytes.bit INTERFACE
  HEADERS
    public/pw_bytes/bit.h
  PUBLIC_INCLUDES
    public
)

pw_add_test(pw_bytes.array_test
  SOURCES
    array_test.cc
  PRIVATE_DEPS
    pw_bytes
  GROUPS
    modules
    pw_bytes
)

pw_add_test(pw_bytes.bit_test
  SOURCES
    bit_test.cc
  PRIVATE_DEPS
    pw_bytes.bit
  GROUPS
    modules
    pw_bytes
)

pw_add_test(pw_bytes.byte_builder_test
  SOURCES
    byte_builder_test.cc
  PRIVATE_DEPS
    pw_bytes
  GROUPS
    modules
    pw_bytes
)

pw_add_test(pw_bytes.endian_test
  SOURCES
    endian_test.cc
  PRIVATE_DEPS
    pw_bytes
  GROUPS
    modules
    pw_bytes
)

pw_add_test(pw_bytes.units_test
  SOURCES
    units_test.cc
  PRIVATE_DEPS
    pw_bytes
  GROUPS
    modules
    pw_bytes
)
