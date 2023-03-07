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

include("$ENV{PW_ROOT}/pw_build/pigweed.cmake")
include($ENV{PW_ROOT}/pw_rpc/system_server/backend.cmake)

pw_add_facade(pw_rpc.system_server INTERFACE
  BACKEND
    pw_rpc.system_server_BACKEND
  HEADERS
    public/pw_rpc_system_server/rpc_server.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_rpc.server
    pw_stream
    pw_function
    pw_result
)

pw_add_library(pw_rpc.system_server.socket INTERFACE
  HEADERS
    public/pw_rpc_system_server/socket.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_rpc.system_server
)
