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

#include "pw_rpc/internal/server_call.h"

namespace pw::rpc::internal {

void ServerCall::MoveServerCallFrom(ServerCall& other) {
  // If this call is active, finish it first.
  if (active_locked()) {
    CloseAndSendResponseLocked(OkStatus()).IgnoreError();
  }

  MoveFrom(other);

#if PW_RPC_CLIENT_STREAM_END_CALLBACK
  on_client_stream_end_ = std::move(other.on_client_stream_end_);
#endif  // PW_RPC_CLIENT_STREAM_END_CALLBACK
}

}  // namespace pw::rpc::internal
