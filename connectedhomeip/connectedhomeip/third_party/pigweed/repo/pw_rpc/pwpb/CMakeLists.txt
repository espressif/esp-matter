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

pw_add_library(pw_rpc.pwpb.server_api STATIC
  HEADERS
    public/pw_rpc/pwpb/internal/method.h
    public/pw_rpc/pwpb/internal/method_union.h
    public/pw_rpc/pwpb/server_reader_writer.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_bytes
    pw_rpc.config
    pw_rpc.pwpb.common
    pw_rpc.raw.server_api
    pw_rpc.server
  SOURCES
    server_reader_writer.cc
  PRIVATE_DEPS
    pw_log
    pw_rpc.log_config
)

pw_add_library(pw_rpc.pwpb.client_api INTERFACE
  HEADERS
    public/pw_rpc/pwpb/client_reader_writer.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_function
    pw_rpc.pwpb.common
    pw_rpc.common
)

pw_add_library(pw_rpc.pwpb.common INTERFACE
  HEADERS
    public/pw_rpc/pwpb/internal/common.h
    public/pw_rpc/pwpb/serde.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_assert.assert
    pw_bytes
    pw_log
    pw_protobuf
    pw_rpc.client
    pw_rpc.common
    pw_rpc.log_config
    pw_rpc.server
    pw_span
    pw_status
)

pw_add_library(pw_rpc.pwpb.test_method_context INTERFACE
  HEADERS
    public/pw_rpc/pwpb/fake_channel_output.h
    public/pw_rpc/pwpb/test_method_context.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_assert
    pw_containers
    pw_rpc.pwpb.server_api
    pw_rpc.test_utils
    pw_span
)

pw_add_library(pw_rpc.pwpb.client_testing INTERFACE
  HEADERS
    public/pw_rpc/pwpb/client_testing.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_rpc.client
    pw_rpc.pwpb.test_method_context
    pw_rpc.raw.client_testing
)

pw_add_library(pw_rpc.pwpb.client_server_testing INTERFACE
  HEADERS
    public/pw_rpc/pwpb/client_server_testing.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_rpc.pwpb.test_method_context
    pw_rpc.client_server_testing
)

pw_add_library(pw_rpc.pwpb.client_server_testing_threaded INTERFACE
  HEADERS
    public/pw_rpc/pwpb/client_server_testing_threaded.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_rpc.pwpb.test_method_context
    pw_rpc.client_server_testing_threaded
)

pw_add_library(pw_rpc.pwpb.internal_test_utils INTERFACE
  HEADERS
    pw_rpc_pwpb_private/internal_test_utils.h
  PUBLIC_DEPS
    pw_status
    pw_stream
)

pw_add_library(pw_rpc.pwpb.echo_service INTERFACE
  HEADERS
    public/pw_rpc/echo_service_pwpb.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_rpc.protos.pwpb_rpc
)

pw_add_library(pw_rpc.pwpb.client_integration_test STATIC
  SOURCES
    client_integration_test.cc
  PRIVATE_DEPS
    pw_assert
    pw_rpc.integration_testing
    pw_rpc.protos.pwpb_rpc
    pw_sync.binary_semaphore
    pw_unit_test
)

pw_add_test(pw_rpc.pwpb.client_call_test
  SOURCES
    client_call_test.cc
  PRIVATE_DEPS
    pw_rpc.pwpb.client_api
    pw_rpc.pwpb.internal_test_utils
    pw_rpc.test_protos.pwpb
    pw_rpc.test_utils
  GROUPS
    modules
    pw_rpc.pwpb
)

pw_add_test(pw_rpc.pwpb.client_reader_writer_test
  SOURCES
    client_reader_writer_test.cc
  PRIVATE_DEPS
    pw_rpc.pwpb.client_api
    pw_rpc.pwpb.client_testing
    pw_rpc.test_protos.pwpb_rpc
  GROUPS
    modules
    pw_rpc.pwpb
)

pw_add_test(pw_rpc.pwpb.client_server_context_test
  SOURCES
    client_server_context_test.cc
  PRIVATE_DEPS
    pw_rpc.pwpb.client_api
    pw_rpc.pwpb.client_server_testing
    pw_rpc.test_protos.pwpb_rpc
  GROUPS
    modules
    pw_rpc.pwpb
)

if(("${pw_thread.thread_BACKEND}" STREQUAL "pw_thread_stl.thread") AND
   (NOT "${pw_sync.binary_semaphore_BACKEND}" STREQUAL "") AND
   (NOT "${pw_sync.mutex_BACKEND}" STREQUAL ""))
  pw_add_test(pw_rpc.pwpb.client_server_context_threaded_test
    SOURCES
      client_server_context_threaded_test.cc
    PRIVATE_DEPS
      pw_rpc.pwpb.client_api
      pw_rpc.pwpb.client_server_testing_threaded
      pw_rpc.test_protos.pwpb_rpc
      pw_sync.binary_semaphore
      pw_thread.test_threads
      pw_thread.thread
      pw_thread_stl.test_threads
    GROUPS
      modules
      pw_rpc.pwpb
  )
