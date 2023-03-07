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

pw_add_library(pw_snapshot.uuid STATIC
  HEADERS
    public/pw_snapshot/uuid.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_bytes
    pw_result
    pw_span
  SOURCES
    uuid.cc
  PRIVATE_DEPS
    pw_protobuf
    pw_snapshot.metadata_proto.pwpb
)

# This proto library only contains the snapshot_metadata.proto. Typically this
# should be a dependency of snapshot-like protos.
pw_proto_library(pw_snapshot.metadata_proto
  SOURCES
    pw_snapshot_protos/snapshot_metadata.proto
  STRIP_PREFIX
    pw_snapshot_protos
  PREFIX
    pw_snapshot_metadata_proto
  DEPS
    pw_tokenizer.proto
)

# This proto provides the complete "Snapshot" proto, which depends on various
# proto libraries throughout Pigweed. This is the proto library to use when
# building/reading snapshots.
pw_proto_library(pw_snapshot.snapshot_proto
  SOURCES
    pw_snapshot_protos/snapshot.proto
  DEPS
    pw_chrono.protos
    pw_cpu_exception_cortex_m.cpu_state_protos
    pw_log.protos
    pw_snapshot.metadata_proto
    pw_thread.protos
)

pw_add_test(pw_snapshot.cpp_compile_test
  SOURCES
    cpp_compile_test.cc
  PRIVATE_DEPS
    pw_protobuf
    pw_snapshot.snapshot_proto.pwpb
  GROUPS
    modules
    pw_snapshot
)

pw_add_test(pw_snapshot.uuid_test
  SOURCES
    uuid_test.cc
  PRIVATE_DEPS
    pw_bytes
    pw_span
    pw_protobuf
    pw_result
    pw_snapshot.metadata_proto.pwpb
    pw_snapshot.uuid
    pw_status
  GROUPS
    modules
    pw_snapshot
)
