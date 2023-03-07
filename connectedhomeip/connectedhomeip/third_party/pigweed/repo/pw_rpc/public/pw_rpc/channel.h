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

#include <cstdint>
#include <limits>
#include <type_traits>

#include "pw_assert/assert.h"
#include "pw_bytes/span.h"
#include "pw_result/result.h"
#include "pw_rpc/internal/lock.h"
#include "pw_rpc/internal/packet.h"
#include "pw_span/span.h"
#include "pw_status/status.h"

namespace pw::rpc {

// Extracts the channel ID from a pw_rpc packet. Returns DATA_LOSS if the
// packet is corrupt and the channel ID could not be found.
Result<uint32_t> ExtractChannelId(ConstByteSpan packet);

// Returns the maximum size of the payload of an RPC packet. This can be used
// when allocating response encode buffers for RPC services.
// If the RPC encode buffer is too small to fit RPC packet headers, this will
// return zero.
constexpr size_t MaxSafePayloadSize(
    size_t encode_buffer_size = cfg::kEncodingBufferSizeBytes) {
  return encode_buffer_size > internal::Packet::kMinEncodedSizeWithoutPayload
             ? encode_buffer_size -
                   internal::Packet::kMinEncodedSizeWithoutPayload
             : 0;
}

class ChannelOutput {
 public:
  // Returned from MaximumTransmissionUnit() to indicate that this ChannelOutput
  // imposes no limits on the MTU.
  static constexpr size_t kUnlimited = std::numeric_limits<size_t>::max();

  // Creates a channel output with the provided name. The name is used for
  // logging only.
  constexpr ChannelOutput(const char* name) : name_(name) {}

  virtual ~ChannelOutput() = default;

  constexpr const char* name() const { return name_; }

  // Returns the maximum transmission unit that this ChannelOutput supports. If
  // the ChannelOutput imposes no limit on the MTU, this function returns
  // ChannelOutput::kUnlimited.
  virtual size_t MaximumTransmissionUnit() { return kUnlimited; }

  // Sends an encoded RPC packet. Returns OK if further packets may be sent,
  // even if the current packet could not be sent. Returns any other status if
  // the Channel is no longer able to send packets.
  //
  // The RPC system’s internal lock is held while this function is called. Avoid
  // long-running operations, since these will delay any other users of the RPC
  // system.
  //
  // !!! DANGER !!!
  //
  // No pw_rpc APIs may be accessed in this function! Implementations MUST NOT
  // access any RPC endpoints (pw::rpc::Client, pw::rpc::Server) or call objects
  // (pw::rpc::ServerReaderWriter, pw::rpc::ClientReaderWriter, etc.) inside the
  // Send() function or any descendent calls. Doing so will result in deadlock!
  // RPC APIs may be used by other threads, just not within Send().
  //
  // The buffer provided in packet must NOT be accessed outside of this
  // function. It must be sent immediately or copied elsewhere before the
  // function returns.
  virtual Status Send(span<const std::byte> buffer)
      PW_EXCLUSIVE_LOCKS_REQUIRED(internal::rpc_lock()) = 0;

 private:
  const char* name_;
};

class Channel {
 public:
  static constexpr uint32_t kUnassignedChannelId = 0;

  // Creates a channel with a static ID. The channel's output can also be
  // static, or it can set to null to allow dynamically opening connections
  // through the channel.
  template <uint32_t kId>
  constexpr static Channel Create(ChannelOutput* output) {
    static_assert(kId != kUnassignedChannelId, "Channel ID cannot be 0");
    return Channel(kId, output);
  }

  // Creates a channel with a static ID from an enum value.
  template <auto kId,
            typename T = decltype(kId),
            typename = std::enable_if_t<std::is_enum_v<T>>,
            typename U = std::underlying_type_t<T>>
  constexpr static Channel Create(ChannelOutput* output) {
    constexpr U kIntId = static_cast<U>(kId);
    static_assert(kIntId >= 0, "Channel ID cannot be negative");
    static_assert(kIntId <= std::numeric_limits<uint32_t>::max(),
                  "Channel ID must fit in a uint32");
    return Create<static_cast<uint32_t>(kIntId)>(output);
  }

  // Creates a dynamically assignable channel without a set ID or output.
  constexpr Channel() : id_(kUnassignedChannelId), output_(nullptr) {}

  // TODO(b/234876441): Remove the Configure and set_channel_output functions.
  //     Users should call CloseChannel() / OpenChannel() to change a channel.
  //     This ensures calls are properly update and works consistently between
  //     static and dynamic channel allocation.

  // Manually configures a dynamically-assignable channel with a specified ID
  // and output. This is useful when a channel's parameters are not known until
  // runtime. This can only be called once per channel.
  template <typename UnusedType = void>
  constexpr void Configure(uint32_t id, ChannelOutput& output) {
    static_assert(
        !cfg::kDynamicAllocationEnabled<UnusedType>,
        "Configure() may not be used if PW_RPC_DYNAMIC_ALLOCATION is "
        "enabled. Call CloseChannel/OpenChannel on the endpoint instead.");
    PW_ASSERT(id_ == kUnassignedChannelId);
    PW_ASSERT(id != kUnassignedChannelId);
    id_ = id;
    output_ = &output;
  }

  // Configure using an enum value channel ID.
  template <typename T,
            typename = std::enable_if_t<std::is_enum_v<T>>,
            typename U = std::underlying_type_t<T>>
  constexpr void Configure(T id, ChannelOutput& output) {
    static_assert(
        !cfg::kDynamicAllocationEnabled<T>,
        "Configure() may not be used if PW_RPC_DYNAMIC_ALLOCATION is enabled. "
        "Call CloseChannel/OpenChannel on the endpoint instead.");
    static_assert(sizeof(U) <= sizeof(uint32_t));
    const U kIntId = static_cast<U>(id);
    PW_ASSERT(kIntId > 0);
    return Configure<T>(static_cast<uint32_t>(kIntId), output);
  }

  // Reconfigures a channel with a new output. Depending on the output's
  // implementatation, there might be unintended behavior if the output is in
  // use.
  template <typename UnusedType = void>
  constexpr void set_channel_output(ChannelOutput& output) {
    static_assert(
        !cfg::kDynamicAllocationEnabled<UnusedType>,
        "set_channel_output() may not be used if PW_RPC_DYNAMIC_ALLOCATION is "
        "enabled. Call CloseChannel/OpenChannel on the endpoint instead.");
    PW_ASSERT(id_ != kUnassignedChannelId);
    output_ = &output;
  }

  constexpr uint32_t id() const { return id_; }
  constexpr bool assigned() const { return id_ != kUnassignedChannelId; }

 protected:
  constexpr Channel(uint32_t id, ChannelOutput* output)
      : id_(id), output_(output) {
    PW_ASSERT(id != kUnassignedChannelId);
  }

  ChannelOutput& output() const {
    PW_ASSERT(output_ != nullptr);
    return *output_;
  }

  void set_channel_id(uint32_t channel_id) { id_ = channel_id; }

  constexpr void Close() {
    PW_ASSERT(id_ != kUnassignedChannelId);
    id_ = kUnassignedChannelId;
    output_ = nullptr;
  }

 private:
  uint32_t id_;
  ChannelOutput* output_;
};

}  // namespace pw::rpc
