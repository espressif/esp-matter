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
include($ENV{PW_ROOT}/pw_protobuf_compiler/proto.cmake)

add_subdirectory(nanopb)
add_subdirectory(pwpb)
add_subdirectory(raw)
add_subdirectory(system_server)

pw_add_module_config(pw_rpc_CONFIG)

pw_add_library(pw_rpc.config INTERFACE
  HEADERS
    public/pw_rpc/internal/config.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    ${pw_rpc_CONFIG}
)

pw_add_library(pw_rpc.log_config INTERFACE
  HEADERS
    public/pw_rpc/internal/log_config.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_rpc.config
)

pw_add_library(pw_rpc.server STATIC
  HEADERS
    public/pw_rpc/server.h
    public/pw_rpc/service.h
    public/pw_rpc/internal/hash.h
    public/pw_rpc/internal/method.h
    public/pw_rpc/internal/method_lookup.h
    public/pw_rpc/internal/method_union.h
    public/pw_rpc/internal/server_call.h
  PUBLIC_INCLUDES
    public
  SOURCES
    server.cc
    server_call.cc
    service.cc
  PUBLIC_DEPS
    pw_rpc.common
  PRIVATE_DEPS
    pw_log
    pw_rpc.log_config
)
if(Zephyr_FOUND AND CONFIG_PIGWEED_RPC_SERVER)
  zephyr_link_libraries(pw_rpc.server)
endif()

pw_add_library(pw_rpc.client STATIC
  HEADERS
    public/pw_rpc/client.h
    public/pw_rpc/internal/client_call.h
    public/pw_rpc/internal/service_client.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_result
    pw_rpc.common
    pw_span
  SOURCES
    client.cc
    client_call.cc
  PRIVATE_DEPS
    pw_log
    pw_rpc.log_config
)
if(Zephyr_FOUND AND CONFIG_PIGWEED_RPC_CLIENT)
  zephyr_link_libraries(pw_rpc.client)
endif()

pw_add_library(pw_rpc.client_server STATIC
  HEADERS
    public/pw_rpc/client_server.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_rpc.client
    pw_rpc.server
  SOURCES
    client_server.cc
)
if(Zephyr_FOUND AND CONFIG_PIGWEED_RPC_CLIENT_SERVER)
  zephyr_link_libraries(pw_rpc.client_server)
endif()

pw_add_library(pw_rpc.synchronous_client_api INTERFACE
  HEADERS
    public/pw_rpc/synchronous_call.h
    public/pw_rpc/synchronous_call_result.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_chrono.system_clock
    pw_rpc.client
    pw_rpc.common
    pw_sync.timed_thread_notification
)

pw_add_library(pw_rpc.common STATIC
  HEADERS
    public/pw_rpc/channel.h
    public/pw_rpc/internal/call.h
    public/pw_rpc/internal/call_context.h
    public/pw_rpc/internal/channel.h
    public/pw_rpc/internal/channel_list.h
    public/pw_rpc/internal/endpoint.h
    public/pw_rpc/internal/lock.h
    public/pw_rpc/internal/method_info.h
    public/pw_rpc/internal/packet.h
    public/pw_rpc/method_id.h
    public/pw_rpc/method_info.h
    public/pw_rpc/method_type.h
    public/pw_rpc/packet_meta.h
    public/pw_rpc/service_id.h
    public/pw_rpc/writer.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_assert
    pw_bytes
    pw_containers.intrusive_list
    pw_function
    pw_rpc.config
    pw_rpc.protos.pwpb
    pw_span
    pw_status
    pw_sync.lock_annotations
    pw_toolchain.no_destructor
  SOURCES
    call.cc
    channel.cc
    channel_list.cc
    endpoint.cc
    packet.cc
    packet_meta.cc
  PRIVATE_DEPS
    pw_log
    pw_rpc.log_config
)
if (NOT "${pw_sync.mutex_BACKEND}" STREQUAL "")
  pw_target_link_targets(pw_rpc.common PUBLIC pw_sync.mutex)
endif()
if(Zephyr_FOUND AND CONFIG_PIGWEED_RPC_COMMON)
  zephyr_link_libraries(pw_rpc.common)
endif()

pw_add_library(pw_rpc.fake_channel_output STATIC
  HEADERS
    public/pw_rpc/internal/fake_channel_output.h
    public/pw_rpc/payloads_view.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_assert
    pw_bytes
    pw_containers.filtered_view
    pw_containers.vector
    pw_containers.wrapped_iterator
    pw_function
    pw_rpc.common
    pw_sync.mutex
  SOURCES
    fake_channel_output.cc
  PRIVATE_DEPS
    pw_log
    pw_rpc.log_config
)

pw_add_library(pw_rpc.client_server_testing INTERFACE
  HEADERS
    public/pw_rpc/internal/client_server_testing.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_bytes
    pw_result
    pw_rpc.client_server
    pw_rpc.fake_channel_output
)

pw_add_library(pw_rpc.client_server_testing_threaded INTERFACE
  HEADERS
    public/pw_rpc/internal/client_server_testing_threaded.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_bytes
    pw_result
    pw_rpc.client_server_testing
    pw_sync.binary_semaphore
    pw_sync.lock_annotations
    pw_sync.mutex
    pw_thread.thread
)

