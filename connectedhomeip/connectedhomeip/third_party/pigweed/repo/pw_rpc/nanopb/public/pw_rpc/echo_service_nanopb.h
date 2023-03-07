// Copyright 2020 The Pigweed Authors
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

#include <cstring>

#include "pw_rpc/echo.rpc.pb.h"

namespace pw::rpc {

class EchoService final
    : public pw_rpc::nanopb::EchoService::Service<EchoService> {
 public:
  Status Echo(const pw_rpc_EchoMessage& request, pw_rpc_EchoMessage& response) {
    std::strncpy(response.msg, request.msg, sizeof(response.msg));
    return OkStatus();
  }
};

}  // namespace pw::rpc
