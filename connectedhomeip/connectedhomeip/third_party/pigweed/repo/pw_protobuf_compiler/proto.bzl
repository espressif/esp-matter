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
"""Embedded-friendly replacement for native.cc_proto_library."""

load(
    "//third_party/rules_proto_grpc:internal_proto.bzl",
    _nanopb_proto_library = "nanopb_proto_library",
    _nanopb_rpc_proto_library = "nanopb_rpc_proto_library",
    _pw_proto_library = "pw_proto_library",
    _pwpb_proto_library = "pwpb_proto_library",
    _pwpb_rpc_proto_library = "pwpb_rpc_proto_library",
    _raw_rpc_proto_library = "raw_rpc_proto_library",
)

# Export internal symbols.
nanopb_proto_library = _nanopb_proto_library
nanopb_rpc_proto_library = _nanopb_rpc_proto_library
pw_proto_library = _pw_proto_library
pwpb_proto_library = _pwpb_proto_library
pwpb_rpc_proto_library = _pwpb_rpc_proto_library
raw_rpc_proto_library = _raw_rpc_proto_library
