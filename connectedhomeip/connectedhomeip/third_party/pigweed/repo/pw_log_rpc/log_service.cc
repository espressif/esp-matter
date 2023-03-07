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

// clang-format off
#include "pw_log_rpc/internal/log_config.h" // PW_LOG_* macros must be first.

#include "pw_log_rpc/log_service.h"
// clang-format on

#include "pw_log/log.h"
#include "pw_log/proto/log.pwpb.h"

namespace pw::log_rpc {

void LogService::Listen(ConstByteSpan, rpc::RawServerWriter& writer) {
  uint32_t channel_id = writer.channel_id();
  Result<RpcLogDrain*> drain = drains_.GetDrainFromChannelId(channel_id);
  if (!drain.ok()) {
    return;
  }

  if (const Status status = drain.value()->Open(writer); !status.ok()) {
    PW_LOG_DEBUG("Could not start new log stream. %d",
                 static_cast<int>(status.code()));
  }
}

}  // namespace pw::log_rpc
