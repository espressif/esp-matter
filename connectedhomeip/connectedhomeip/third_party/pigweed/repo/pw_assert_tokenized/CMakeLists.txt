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

pw_add_library(pw_assert_tokenized.handler STATIC
  HEADERS
    public/pw_assert_tokenized/handler.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_preprocessor
  SOURCES
    log_handler.cc
  PRIVATE_DEPS
    pw_assert.config
    pw_base64
    pw_bytes
    pw_log
    pw_log_tokenized
)

pw_add_library(pw_assert_tokenized.assert_backend INTERFACE
  HEADERS
    assert_public_overrides/pw_assert_backend/assert_backend.h
    public/pw_assert_tokenized/assert_tokenized.h
  PUBLIC_INCLUDES
    assert_public_overrides
    public
  PUBLIC_DEPS
    pw_assert_tokenized.handler
    pw_tokenizer
)

pw_add_library(pw_assert_tokenized.check_backend INTERFACE
  HEADERS
    check_public_overrides/pw_assert_backend/check_backend.h
    public/pw_assert_tokenized/check_tokenized.h
  PUBLIC_INCLUDES
    check_public_overrides
    public
  PUBLIC_DEPS
    pw_assert_tokenized.handler
    pw_log_tokenized
    pw_tokenizer
)
