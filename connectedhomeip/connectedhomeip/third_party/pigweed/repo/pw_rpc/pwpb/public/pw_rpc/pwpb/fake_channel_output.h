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

#include <cstddef>

#include "pw_containers/wrapped_iterator.h"
#include "pw_rpc/internal/fake_channel_output.h"
#include "pw_rpc/internal/lock.h"
#include "pw_rpc/pwpb/internal/common.h"
#include "pw_rpc/pwpb/internal/method.h"

namespace pw::rpc {
namespace internal {

// Forward declare for a friend statement.
template <typename, size_t, size_t, size_t>
class ForwardingChannelOutput;

}  // namespace internal
}  // namespace pw::rpc

namespace pw::rpc {
namespace internal::test::pwpb {

// Forward declare for a friend statement.
template <typename, auto, uint32_t, size_t, size_t>
class PwpbInvocationContext;

}  // namespace internal::test::pwpb

// PwpbPayloadsView supports iterating over payloads as decoded pw_protobuf
// request or response message structs.
template <typename Payload>
class PwpbPayloadsView {
 public:
  class iterator : public containers::WrappedIterator<iterator,
                                                      PayloadsView::iterator,
                                                      Payload> {
   public:
    // Access the payload (rather than packet) with operator*.
    Payload operator*() const {
      Payload payload{};
      PW_ASSERT(serde_
                    .Decode(containers::WrappedIterator<iterator,
                                                        PayloadsView::iterator,
                                                        Payload>::value(),
                            payload)
                    .ok());
      return payload;
    }

   private:
    friend class PwpbPayloadsView;

    constexpr iterator(const PayloadsView::iterator& it, const PwpbSerde& serde)
        : containers::
              WrappedIterator<iterator, PayloadsView::iterator, Payload>(it),
          serde_(serde) {}

    PwpbSerde serde_;
  };

  Payload operator[](size_t index) const {
    Payload payload{};
    PW_ASSERT(serde_.Decode(view_[index], payload).ok());
    return payload;
  }

  size_t size() const { return view_.size(); }
  bool empty() const { return view_.empty(); }

  // Returns the first/last payload for the RPC. size() must be > 0.
  Payload front() const { return *begin(); }
  Payload back() const { return *std::prev(end()); }

  iterator begin() const { return iterator(view_.begin(), serde_); }
  iterator end() const { return iterator(view_.end(), serde_); }

 private:
  template <size_t, size_t>
  friend class PwpbFakeChannelOutput;

  template <typename... Args>
  PwpbPayloadsView(const PwpbSerde& serde, Args&&... args)
      : view_(args...), serde_(serde) {}

  PayloadsView view_;
  PwpbSerde serde_;
};

// A ChannelOutput implementation that stores the outgoing payloads and status.
template <size_t kMaxPackets, size_t kPayloadsBufferSizeBytes = 128>
class PwpbFakeChannelOutput final
    : public internal::test::FakeChannelOutputBuffer<kMaxPackets,
                                                     kPayloadsBufferSizeBytes> {
 private:
  template <auto kMethod>
  using Request = typename internal::MethodInfo<kMethod>::Request;
  template <auto kMethod>
  using Response = typename internal::MethodInfo<kMethod>::Response;

 public:
  PwpbFakeChannelOutput() = default;

  // Iterates over request payloads from request or client stream packets.
  //
  // !!! WARNING !!!
  //
  // Access to the FakeChannelOutput through the PwpbPayloadsView is NOT
  // synchronized! The PwpbPayloadsView is immediately invalidated if any
  // thread accesses the FakeChannelOutput.
  template <auto kMethod>
  PwpbPayloadsView<Request<kMethod>> requests(
      uint32_t channel_id = Channel::kUnassignedChannelId) const
      PW_NO_LOCK_SAFETY_ANALYSIS {
    constexpr internal::pwpb::PacketType packet_type =
        HasClientStream(internal::MethodInfo<kMethod>::kType)
            ? internal::pwpb::PacketType::CLIENT_STREAM
            : internal::pwpb::PacketType::REQUEST;
    return PwpbPayloadsView<Request<kMethod>>(
        internal::MethodInfo<kMethod>::serde().request(),
        internal::test::FakeChannelOutputBuffer<
            kMaxPackets,
            kPayloadsBufferSizeBytes>::packets(),
        packet_type,
        packet_type,
        channel_id,
        internal::MethodInfo<kMethod>::kServiceId,
        internal::MethodInfo<kMethod>::kMethodId);
  }

  // Iterates over response payloads from response or server stream packets.
  //
  // !!! WARNING !!!
  //
  // Access to the FakeChannelOutput through the PwpbPayloadsView is NOT
  // synchronized! The PwpbPayloadsView is immediately invalidated if any
  // thread accesses the FakeChannelOutput.
  template <auto kMethod>
  PwpbPayloadsView<Response<kMethod>> responses(
      uint32_t channel_id = Channel::kUnassignedChannelId) const
      PW_NO_LOCK_SAFETY_ANALYSIS {
    constexpr internal::pwpb::PacketType packet_type =
        HasServerStream(internal::MethodInfo<kMethod>::kType)
            ? internal::pwpb::PacketType::SERVER_STREAM
            : internal::pwpb::PacketType::RESPONSE;
    return PwpbPayloadsView<Response<kMethod>>(
        internal::MethodInfo<kMethod>::serde().response(),
        internal::test::FakeChannelOutputBuffer<
            kMaxPackets,
            kPayloadsBufferSizeBytes>::packets(),
        packet_type,
        packet_type,
        channel_id,
        internal::MethodInfo<kMethod>::kServiceId,
        internal::MethodInfo<kMethod>::kMethodId);
  }

  template <auto kMethod>
  Response<kMethod> last_response() const {
    internal::LockGuard lock(internal::test::FakeChannelOutput::mutex());
    PwpbPayloadsView<Response<kMethod>> payloads = responses<kMethod>();
    PW_ASSERT(!payloads.empty());
    return payloads.back();
  }

 private:
  template <typename, auto, uint32_t, size_t, size_t>
  friend class internal::test::pwpb::PwpbInvocationContext;
  template <typename, size_t, size_t, size_t>
  friend class internal::ForwardingChannelOutput;

  using internal::test::FakeChannelOutput::last_packet;

  // !!! WARNING !!!
  //
  // Access to the FakeChannelOutput through the PwpbPayloadsView is NOT
  // synchronized! The PwpbPayloadsView is immediately invalidated if any
  // thread accesses the FakeChannelOutput.
  template <typename T>
  PwpbPayloadsView<T> payload_structs(const PwpbSerde& serde,
                                      MethodType type,
                                      uint32_t channel_id,
                                      uint32_t service_id,
                                      uint32_t method_id) const
      PW_NO_LOCK_SAFETY_ANALYSIS {
    return PwpbPayloadsView<T>(serde,
                               internal::test::FakeChannelOutputBuffer<
                                   kMaxPackets,
                                   kPayloadsBufferSizeBytes>::packets(),
                               type,
                               channel_id,
                               service_id,
                               method_id);
  }
};

}  // namespace pw::rpc
