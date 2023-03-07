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

set(dir_pw_third_party_nanopb "" CACHE PATH "Path to the Nanopb installation.")
option(pw_third_party_nanopb_ADD_SUBDIRECTORY
    "Whether to add the dir_pw_third_party_nanopb subdirectory" OFF)

if("${dir_pw_third_party_nanopb}" STREQUAL "")
  pw_add_error_target(pw_third_party.nanopb
    MESSAGE
      "Attempted to use nanopb without configuring it, see "
      "pigweed.dev/third_party/nanopb/"
  )
  return()
elseif(pw_third_party_nanopb_ADD_SUBDIRECTORY)
  add_subdirectory("${dir_pw_third_party_nanopb}" third_party/nanopb)
endif()

set(nanopb_main_library
    $<IF:$<TARGET_EXISTS:protobuf-nanopb-static>,protobuf-nanopb-static,protobuf-nanopb>)

add_library(pw_third_party.nanopb INTERFACE)
target_link_libraries(pw_third_party.nanopb INTERFACE "${nanopb_main_library}")
target_include_directories(pw_third_party.nanopb
  INTERFACE
    "${dir_pw_third_party_nanopb}"
)

pw_proto_library(pw_third_party.nanopb.proto
  SOURCES
    "${dir_pw_third_party_nanopb}/generator/proto/nanopb.proto"
  STRIP_PREFIX
    "${dir_pw_third_party_nanopb}/generator/proto"
)

# Generates nanopb_pb2.py, which is needed to compile protobufs with Nanopb.
add_custom_command(
  COMMAND
    python3 "${CMAKE_CURRENT_LIST_DIR}/generate_nanopb_proto.py" "${dir_pw_third_party_nanopb}"
  OUTPUT
    "${dir_pw_third_party_nanopb}/generator/proto/nanopb_pb2.py"
)
add_custom_target(pw_third_party.nanopb.generate_proto
  DEPENDS
    "${dir_pw_third_party_nanopb}/generator/proto/nanopb_pb2.py"
)
