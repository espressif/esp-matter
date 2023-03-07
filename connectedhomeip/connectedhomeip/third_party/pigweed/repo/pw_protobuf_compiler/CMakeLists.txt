# Copyright 2021 The Pigweed Authors
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

pw_proto_library(pw_protobuf_compiler.pwpb_test_protos
  SOURCES
    pw_protobuf_compiler_pwpb_protos/pwpb_test.proto
  INPUTS
    pw_protobuf_compiler_pwpb_protos/pwpb_test.options
)

pw_add_test(pw_protobuf_compiler.pwpb_test
  SOURCES
    pwpb_test.cc
  PRIVATE_DEPS
    pw_protobuf_compiler.pwpb_test_protos.pwpb
  GROUPS
    pw_protobuf_compiler
)

if(NOT "${dir_pw_third_party_nanopb}" STREQUAL "")
  pw_proto_library(pw_protobuf_compiler.nanopb_test_protos
    SOURCES
      pw_protobuf_compiler_nanopb_protos/nanopb_test.proto
    DEPS
      pw_third_party.nanopb.proto
  )

  pw_add_test(pw_protobuf_compiler.nanopb_test
    SOURCES
      nanopb_test.cc
    PRIVATE_DEPS
      pw_protobuf_compiler.nanopb_test_protos.nanopb
    GROUPS
      pw_protobuf_compiler
  )
endif()
