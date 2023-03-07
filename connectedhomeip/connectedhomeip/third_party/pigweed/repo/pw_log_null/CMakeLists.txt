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

pw_add_library(pw_log_null INTERFACE
  HEADERS
    public/pw_log_null/log_null.h
    public_overrides/pw_log_backend/log_backend.h
  PUBLIC_INCLUDES
    public
    public_overrides
  PUBLIC_DEPS
    pw_preprocessor
)

pw_add_test(pw_log_null.test
  SOURCES
    test.cc
    test_c.c
  PRIVATE_DEPS
    pw_log_null
  GROUPS
    modules
    pw_log_null
)
