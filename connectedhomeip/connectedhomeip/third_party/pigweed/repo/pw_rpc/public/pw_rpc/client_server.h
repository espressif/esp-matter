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
#pragma once

#include "pw_rpc/client.h"
#include "pw_rpc/server.h"

namespace pw::rpc {

// Class that wraps both an RPC client and a server, simplifying RPC setup when
// a device needs to function as both.
class ClientServer {
 public:
  _PW_RPC_CONSTEXPR ClientServer(span<Channel> channels)
      : client_(channels), server_(channels) {}

  // Sends a packet to either the client or the server, depending on its type.
  Status ProcessPacket(ConstByteSpan packet);

  constexpr Client& client() { return client_; }
  constexpr Server& server() { return server_; }

 private:
  Client client_;
  Server server_;
};

}  // namespace pw::rpc
