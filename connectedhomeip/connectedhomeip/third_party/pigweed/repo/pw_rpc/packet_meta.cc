// Copyright 2022 The Pigweed Authors
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
#include "pw_rpc/internal/log_config.h"  // PW_LOG_* macros must be first.

#include "pw_rpc/packet_meta.h"
// clang-format on

#include "pw_log/log.h"
#include "pw_rpc/internal/channel.h"
#include "pw_rpc/internal/packet.h"
#include "pw_span/span.h"
#include "pw_status/status_with_size.h"

namespace pw::rpc {

Result<PacketMeta> PacketMeta::FromBuffer(ConstByteSpan data) {
  PW_TRY_ASSIGN(internal::Packet packet, internal::Packet::FromBuffer(data));
  if (packet.channel_id() == internal::Channel::kUnassignedChannelId ||
      packet.service_id() == 0 || packet.method_id() == 0) {
    PW_LOG_WARN("Received malformed pw_rpc packet");
    return Status::DataLoss();
  }
  return PacketMeta(packet);
}

}  // namespace pw::rpc