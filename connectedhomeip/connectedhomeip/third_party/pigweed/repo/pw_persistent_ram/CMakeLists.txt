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

pw_add_library(pw_persistent_ram STATIC
  HEADERS
    public/pw_persistent_ram/persistent.h
    public/pw_persistent_ram/persistent_buffer.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_assert
    pw_bytes
    pw_checksum
    pw_preprocessor
    pw_span
    pw_stream
  SOURCES
    persistent_buffer.cc
)

pw_add_test(pw_persistent_ram.persistent_test
  SOURCES
    persistent_test.cc
  PRIVATE_DEPS
    pw_persistent_ram
    pw_random
  GROUPS
    modules
    pw_persistent_ram
)

pw_add_test(pw_persistent_ram.persistent_buffer_test
  SOURCES
    persistent_buffer_test.cc
  PRIVATE_DEPS
    pw_persistent_ram
    pw_random
  GROUPS
    modules
    pw_persistent_ram
)
