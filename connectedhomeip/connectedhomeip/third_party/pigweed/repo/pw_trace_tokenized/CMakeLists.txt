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

pw_add_module_config(pw_trace_tokenized_CONFIG)

pw_add_library(pw_trace_tokenized.config INTERFACE
  HEADERS
    public/pw_trace_tokenized/config.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    ${pw_trace_CONFIG}
)

pw_add_library(pw_trace_tokenized STATIC
  HEADERS
    public/pw_trace_tokenized/internal/trace_tokenized_internal.h
    public/pw_trace_tokenized/trace_callback.h
    public/pw_trace_tokenized/trace_tokenized.h
    public_overrides/pw_trace_backend/trace_backend.h
  PUBLIC_INCLUDES
    public
    public_overrides
  PUBLIC_DEPS
    pw_span
    pw_status
    pw_tokenizer
    pw_trace_tokenized.config
  SOURCES
    trace.cc
  PRIVATE_DEPS
    pw_assert
    pw_log
    pw_ring_buffer
    pw_trace.facade
    pw_varint
)

pw_add_library(pw_trace_tokenized.trace_buffer STATIC
  SOURCES
    trace_buffer.cc
  PRIVATE_DEPS
    pw_assert
    pw_bytes
    pw_log
  PUBLIC_DEPS
    pw_ring_buffer
    pw_status
    pw_tokenizer
    pw_trace_tokenized
    pw_trace_tokenized.config
    pw_varint
)

pw_proto_library(pw_trace_tokenized.protos
  SOURCES
    pw_trace_protos/trace_rpc.proto
  INPUTS
    pw_trace_protos/trace_rpc.options
)

pw_add_library(pw_trace_tokenized.rpc_service STATIC
  SOURCES
    trace_rpc_service_nanopb.cc
  PRIVATE_DEPS
    pw_assert
    pw_log
    pw_ring_buffer
    pw_trace_tokenized.trace_buffer
    pw_trace_tokenized.protos.nanopb_rpc
    pw_varint
  PUBLIC_DEPS
    pw_tokenizer
    pw_status
)
