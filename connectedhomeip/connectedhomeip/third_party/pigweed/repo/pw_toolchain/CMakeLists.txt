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

include("$ENV{PW_ROOT}/pw_build/pigweed.cmake")

add_subdirectory(arm_gcc EXCLUDE_FROM_ALL)

pw_add_library(pw_toolchain.no_destructor INTERFACE
  HEADERS
    public/pw_toolchain/no_destructor.h
  PUBLIC_INCLUDES
    public
)

pw_add_library(pw_toolchain.wrap_abort STATIC
  SOURCES
    wrap_abort.cc
  PUBLIC_DEPS
    pw_assert
  PUBLIC_LINK_OPTIONS
    "-Wl,--wrap=abort"
)

pw_add_test(pw_toolchain.no_destructor_test
  SOURCES
    no_destructor_test.cc
  PRIVATE_DEPS
    pw_toolchain.no_destructor
  GROUPS
    modules
    pw_toolchain
)