endif()

pw_add_test(pw_rpc.pwpb.codegen_test
  SOURCES
    codegen_test.cc
  PRIVATE_DEPS
    pw_rpc.pwpb.client_api
    pw_rpc.pwpb.internal_test_utils
    pw_rpc.pwpb.server_api
    pw_rpc.pwpb.test_method_context
    pw_rpc.test_protos.pwpb_rpc
    pw_rpc.test_utils
  GROUPS
    modules
    pw_rpc.pwpb
)

pw_add_test(pw_rpc.pwpb.echo_service_test
  SOURCES
    echo_service_test.cc
  PRIVATE_DEPS
    pw_rpc.pwpb.echo_service
    pw_rpc.pwpb.server_api
    pw_rpc.pwpb.test_method_context
  GROUPS
    modules
    pw_rpc.pwpb
)

pw_add_test(pw_rpc.pwpb.fake_channel_output_test
  SOURCES
    fake_channel_output_test.cc
  PRIVATE_DEPS
    pw_rpc.pwpb.server_api
    pw_rpc.pwpb.test_method_context
    pw_rpc.test_protos.pwpb_rpc
  GROUPS
    modules
    pw_rpc.pwpb
)

pw_add_test(pw_rpc.pwpb.method_test
  SOURCES
    method_test.cc
  PRIVATE_DEPS
    pw_containers
    pw_rpc.pwpb.internal_test_utils
    pw_rpc.pwpb.server_api
    pw_rpc.test_protos.pwpb
    pw_rpc.test_utils
  GROUPS
    modules
    pw_rpc.pwpb
)

pw_add_test(pw_rpc.pwpb.method_info_test
  SOURCES
    method_info_test.cc
  PRIVATE_DEPS
    pw_rpc.common
    pw_rpc.test_protos.pwpb_rpc
    pw_rpc.test_utils
  GROUPS
    modules
    pw_rpc.pwpb
)

pw_add_test(pw_rpc.pwpb.method_lookup_test
  SOURCES
    method_lookup_test.cc
  PRIVATE_DEPS
    pw_rpc.pwpb.server_api
    pw_rpc.pwpb.test_method_context
    pw_rpc.raw.test_method_context
    pw_rpc.test_protos.pwpb_rpc
    pw_rpc.test_utils
  GROUPS
    modules
    pw_rpc.pwpb
)

pw_add_test(pw_rpc.pwpb.method_union_test
  SOURCES
    method_union_test.cc
  PRIVATE_DEPS
    pw_rpc.pwpb.internal_test_utils
    pw_rpc.pwpb.server_api
    pw_rpc.test_protos.pwpb
    pw_rpc.test_utils
  GROUPS
    modules
    pw_rpc.pwpb
)

pw_add_test(pw_rpc.pwpb.serde_test
  SOURCES
    serde_test.cc
  PRIVATE_DEPS
    pw_rpc.pwpb.server_api
    pw_rpc.test_protos.pwpb
  GROUPS
    modules
    pw_rpc.pwpb
)

pw_add_test(pw_rpc.pwpb.server_callback_test
  SOURCES
    server_callback_test.cc
  PRIVATE_DEPS
    pw_rpc.pwpb.server_api
    pw_rpc.pwpb.test_method_context
    pw_rpc.test_protos.pwpb_rpc
  GROUPS
    modules
    pw_rpc.pwpb
)

pw_add_test(pw_rpc.pwpb.server_reader_writer_test
  SOURCES
    server_reader_writer_test.cc
  PRIVATE_DEPS
    pw_rpc.pwpb.server_api
    pw_rpc.pwpb.test_method_context
    pw_rpc.test_protos.pwpb_rpc
  GROUPS
    modules
    pw_rpc.pwpb
)

pw_add_test(pw_rpc.pwpb.stub_generation_test
  SOURCES
    stub_generation_test.cc
  PRIVATE_DEPS
    pw_rpc.test_protos.pwpb_rpc
  GROUPS
    modules
    pw_rpc.pwpb
)

# TODO(b/231950909) Test disabled as pw_work_queue lacks CMakeLists.txt
if((TARGET pw_work_queue.pw_work_queue) AND
   ("${pw_thread.thread_BACKEND}" STREQUAL "pw_thread_stl.thread") AND
   (NOT "${pw_sync.timed_thread_notification_BACKEND}" STREQUAL
   "pw_sync.timed_thread_notification.NO_BACKEND_SET"))
  pw_add_test(pw_rpc.pwpb.synchronous_call_test
    SOURCES
      synchronous_call_test.cc
    PRIVATE_DEPS
      pw_rpc.pwpb.test_method_context
      pw_rpc.synchronous_client_api
      pw_rpc.test_protos.pwpb_rpc
      pw_thread.thread
      pw_work_queue.pw_work_queue
      pw_work_queue.stl_test_thread
      pw_work_queue.test_thread
    GROUPS
      modules
      pw_rpc.pwpb
  )
endif()
