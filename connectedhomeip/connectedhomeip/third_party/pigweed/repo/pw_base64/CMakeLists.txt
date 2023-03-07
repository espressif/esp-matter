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

pw_add_library(pw_base64 STATIC
  HEADERS
    public/pw_base64/base64.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_span
    pw_string.string
  SOURCES
    base64.cc
)

pw_add_test(pw_base64.base64_test
  SOURCES
    base64_test.cc
    base64_test_c.c
  PRIVATE_DEPS
    pw_base64
  GROUPS
    modules
    pw_base64
)
