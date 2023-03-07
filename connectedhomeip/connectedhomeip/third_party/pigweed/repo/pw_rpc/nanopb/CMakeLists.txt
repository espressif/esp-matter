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

pw_add_library(pw_rpc.nanopb.server_api STATIC
  HEADERS
    public/pw_rpc/nanopb/internal/method.h
    public/pw_rpc/nanopb/internal/method_union.h
    public/pw_rpc/nanopb/server_reader_writer.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_bytes
    pw_rpc.config
    pw_rpc.nanopb.common
    pw_rpc.raw.server_api
    pw_rpc.server
    pw_span
  SOURCES
    method.cc
    server_reader_writer.cc
  PRIVATE_DEPS
    pw_log
    pw_rpc.log_config
)
if(Zephyr_FOUND AND CONFIG_PIGWEED_RPC_NANOPB_METHOD)
  zephyr_link_libraries(pw_rpc.nanopb.method)
endif()

# TODO(hepler): Deprecate this once no one depends on it.
pw_add_library(pw_rpc.nanopb.method_union INTERFACE
  PUBLIC_DEPS
    pw_rpc.nanopb.server_api
)
if(Zephyr_FOUND AND CONFIG_PIGWEED_RPC_NANOPB_METHOD_UNION)
  zephyr_link_libraries(pw_rpc.nanopb.method_union)
endif()

pw_add_library(pw_rpc.nanopb.client_api INTERFACE
  HEADERS
    public/pw_rpc/nanopb/client_reader_writer.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_function
    pw_rpc.client
    pw_rpc.nanopb.common
)
if(Zephyr_FOUND AND CONFIG_PIGWEED_RPC_NANOPB_CLIENT)
  zephyr_link_libraries(pw_rpc.nanopb.client_api)
endif()

pw_add_library(pw_rpc.nanopb.common STATIC
  HEADERS
    public/pw_rpc/nanopb/internal/common.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_bytes
    pw_rpc.common
    pw_third_party.nanopb
  SOURCES
    common.cc
  PRIVATE_DEPS
    pw_log
    pw_rpc.client
    pw_rpc.log_config
)
if(Zephyr_FOUND AND CONFIG_PIGWEED_RPC_NANOPB_COMMON)
  zephyr_link_libraries(pw_rpc.nanopb.common)
endif()

pw_add_library(pw_rpc.nanopb.test_method_context INTERFACE
  HEADERS
    public/pw_rpc/nanopb/fake_channel_output.h
    public/pw_rpc/nanopb/test_method_context.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_assert
    pw_containers
    pw_rpc.nanopb.server_api
    pw_rpc.test_utils
)

pw_add_library(pw_rpc.nanopb.client_testing INTERFACE
  HEADERS
    public/pw_rpc/nanopb/client_testing.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_rpc.nanopb.test_method_context
    pw_rpc.client
    pw_rpc.raw.client_testing
)

pw_add_library(pw_rpc.nanopb.client_server_testing INTERFACE
  HEADERS
    public/pw_rpc/nanopb/client_server_testing.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_rpc.nanopb.test_method_context
    pw_rpc.client_server_testing
)

pw_add_library(pw_rpc.nanopb.client_server_testing_threaded INTERFACE
  HEADERS
    public/pw_rpc/nanopb/client_server_testing_threaded.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_rpc.nanopb.test_method_context
    pw_rpc.client_server_testing_threaded
)

pw_add_library(pw_rpc.nanopb.internal_test_utils INTERFACE
  HEADERS
    pw_rpc_nanopb_private/internal_test_utils.h
  PUBLIC_DEPS
    pw_span
    pw_third_party.nanopb
)

pw_add_library(pw_rpc.nanopb.echo_service INTERFACE
  HEADERS
    public/pw_rpc/echo_service_nanopb.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_rpc.protos.nanopb_rpc
)
if(Zephyr_FOUND AND CONFIG_PIGWEED_RPC_NANOPB_ECHO_SERVICE)
  zephyr_link_libraries(pw_rpc.nanopb.echo_service)
endif()

pw_add_library(pw_rpc.nanopb.client_integration_test STATIC
  SOURCES
    client_integration_test.cc
  PRIVATE_DEPS
    pw_assert
    pw_rpc.integration_testing
    pw_rpc.protos.nanopb_rpc
    pw_sync.binary_semaphore
    pw_unit_test
)

pw_add_test(pw_rpc.nanopb.client_call_test
  SOURCES
    client_call_test.cc
  PRIVATE_DEPS
    pw_rpc.nanopb.client_api
    pw_rpc.nanopb.internal_test_utils
    pw_rpc.test_protos.nanopb
    pw_rpc.test_utils
  GROUPS
    modules
    pw_rpc.nanopb
)

pw_add_test(pw_rpc.nanopb.client_reader_writer_test
  SOURCES
    client_reader_writer_test.cc
  PRIVATE_DEPS
    pw_rpc.nanopb.client_api
    pw_rpc.nanopb.client_testing
    pw_rpc.test_protos.nanopb_rpc
  GROUPS
    modules
    pw_rpc.nanopb
)

pw_add_test(pw_rpc.nanopb.client_server_context_test
  SOURCES
    client_server_context_test.cc
  PRIVATE_DEPS
    pw_rpc.nanopb.client_api
    pw_rpc.nanopb.client_server_testing
    pw_rpc.test_protos.nanopb_rpc
  GROUPS
    modules
    pw_rpc.nanopb
)

