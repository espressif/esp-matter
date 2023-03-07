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
include($ENV{PW_ROOT}/pw_log/backend.cmake)
include($ENV{PW_ROOT}/pw_protobuf_compiler/proto.cmake)

pw_add_module_config(pw_log_CONFIG)

pw_add_module_config(pw_log_GLOG_ADAPTER_CONFIG)

pw_add_library(pw_log.config INTERFACE
  HEADERS
    public/pw_log/config.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    ${pw_log_CONFIG}
)

pw_add_facade(pw_log INTERFACE
  BACKEND
    pw_log_BACKEND
  HEADERS
    public/pw_log/levels.h
    public/pw_log/log.h
    public/pw_log/options.h
    public/pw_log/short.h
    public/pw_log/shorter.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_log.config
)

pw_add_library(pw_log.glog_adapter INTERFACE
  HEADERS
    public/pw_log/glog_adapter.h
    public/pw_log/glog_adapter_config.h
    public/pw_log/internal/glog_adapter.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_assert
    pw_log
    pw_log.config
    pw_preprocessor
    pw_string
    ${pw_log_GLOG_ADAPTER_CONFIG}
)

pw_add_library(pw_log.proto_utils STATIC
  HEADERS
    public/pw_log/proto_utils.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_log.facade
    pw_bytes
    pw_log.protos.pwpb
    pw_log_tokenized.metadata
    pw_result
  PRIVATE_DEPS
    pw_protobuf
  SOURCES
    proto_utils.cc
)

pw_proto_library(pw_log.protos
  SOURCES
    log.proto
  PREFIX
    pw_log/proto
  DEPS
    pw_protobuf.common_proto
    pw_tokenizer.proto
)

if(NOT "${pw_log_BACKEND}" STREQUAL "")
  pw_add_test(pw_log.basic_log_test
    SOURCES
      basic_log_test.cc
      basic_log_test_plain_c.c
    PRIVATE_DEPS
      pw_log
      pw_preprocessor
    GROUPS
      modules
      pw_log
  )

  pw_add_test(pw_log.glog_adapter_test
    SOURCES
      glog_adapter_test.cc
    PRIVATE_DEPS
      pw_log.glog_adapter
    GROUPS
      modules
      pw_log
  )

  pw_add_test(pw_log.proto_utils_test
    SOURCES
      proto_utils_test.cc
    PRIVATE_DEPS
      pw_log
      pw_log.proto_utils
      pw_log.protos.pwpb
      pw_preprocessor
      pw_protobuf
      pw_protobuf.bytes_utils
      pw_containers
    GROUPS
      modules
      pw_log
  )
endif()
