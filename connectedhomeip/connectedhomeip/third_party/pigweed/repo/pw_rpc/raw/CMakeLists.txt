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

pw_add_library(pw_rpc.raw.server_api STATIC
  HEADERS
    public/pw_rpc/raw/internal/method.h
    public/pw_rpc/raw/internal/method_union.h
    public/pw_rpc/raw/server_reader_writer.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_rpc.server
    pw_bytes
  SOURCES
    method.cc
)

pw_add_library(pw_rpc.raw.client_api INTERFACE
  HEADERS
    public/pw_rpc/raw/client_reader_writer.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_rpc.client
    pw_bytes
)

pw_add_library(pw_rpc.raw.fake_channel_output INTERFACE
  HEADERS
    public/pw_rpc/raw/fake_channel_output.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_rpc.fake_channel_output
)

pw_add_library(pw_rpc.raw.test_method_context INTERFACE
  HEADERS
    public/pw_rpc/raw/test_method_context.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_assert
    pw_containers
    pw_rpc.raw.fake_channel_output
    pw_rpc.raw.server_api
    pw_rpc.test_utils
)

pw_add_library(pw_rpc.raw.client_testing STATIC
  HEADERS
    public/pw_rpc/raw/client_testing.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_rpc.fake_channel_output
  SOURCES
    client_testing.cc
  PRIVATE_DEPS
    pw_log
    pw_rpc.log_config
)

pw_add_test(pw_rpc.raw.client_test
  SOURCES
    client_test.cc
  PRIVATE_DEPS
    pw_rpc.raw.client_api
    pw_rpc.raw.client_testing
    pw_rpc.test_utils
  GROUPS
    modules
    pw_rpc.raw
)

pw_add_test(pw_rpc.raw.client_reader_writer_test
  SOURCES
    client_reader_writer_test.cc
  PRIVATE_DEPS
    pw_rpc.raw.client_api
    pw_rpc.raw.client_testing
    pw_rpc.test_protos.raw_rpc
  GROUPS
    modules
    pw_rpc.raw
)

pw_add_test(pw_rpc.raw.method_test
  SOURCES
    method_test.cc
  PRIVATE_DEPS
    pw_containers
    pw_protobuf
    pw_rpc.raw.server_api
    pw_rpc.test_protos.pwpb
    pw_rpc.test_protos.raw_rpc
    pw_rpc.test_utils
  GROUPS
    modules
    pw_rpc.raw
)

pw_add_test(pw_rpc.raw.method_info_test
  SOURCES
    method_info_test.cc
  PRIVATE_DEPS
    pw_rpc.common
    pw_rpc.test_protos.raw_rpc
    pw_rpc.test_utils
  GROUPS
    modules
    pw_rpc.raw
)

pw_add_test(pw_rpc.raw.method_union_test
  SOURCES
    method_union_test.cc
  PRIVATE_DEPS
    pw_protobuf
    pw_rpc.raw.server_api
    pw_rpc.test_protos.pwpb
    pw_rpc.test_utils
  GROUPS
    modules
    pw_rpc.raw
)

pw_add_test(pw_rpc.raw.server_reader_writer_test
  SOURCES
    server_reader_writer_test.cc
  PRIVATE_DEPS
    pw_rpc.raw.server_api
    pw_rpc.raw.test_method_context
    pw_rpc.test_protos.raw_rpc
  GROUPS
    modules
    pw_rpc.raw
)
