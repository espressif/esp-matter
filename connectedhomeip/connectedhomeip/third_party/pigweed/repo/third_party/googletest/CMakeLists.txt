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

set(dir_pw_third_party_googletest "" CACHE PATH
    "Path to the googletest installation. When set, pw_third_party.googletest \
     is provided")

# If googletest is not configured, a script that displays an error message is
# used instead. If the build rule is used in the build it fails with this
# error.
if(NOT dir_pw_third_party_googletest)
  pw_add_error_target(pw_third_party.googletest
    MESSAGE
      "Attempted to build the pw_third_party.googletest without "
      "configuring it via dir_pw_third_party_googletest. "
      "See https://pigweed.dev/third_party/googletest."
  )

  pw_add_library(pw_third_party.googletest.gtest_main INTERFACE
    PUBLIC_DEPS
      pw_third_party.googletest
  )

  pw_add_library(pw_third_party.googletest.gmock_main INTERFACE
    PUBLIC_DEPS
      pw_third_party.googletest
  )

else()  # dir_pw_thid_parrty_googletest is set
  pw_add_library(pw_third_party.googletest STATIC
    SOURCES
      # Only add the "*-all.cc" files (and no headers) to improve maintainability
      # from upstream refactoring. The "*-all.cc" files include the respective
      # source files.
      ${dir_pw_third_party_googletest}/googlemock/src/gmock-all.cc
      ${dir_pw_third_party_googletest}/googletest/src/gtest-all.cc
    HEADERS
      ${dir_pw_third_party_googletest}/googlemock/include/gmock/gmock.h
      ${dir_pw_third_party_googletest}/googletest/include/gtest/gtest-spi.h
      ${dir_pw_third_party_googletest}/googletest/include/gtest/gtest.h
    PUBLIC_INCLUDES
      ${dir_pw_third_party_googletest}/googletest
      ${dir_pw_third_party_googletest}/googletest/include
      ${dir_pw_third_party_googletest}/googlemock
      ${dir_pw_third_party_googletest}/googlemock/include
    PUBLIC_COMPILE_OPTIONS
      -Wno-undef
  )

  pw_add_library(pw_third_party.googletest.gtest_main STATIC
    SOURCES
      ${dir_pw_third_party_googletest}/googletest/src/gtest_main.cc
    PUBLIC_DEPS
      pw_third_party.googletest
  )

  pw_add_library(pw_third_party.googletest.gmock_main STATIC
    SOURCES
      ${dir_pw_third_party_googletest}/googlemock/src/gmock_main.cc
    PUBLIC_DEPS
      pw_third_party.googletest
  )
endif()