pw_add_library(pw_rpc.test_helpers INTERFACE
  HEADERS
    public/pw_rpc/test_helpers.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_assert
    pw_chrono.system_clock
    pw_rpc.fake_channel_output
    pw_status
    pw_sync.counting_semaphore
    pw_thread.yield
)

# thread_testing target is kept for backward compatibility.
# New code should use pw_rpc.test_helpers instead.
pw_add_library(pw_rpc.thread_testing INTERFACE
  HEADERS
    public/pw_rpc/thread_testing.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_rpc.test_helpers
)

pw_add_library(pw_rpc.test_utils INTERFACE
  HEADERS
    public/pw_rpc/internal/fake_channel_output.h
    public/pw_rpc/internal/method_impl_tester.h
    public/pw_rpc/internal/method_info_tester.h
    public/pw_rpc/internal/test_method.h
    public/pw_rpc/internal/test_method_context.h
    public/pw_rpc/internal/test_utils.h
    pw_rpc_private/fake_server_reader_writer.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_assert
    pw_bytes
    pw_rpc.client
    pw_rpc.server
    pw_containers.vector
    pw_rpc.raw.fake_channel_output
    pw_rpc.raw.server_api
)

pw_add_library(pw_rpc.integration_testing STATIC
  HEADERS
    public/pw_rpc/integration_test_socket_client.h
    public/pw_rpc/integration_testing.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_assert
    pw_function
    pw_hdlc.pw_rpc
    pw_hdlc.rpc_channel_output
    pw_rpc.client
    pw_stream.socket_stream
    pw_unit_test
    pw_unit_test.logging_event_handler
  SOURCES
    integration_testing.cc
  PRIVATE_DEPS
    pw_log
)

pw_proto_library(pw_rpc.protos
  SOURCES
    benchmark.proto
    internal/packet.proto
    echo.proto
  INPUTS
    echo.options
    benchmark.options
  PREFIX
    pw_rpc
)

pw_proto_library(pw_rpc.test_protos
  SOURCES
    pw_rpc_test_protos/no_package.proto
    pw_rpc_test_protos/test.proto
  INPUTS
    pw_rpc_test_protos/test.options
)

# Set pw_rpc_CONFIG to this to disable the global mutex.
pw_add_library(pw_rpc.disable_global_mutex_config INTERFACE
  PUBLIC_DEFINES
    PW_RPC_USE_GLOBAL_MUTEX=0
)

pw_add_test(pw_rpc.call_test
  SOURCES
    call_test.cc
  PRIVATE_DEPS
    pw_rpc.server
    pw_rpc.test_utils
  GROUPS
    modules
    pw_rpc
)

pw_add_test(pw_rpc.channel_test
  SOURCES
    channel_test.cc
  PRIVATE_DEPS
    pw_rpc.server
    pw_rpc.test_utils
  GROUPS
    modules
    pw_rpc
)

pw_add_test(pw_rpc.packet_test
  SOURCES
    packet_test.cc
  PRIVATE_DEPS
    pw_bytes
    pw_protobuf
    pw_rpc.server
  GROUPS
    modules
    pw_rpc
)

pw_add_test(pw_rpc.packet_meta_test
  SOURCES
    packet_meta_test.cc
  PRIVATE_DEPS
    pw_bytes
    pw_rpc.server
  GROUPS
    modules
    pw_rpc
)

pw_add_test(pw_rpc.service_test
  SOURCES
    service_test.cc
  PRIVATE_DEPS
    pw_assert
    pw_rpc.protos.pwpb
    pw_rpc.server
  GROUPS
    modules
    pw_rpc
)

pw_add_test(pw_rpc.client_server_test
  SOURCES
    client_server_test.cc
  PRIVATE_DEPS
    pw_rpc.client_server
    pw_rpc.test_utils
    pw_rpc.raw.server_api
  GROUPS
    modules
    pw_rpc
)

pw_add_test(pw_rpc.method_test
  SOURCES
    method_test.cc
  PRIVATE_DEPS
    pw_rpc.server
    pw_rpc.test_utils
  GROUPS
    modules
    pw_rpc
)

pw_add_test(pw_rpc.server_test
  SOURCES
    server_test.cc
  PRIVATE_DEPS
    pw_assert
    pw_rpc.protos.pwpb
    pw_rpc.server
    pw_rpc.test_utils
  GROUPS
    modules
    pw_rpc
)

pw_add_test(pw_rpc.fake_channel_output_test
  SOURCES
    fake_channel_output_test.cc
  PRIVATE_DEPS
    pw_rpc.test_utils
  GROUPS
    modules
    pw_rpc
)

pw_add_test(pw_rpc.test_helpers_test
  SOURCES
    test_helpers_test.cc
  PRIVATE_DEPS
    pw_result
    pw_rpc.pwpb.client_testing
    pw_rpc.pwpb.echo_service
    pw_rpc.pwpb.server_api
    pw_rpc.test_helpers
    pw_status
    pw_sync.interrupt_spin_lock
    pw_sync.lock_annotations
    pw_sync.timed_thread_notification
  GROUPS
    modules
    pw_rpc
)
