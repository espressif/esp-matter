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
include($ENV{PW_ROOT}/pw_assert_basic/backend.cmake)

pw_add_facade(pw_assert_basic.handler INTERFACE
  BACKEND
    pw_assert_basic.handler_BACKEND
  HEADERS
    public/pw_assert_basic/handler.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_preprocessor
)

# TODO(b/235149326): This backend implements pw_assert's check backend and the
# temporary compatibility C ABI (pw_assert_HandleFailure()).
pw_add_library(pw_assert_basic.check_backend STATIC
  HEADERS
    public_overrides/pw_assert_backend/check_backend.h
    public/pw_assert_basic/assert_basic.h
  PUBLIC_INCLUDES
    public
    public_overrides
  PUBLIC_DEPS
    pw_assert_basic.handler
  SOURCES
    assert_basic.cc
  PRIVATE_DEPS
    pw_assert_basic.handler
    pw_assert.config
    pw_assert.assert_compatibility_backend
)

pw_add_library(pw_assert_basic.basic_handler STATIC
  SOURCES
    basic_handler.cc
  PRIVATE_DEPS
    pw_assert_basic.handler.facade
    pw_assert.config
    pw_preprocessor
    pw_string.builder
    pw_sys_io
)
