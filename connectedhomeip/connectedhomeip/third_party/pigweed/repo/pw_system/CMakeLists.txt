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
include($ENV{PW_ROOT}/pw_system/backend.cmake)

# WARNING: Many of the dependencies in this file are missing and need to be
# added/updated. This is provided as a starting point, but currently does not
# work.

pw_add_library(pw_system.config INTERFACE
  HEADERS
    public/pw_system/config.h
  PUBLIC_INCLUDES
    public
)

pw_add_library(pw_system.log STATIC
  PUBLIC_DEPS
    # TODO(b/246101669): Add CMake support.
    # pw_log_rpc.log_service
    # pw_log_rpc.rpc_log_drain_thread
    pw_multisink
  PRIVATE_DEPS
    pw_system.config
    pw_system.rpc_server
    # TODO(b/246101669): Add CMake support.
    # pw_log_rpc.rpc_log_drain
    pw_sync.lock_annotations
    pw_sync.mutex
  HEADERS
    pw_system_private/log.h
  SOURCES
    log.cc
)

pw_add_library(pw_system.log_backend STATIC
  PRIVATE_DEPS
    pw_system.config
    pw_system.log
    pw_bytes
    pw_chrono.system_clock
    pw_log.facade
    pw_log.proto_utils
    pw_log_string.handler.facade
    pw_log_tokenized.metadata
    pw_multisink
    pw_result
    pw_sync.interrupt_spin_lock
    pw_sync.lock_annotations
    pw_tokenizer
    pw_tokenizer.global_handler_with_payload.facade
  SOURCES
    log_backend.cc
)

pw_add_facade(pw_system.rpc_server INTERFACE
  BACKEND
    pw_system.rpc_server_BACKEND
  HEADERS
    public/pw_system/rpc_server.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_system.config
    pw_thread.thread_core
)

pw_add_library(pw_system.hdlc_rpc_server STATIC
  PRIVATE_DEPS
    pw_assert
    pw_hdlc.pw_rpc
    pw_hdlc.rpc_channel_output
    pw_sync.mutex
    pw_system.config
    pw_system.io
    pw_system.rpc_server.facade
    pw_system.target_io
    pw_thread.thread_core
  SOURCES
    hdlc_rpc_server.cc
)

pw_add_library(pw_system.io INTERFACE
  HEADERS
    public/pw_system/io.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_stream
)

pw_add_library(pw_system.init STATIC
  HEADERS
    public/pw_system/init.h
  PUBLIC_INCLUDES
    public
  SOURCES
    init.cc
  PRIVATE_DEPS
        pw_system.log
        pw_system.rpc_server
        pw_rpc.pwpb.echo_service
        pw_thread.thread
)

pw_add_library(pw_system.work_queue STATIC
  HEADERS
    public/pw_system/work_queue.h
  PUBLIC_INCLUDES
    public
  SOURCES
    work_queue.cc
  PRIVATE_DEPS
    pw_work_queue
)

pw_add_library(pw_system.target_io STATIC
  PRIVATE_DEPS
    pw_system.io
    pw_stream
    pw_stream.sys_io_stream
  SOURCES
    target_io.cc
)

pw_add_library(pw_system.target_hooks INTERFACE
  HEADERS
    public/pw_system/target_hooks.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_thread.thread
)

pw_add_library(pw_system.stl_target_hooks STATIC
  PRIVATE_DEPS
    pw_thread.sleep
    pw_thread.thread
    pw_thread_stl.thread

  SOURCES
    stl_target_hooks.cc
)

pw_add_library(pw_system.freertos_target_hooks STATIC
  SOURCES
    freertos_target_hooks.cc
  PRIVATE_DEPS
    pw_thread.thread
    pw_thread_freertos.thread
    # TODO(b/234876414): This should depend on FreeRTOS but our third parties
    # currently do not have CMake support.
)

pw_add_library(pw_system.system_example STATIC
  PRIVATE_DEPS
    pw_system.init
    pw_system.io
    pw_system.target_hooks
    pw_stream
    pw_stream.sys_io_stream
  SOURCES
    example_user_app_init.cc
)