if(("${pw_thread.thread_BACKEND}" STREQUAL "pw_thread_stl.thread") AND
   (NOT "${pw_sync.binary_semaphore_BACKEND}" STREQUAL "") AND
   (NOT "${pw_sync.mutex_BACKEND}" STREQUAL ""))
  pw_add_test(pw_rpc.nanopb.client_server_context_threaded_test
    SOURCES
      client_server_context_threaded_test.cc
    PRIVATE_DEPS
      pw_rpc.nanopb.client_api
      pw_rpc.nanopb.client_server_testing_threaded
      pw_rpc.test_protos.nanopb_rpc
      pw_sync.binary_semaphore
      pw_thread.test_threads
      pw_thread.thread
      pw_thread_stl.test_threads
    GROUPS
      modules
      pw_rpc.nanopb
  )
endif()

pw_add_test(pw_rpc.nanopb.codegen_test
  SOURCES
    codegen_test.cc
  PRIVATE_DEPS
    pw_rpc.nanopb.client_api
    pw_rpc.nanopb.internal_test_utils
    pw_rpc.nanopb.server_api
    pw_rpc.nanopb.test_method_context
    pw_rpc.test_protos.nanopb_rpc
    pw_rpc.test_utils
  GROUPS
    modules
    pw_rpc.nanopb
)

pw_add_test(pw_rpc.nanopb.fake_channel_output_test
  SOURCES
    fake_channel_output_test.cc
  PRIVATE_DEPS
    pw_rpc.nanopb.server_api
    pw_rpc.nanopb.test_method_context
    pw_rpc.test_protos.nanopb_rpc
  GROUPS
    modules
    pw_rpc.nanopb
)

pw_add_test(pw_rpc.nanopb.method_test
  SOURCES
    method_test.cc
  PRIVATE_DEPS
    pw_containers
    pw_rpc.nanopb.internal_test_utils
    pw_rpc.nanopb.server_api
    pw_rpc.test_protos.nanopb
    pw_rpc.test_utils
  GROUPS
    modules
    pw_rpc.nanopb
)

pw_add_test(pw_rpc.nanopb.method_info_test
  SOURCES
    method_info_test.cc
  PRIVATE_DEPS
		pw_rpc.common
    pw_rpc.test_protos.nanopb_rpc
    pw_rpc.test_utils
  GROUPS
    modules
    pw_rpc.nanopb
)

pw_add_test(pw_rpc.nanopb.method_lookup_test
  SOURCES
    method_lookup_test.cc
  PRIVATE_DEPS
    pw_rpc.nanopb.server_api
    pw_rpc.nanopb.test_method_context
    pw_rpc.test_protos.nanopb_rpc
    pw_rpc.test_utils
    pw_rpc.raw.test_method_context
  GROUPS
    modules
    pw_rpc.nanopb
)

pw_add_test(pw_rpc.nanopb.method_union_test
  SOURCES
    method_union_test.cc
  PRIVATE_DEPS
    pw_rpc.nanopb.internal_test_utils
    pw_rpc.nanopb.server_api
    pw_rpc.test_protos.nanopb
    pw_rpc.test_utils
  GROUPS
    modules
    pw_rpc.nanopb
)

pw_add_test(pw_rpc.nanopb.echo_service_test
  SOURCES
    echo_service_test.cc
  PRIVATE_DEPS
    pw_rpc.nanopb.echo_service
    pw_rpc.nanopb.server_api
    pw_rpc.nanopb.test_method_context
  GROUPS
    modules
    pw_rpc.nanopb
)

pw_add_test(pw_rpc.nanopb.serde_test
  SOURCES
    serde_test.cc
  PRIVATE_DEPS
    pw_rpc.nanopb.server_api
    pw_rpc.test_protos.nanopb
  GROUPS
    modules
    pw_rpc.nanopb
)

pw_add_test(pw_rpc.nanopb.server_callback_test
  SOURCES
    server_callback_test.cc
  PRIVATE_DEPS
    pw_rpc.nanopb.server_api
    pw_rpc.nanopb.test_method_context
    pw_rpc.test_protos.nanopb_rpc
  GROUPS
    modules
    pw_rpc.nanopb
)

pw_add_test(pw_rpc.nanopb.server_reader_writer_test
  SOURCES
    server_reader_writer_test.cc
  PRIVATE_DEPS
    pw_rpc.nanopb.server_api
    pw_rpc.nanopb.test_method_context
    pw_rpc.test_protos.nanopb_rpc
  GROUPS
    modules
    pw_rpc.nanopb
)

pw_add_test(pw_rpc.nanopb.stub_generation_test
  SOURCES
    stub_generation_test.cc
  PRIVATE_DEPS
    pw_rpc.test_protos.nanopb_rpc
  GROUPS
    modules
    pw_rpc.nanopb
)

# TODO(b/231950909) Test disabled as pw_work_queue lacks CMakeLists.txt
if((TARGET pw_work_queue.pw_work_queue) AND
   ("${pw_thread.thread_BACKEND}" STREQUAL "pw_thread_stl.thread") AND
   (NOT "${pw_sync.timed_thread_notification_BACKEND}" STREQUAL ""))
  pw_add_test(pw_rpc.nanopb.synchronous_call_test
    SOURCES
      synchronous_call_test.cc
    PRIVATE_DEPS
      pw_rpc.nanopb.test_method_context
      pw_rpc.synchronous_client_api
      pw_rpc.test_protos.nanopb_rpc
      pw_thread.thread
      pw_work_queue.pw_work_queue
      pw_work_queue.stl_test_thread
      pw_work_queue.test_thread
    GROUPS
      modules
      pw_rpc.nanopb
  )
endif()
