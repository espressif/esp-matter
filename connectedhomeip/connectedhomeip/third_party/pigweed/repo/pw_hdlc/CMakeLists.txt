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

add_subdirectory(rpc_example)

pw_add_library(pw_hdlc INTERFACE
  PUBLIC_DEPS
    pw_hdlc.decoder
    pw_hdlc.encoded_size
    pw_hdlc.encoder
)

pw_add_library(pw_hdlc.common INTERFACE
  HEADERS
    public/pw_hdlc/internal/protocol.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_varint
)

pw_add_library(pw_hdlc.encoded_size INTERFACE
  HEADERS
    public/pw_hdlc/encoded_size.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_hdlc.common
    pw_bytes
    pw_span
    pw_varint
)

pw_add_library(pw_hdlc.decoder STATIC
  HEADERS
    public/pw_hdlc/decoder.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_hdlc.common
    pw_bytes
    pw_checksum
    pw_result
    pw_span
    pw_status
  PRIVATE_DEPS
    pw_log
  SOURCES
    decoder.cc
)

pw_add_library(pw_hdlc.encoder STATIC
  HEADERS
    public/pw_hdlc/encoder.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_hdlc.common
    pw_bytes
    pw_checksum
    pw_span
    pw_status
    pw_stream
  PRIVATE_DEPS
    pw_hdlc.encoded_size
  SOURCES
    encoder.cc
    public/pw_hdlc/internal/encoder.h
)

pw_add_library(pw_hdlc.rpc_channel_output INTERFACE
  HEADERS
    public/pw_hdlc/rpc_channel.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_hdlc
    pw_rpc.server
    pw_span
)

pw_add_library(pw_hdlc.pw_rpc STATIC
  HEADERS
    public/pw_hdlc/rpc_packets.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_hdlc
    pw_rpc.server
    pw_sys_io
  SOURCES
    rpc_packets.cc
)

pw_add_library(pw_hdlc.packet_parser STATIC
  HEADERS
    public/pw_hdlc/wire_packet_parser.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_hdlc
    pw_router.packet_parser
    PRIVATE_DEPS
    pw_bytes
    pw_checksum
  SOURCES
    wire_packet_parser.cc
)

pw_add_library(pw_hdlc.hdlc_sys_io_system_server STATIC
  PRIVATE_DEPS
    pw_hdlc.pw_rpc
    pw_hdlc.rpc_channel_output
    pw_rpc.system_server.facade
    pw_stream.sys_io_stream
    pw_log
  SOURCES
    hdlc_sys_io_system_server.cc
)

if(Zephyr_FOUND AND CONFIG_PIGWEED_HDLC_RPC)
  zephyr_link_libraries(pw_hdlc.pw_rpc)
endif()

pw_add_test(pw_hdlc.decoder_test
  SOURCES
    decoder_test.cc
  PRIVATE_DEPS
    pw_bytes
    pw_hdlc
  GROUPS
    modules
    pw_hdlc
)

pw_add_test(pw_hdlc.rpc_channel_test
  SOURCES
    rpc_channel_test.cc
  PRIVATE_DEPS
    pw_hdlc
    pw_hdlc.rpc_channel_output
  GROUPS
    modules
    pw_hdlc
)

pw_add_test(pw_hdlc.wire_packet_parser_test
  SOURCES
    wire_packet_parser_test.cc
  PRIVATE_DEPS
    pw_bytes
    pw_hdlc.packet_parser
  GROUPS
    modules
    pw_hdlc
)
