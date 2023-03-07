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

#include "pw_rpc/internal/channel_list.h"

namespace pw::rpc::internal {

const Channel* ChannelList::Get(uint32_t channel_id) const {
  for (const Channel& channel : channels_) {
    if (channel.id() == channel_id) {
      return &channel;
    }
  }
  return nullptr;
}

Status ChannelList::Add(uint32_t channel_id, ChannelOutput& output) {
  if (Get(channel_id) != nullptr) {
    return Status::AlreadyExists();
  }

#if PW_RPC_DYNAMIC_ALLOCATION
  channels_.emplace_back(channel_id, &output);
#else
  Channel* new_channel = Get(Channel::kUnassignedChannelId);
  if (new_channel == nullptr) {
    return Status::ResourceExhausted();
  }

  new_channel->Configure(channel_id, output);
#endif  // PW_RPC_DYNAMIC_ALLOCATION

  return OkStatus();
}

Status ChannelList::Remove(uint32_t channel_id) {
  Channel* channel = Get(channel_id);

  if (channel == nullptr) {
    return Status::NotFound();
  }
  channel->Close();

#if PW_RPC_DYNAMIC_ALLOCATION
  // Order isn't important, so move the channel to the back then pop it.
  std::swap(*channel, channels_.back());
  channels_.pop_back();
#endif  // PW_RPC_DYNAMIC_ALLOCATION

  return OkStatus();
}

}  // namespace pw::rpc::internal
