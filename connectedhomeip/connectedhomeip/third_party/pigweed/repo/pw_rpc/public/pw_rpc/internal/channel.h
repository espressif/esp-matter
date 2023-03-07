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

#include "pw_assert/assert.h"
#include "pw_bytes/span.h"
#include "pw_rpc/channel.h"
#include "pw_rpc/internal/lock.h"
#include "pw_rpc/internal/packet.h"
#include "pw_span/span.h"
#include "pw_status/status.h"

namespace pw::rpc::internal {

// Returns a portion of the encoding buffer that may be used to encode an
// outgoing payload.
ByteSpan GetPayloadBuffer() PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock());

class Channel : public rpc::Channel {
 public:
  Channel() = delete;

  constexpr Channel(uint32_t id, ChannelOutput* output)
      : rpc::Channel(id, output) {}

  // Allow closing a channel for internal API users.
  using rpc::Channel::Close;

  // Allow setting the channel ID for tests.
  using rpc::Channel::set_channel_id;

  Status Send(const Packet& packet) PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock());
};

}  // namespace pw::rpc::internal
