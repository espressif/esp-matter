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

#include <cstdint>

#include "pw_rpc/client.h"

namespace pw::rpc::internal {

// This class serves as the base for generated RpcService::Client classes.
// Generated service classes extend this and add a member function for each
// service method.
class ServiceClient {
 public:
  constexpr ServiceClient(const ServiceClient&) = default;
  constexpr ServiceClient& operator=(const ServiceClient&) = default;

  constexpr Client& client() const { return *client_; }

  constexpr uint32_t channel_id() const { return channel_id_; }

 protected:
  constexpr ServiceClient(Client& client, uint32_t channel_id)
      : client_(&client), channel_id_(channel_id) {}

 private:
  Client* client_;
  uint32_t channel_id_;
};

}  // namespace pw::rpc::internal
