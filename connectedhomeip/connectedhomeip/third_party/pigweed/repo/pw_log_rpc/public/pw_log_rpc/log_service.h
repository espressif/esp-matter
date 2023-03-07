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

#include "pw_log/proto/log.raw_rpc.pb.h"
#include "pw_log_rpc/rpc_log_drain_map.h"
#include "pw_status/status.h"

namespace pw::log_rpc {

// The RPC LogService provides a way to start a log stream on a known RPC
// channel with a writer provided on a call. Log streams maintenance is flexible
// and delegated outside the service.
class LogService final : public log::pw_rpc::raw::Logs::Service<LogService> {
 public:
  LogService(RpcLogDrainMap& drains) : drains_(drains) {}

  // Starts listening to logs on the given RPC channel and writer. The call is
  // ignored if the channel was not pre-registered in the drain map. If there is
  // an existent stream of logs for the given channel and previous writer, the
  // writer in this call is closed without finishing the RPC call and the log
  // stream using the previous writer continues.
  void Listen(ConstByteSpan, rpc::RawServerWriter& writer);

 private:
  RpcLogDrainMap& drains_;
};

}  // namespace pw::log_rpc
