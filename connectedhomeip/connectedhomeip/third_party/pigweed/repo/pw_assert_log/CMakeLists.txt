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

# This backend to pw_assert's PW_CHECK()/PW_CRASH() macros via PW_LOG.
pw_add_library(pw_assert_log.check_backend STATIC
  HEADERS
    check_backend_public_overrides/pw_assert_backend/check_backend.h
    public/pw_assert_log/check_log.h
  PUBLIC_INCLUDES
    check_backend_public_overrides
    public
  PUBLIC_DEPS
    pw_log
    pw_preprocessor
  SOURCES
    assert_log.cc
  PRIVATE_DEPS
    pw_assert.config
    pw_assert.assert_compatibility_backend
)

# This backend to pw_assert's PW_ASSERT() macros via PW_LOG. It is intended only
# for use with PW_LOG backends which are constexpr compatible such as
# pw_log_android.
pw_add_library(pw_assert_log.assert_backend INTERFACE
  HEADERS
    assert_backend_public_overrides/pw_assert_backend/assert_backend.h
    public/pw_assert_log/assert_log.h
  PUBLIC_INCLUDES
    assert_backend_public_overrides
    public
  PUBLIC_DEPS
    pw_log
    pw_preprocessor
)
