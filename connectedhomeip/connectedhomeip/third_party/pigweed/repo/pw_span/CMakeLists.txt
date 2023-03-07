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

# Provides pw::span in "pw_span/span.h".
pw_add_library(pw_span INTERFACE
  HEADERS
    public/pw_span/span.h
    public/pw_span/internal/config.h
    public/pw_span/internal/span_impl.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_assert
    pw_polyfill
    pw_polyfill.standard_library
)

pw_add_test(pw_span.pw_span_test
  SOURCES
    span_test.cc
  PRIVATE_DEPS
    pw_span
  GROUPS
    modules
    pw_span
)

if(Zephyr_FOUND AND CONFIG_PIGWEED_SPAN)
  zephyr_link_libraries(pw_span)
endif()
