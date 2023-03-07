// Copyright 2021 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.

#include "pw_rpc/client_server.h"

namespace pw::rpc {

Status ClientServer::ProcessPacket(ConstByteSpan packet) {
  Status status = server_.ProcessPacket(packet);
  if (status.IsInvalidArgument()) {
    // INVALID_ARGUMENT indicates the packet is intended for a client.
    status = client_.ProcessPacket(packet);
  }

  return status;
}

}  // namespace pw::rpc
