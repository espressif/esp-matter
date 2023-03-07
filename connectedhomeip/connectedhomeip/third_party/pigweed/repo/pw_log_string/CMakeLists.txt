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
include($ENV{PW_ROOT}/pw_log_string/backend.cmake)

pw_add_library(pw_log_string INTERFACE
  HEADERS
    public/pw_log_string/log_string.h
    public_overrides/pw_log_backend/log_backend.h
  PUBLIC_INCLUDES
    public
    public_overrides
  PUBLIC_DEPS
    pw_preprocessor
    pw_log_string.handler
)

pw_add_facade(pw_log_string.handler STATIC
  BACKEND
    pw_log_string.handler_BACKEND
  HEADERS
    public/pw_log_string/handler.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_preprocessor
  SOURCES
    handler.cc
)
