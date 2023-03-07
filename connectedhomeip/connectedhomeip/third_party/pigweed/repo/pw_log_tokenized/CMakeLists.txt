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
include($ENV{PW_ROOT}/pw_tokenizer/backend.cmake)

pw_add_module_config(pw_log_tokenized_CONFIG)

pw_add_library(pw_log_tokenized.config INTERFACE
  HEADERS
    public/pw_log_tokenized/config.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_log.facade
    ${pw_log_tokenized_CONFIG}
)

pw_add_library(pw_log_tokenized INTERFACE
  HEADERS
    public/pw_log_tokenized/log_tokenized.h
    public_overrides/pw_log_backend/log_backend.h
  PUBLIC_INCLUDES
    public
    public_overrides
  PUBLIC_DEPS
    pw_log_tokenized.config
    pw_log_tokenized.metadata
    pw_tokenizer
    pw_tokenizer.global_handler_with_payload
)

pw_add_library(pw_log_tokenized.metadata INTERFACE
  HEADERS
    public/pw_log_tokenized/metadata.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_log_tokenized.config
)

# This target provides a backend for pw_tokenizer that encodes tokenized logs as
# Base64, encodes them into HDLC frames, and writes them over sys_io.
pw_add_library(pw_log_tokenized.base64_over_hdlc STATIC
  HEADERS
    public/pw_log_tokenized/base64_over_hdlc.h
  PUBLIC_INCLUDES
    public
  SOURCES
    base64_over_hdlc.cc
  PRIVATE_DEPS
    pw_hdlc.encoder
    pw_span
    pw_stream.sys_io_stream
    pw_tokenizer.base64
    pw_tokenizer.global_handler_with_payload.facade
)

if(NOT "${pw_tokenizer.global_handler_with_payload_BACKEND}" STREQUAL "")
  pw_add_test(pw_log_tokenized.log_tokenized_test
    SOURCES
      log_tokenized_test.cc
      log_tokenized_test_c.c
      pw_log_tokenized_private/test_utils.h
    PRIVATE_DEPS
      pw_log_tokenized
      pw_preprocessor
    GROUPS
      modules
      pw_log_tokenized
  )
endif()

pw_add_test(pw_log_tokenized.metadata_test
  SOURCES
    metadata_test.cc
  PRIVATE_DEPS
    pw_log_tokenized.metadata
  GROUPS
    modules
    pw_log_tokenized
)
