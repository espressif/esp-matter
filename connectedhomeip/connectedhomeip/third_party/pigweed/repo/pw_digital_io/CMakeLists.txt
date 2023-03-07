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

pw_add_library(pw_digital_io STATIC
  HEADERS
    public/pw_digital_io/digital_io.h
    public/pw_digital_io/internal/conversions.h
  PUBLIC_INCLUDES
    public
  SOURCES
    digital_io.cc
  PUBLIC_DEPS
    pw_assert
    pw_function
    pw_result
    pw_status
)
if(Zephyr_FOUND AND CONFIG_PIGWEED_DIGITAL_IO)
  zephyr_link_libraries(pw_digital_io)
endif()

pw_add_test(pw_digital_io.stream_test
  SOURCES
    digital_io_test.cc
  PRIVATE_DEPS
    pw_digital_io
  GROUPS
    modules
    pw_digital_io
)
