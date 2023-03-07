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

pw_add_module_config(pw_unit_test_CONFIG)

pw_add_library(pw_unit_test.config INTERFACE
  HEADERS
    public/pw_unit_test/config.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    ${pw_unit_test_CONFIG}
    pw_polyfill
)

add_library(pw_unit_test INTERFACE)
target_link_libraries(pw_unit_test
  INTERFACE
    "${pw_unit_test_GOOGLETEST_BACKEND}"
)

pw_add_library(pw_unit_test.light STATIC
  SOURCES
    framework.cc
  PUBLIC_DEPS
    pw_polyfill
    pw_preprocessor
    pw_string
    pw_unit_test.config
  PUBLIC_INCLUDES
    public_overrides  # pw_unit_test overrides the gtest/gtest.h header.
)

pw_add_library(pw_unit_test.static_library_support STATIC
  HEADERS
    public/pw_unit_test/static_library_support.h
  PUBLIC_INCLUDES
    public
  SOURCES
    static_library_support.cc
  PUBLIC_DEPS
    pw_unit_test.light
)

pw_add_library(pw_unit_test.event_handler INTERFACE
  HEADERS
    public/pw_unit_test/event_handler.h
  PUBLIC_INCLUDES
    public
)

pw_add_library(pw_unit_test.googletest_style_event_handler STATIC
  HEADERS
    public/pw_unit_test/googletest_style_event_handler.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_unit_test.event_handler
    pw_preprocessor
  SOURCES
    googletest_style_event_handler.cc
)

pw_add_library(pw_unit_test.simple_printing_main STATIC
  SOURCES
    simple_printing_main.cc
    simple_printing_event_handler.cc
  PRIVATE_DEPS
    pw_unit_test
    pw_unit_test.googletest_style_event_handler
    pw_preprocessor
    pw_string
    pw_sys_io
)

pw_add_library(pw_unit_test.logging_event_handler STATIC
  HEADERS
    public/pw_unit_test/logging_event_handler.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_log
    pw_unit_test.googletest_style_event_handler
  SOURCES
    logging_event_handler.cc
)

pw_add_library(pw_unit_test.logging_main STATIC
  SOURCES
    logging_main.cc
  PRIVATE_DEPS
    pw_unit_test.logging_event_handler
    pw_unit_test
)
