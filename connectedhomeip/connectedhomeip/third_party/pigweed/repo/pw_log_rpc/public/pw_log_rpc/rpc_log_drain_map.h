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

#include "pw_log_rpc/rpc_log_drain.h"
#include "pw_result/result.h"
#include "pw_span/span.h"
#include "pw_status/status.h"

namespace pw::log_rpc {

// Holds an inmutable map of RPC channel ID to RpcLogDrain to fascilitate the
// maintenance of all RPC log streams.
class RpcLogDrainMap {
 public:
  explicit constexpr RpcLogDrainMap(span<RpcLogDrain> drains)
      : drains_(drains) {}

  // Not copyable nor movable.
  RpcLogDrainMap(RpcLogDrainMap const&) = delete;
  RpcLogDrainMap& operator=(RpcLogDrainMap const&) = delete;
  RpcLogDrainMap(RpcLogDrainMap&&) = delete;
  RpcLogDrainMap& operator=(RpcLogDrainMap&&) = delete;

  Result<RpcLogDrain*> GetDrainFromChannelId(uint32_t channel_id) const {
    for (auto& drain : drains_) {
      if (drain.channel_id() == channel_id) {
        return &drain;
      }
    }
    return Status::NotFound();
  }

  const span<RpcLogDrain>& drains() const { return drains_; }

 protected:
  const span<RpcLogDrain> drains_;
};

}  // namespace pw::log_rpc
