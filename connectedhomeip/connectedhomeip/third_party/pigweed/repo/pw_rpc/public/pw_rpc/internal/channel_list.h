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
#pragma once

#include "pw_rpc/internal/channel.h"
#include "pw_rpc/internal/config.h"
#include "pw_span/span.h"

#if PW_RPC_DYNAMIC_ALLOCATION
#include PW_RPC_DYNAMIC_CONTAINER_INCLUDE
#endif  // PW_RPC_DYNAMIC_ALLOCATION

namespace pw::rpc::internal {

#if PW_RPC_DYNAMIC_ALLOCATION && !defined(__cpp_lib_constexpr_vector)
#define _PW_RPC_CONSTEXPR
#else
#define _PW_RPC_CONSTEXPR constexpr
#endif  // PW_RPC_DYNAMIC_ALLOCATION && !defined(__cpp_lib_constexpr_vector)

class ChannelList {
 public:
  _PW_RPC_CONSTEXPR ChannelList() = default;

  // Make this a template so it is only instantiated if it is referred to. This
  // avoids requiring an iterator constructor on the underlying container.
  template <typename Span>
  _PW_RPC_CONSTEXPR ChannelList(Span&& channels_span)
      : channels_(std::begin(channels_span), std::end(channels_span)) {}

  // Returns the first channel with the matching ID or nullptr if none match.
  // Except for Channel::kUnassignedChannelId, there should be no duplicate
  // channels.
  const Channel* Get(uint32_t channel_id) const;

  Channel* Get(uint32_t channel_id) {
    return const_cast<Channel*>(
        static_cast<const ChannelList&>(*this).Get(channel_id));
  }

  // Adds the channel with the requested ID to the list. Returns:
  //
  //   OK - the channel was added
  //   ALREADY_EXISTS - a channel with this ID is already present; remove it
  //       first
  //   RESOURCE_EXHAUSTED - no unassigned channels are available; only possible
  //       if PW_RPC_DYNAMIC_ALLOCATION is disabled
  //
  Status Add(uint32_t channel_id, ChannelOutput& output);

  // Removes the channel with the requested ID. Returns:
  //
  //   OK - the channel was removed
  //   NOT_FOUND - no channel with the provided ID was found
  //
  Status Remove(uint32_t channel_id);

#if PW_RPC_DYNAMIC_ALLOCATION
  PW_RPC_DYNAMIC_CONTAINER(Channel) channels_;
#else
  span<Channel> channels_;
#endif  // PW_RPC_DYNAMIC_ALLOCATION
};

}  // namespace pw::rpc::internal
