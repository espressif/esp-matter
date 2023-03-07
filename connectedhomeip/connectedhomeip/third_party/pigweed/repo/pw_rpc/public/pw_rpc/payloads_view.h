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

// The classes in this file facilitate testing RPC services. The main class is
// PayloadsView, which iterates over the payloads sent by an RPC service or
// client. This allows verifying that code that invokes RPCs or an RPC service
// implementation sends the expected requests or responses.
//
// This code is inteded for testing, not for deployment.
#pragma once

#include <tuple>

#include "pw_containers/filtered_view.h"
#include "pw_containers/vector.h"
#include "pw_containers/wrapped_iterator.h"
#include "pw_rpc/channel.h"
#include "pw_rpc/internal/method_info.h"
#include "pw_rpc/internal/packet.h"
#include "pw_rpc/method_type.h"

namespace pw::rpc {
namespace internal::test {

class FakeChannelOutput;

// Finds packets of a specified type for a particular method.
class PacketFilter {
 public:
  // Use Channel::kUnassignedChannelId to ignore the channel.
  constexpr PacketFilter(internal::pwpb::PacketType packet_type_1,
                         internal::pwpb::PacketType packet_type_2,
                         uint32_t channel_id,
                         uint32_t service_id,
                         uint32_t method_id)
      : packet_type_1_(packet_type_1),
        packet_type_2_(packet_type_2),
        channel_id_(channel_id),
        service_id_(service_id),
        method_id_(method_id) {}

  constexpr bool operator()(const Packet& packet) const {
    return (packet.type() == packet_type_1_ ||
            packet.type() == packet_type_2_) &&
           (channel_id_ == Channel::kUnassignedChannelId ||
            packet.channel_id() == channel_id_) &&
           packet.service_id() == service_id_ &&
           packet.method_id() == method_id_;
  }

 private:
  // Support filtering on two packet types to handle reading both client and
  // server streams for bidirectional streams.
  internal::pwpb::PacketType packet_type_1_;
  internal::pwpb::PacketType packet_type_2_;
  uint32_t channel_id_;
  uint32_t service_id_;
  uint32_t method_id_;
};

using PacketsView = containers::FilteredView<Vector<Packet>, PacketFilter>;

}  // namespace internal::test

// Returns the payloads for a particular RPC in a Vector of RPC packets.
//
// Adapts a FilteredView of packets to return payloads instead of packets.
class PayloadsView {
 public:
  class iterator : public containers::WrappedIterator<
                       iterator,
                       internal::test::PacketsView::iterator,
                       ConstByteSpan> {
   public:
    constexpr iterator() = default;

    // Access the payload (rather than packet) with operator* and operator->.
    const ConstByteSpan& operator*() const { return value().payload(); }
    const ConstByteSpan* operator->() const { return &value().payload(); }

   private:
    friend class PayloadsView;

    constexpr iterator(const internal::test::PacketsView::iterator& it)
        : containers::WrappedIterator<iterator,
                                      internal::test::PacketsView::iterator,
                                      ConstByteSpan>(it) {}
  };

  using const_iterator = iterator;

  const ConstByteSpan& operator[](size_t index) const {
    auto it = begin();
    std::advance(it, index);
    return *it;
  }

  // Number of payloads for the specified RPC.
  size_t size() const { return view_.size(); }

  bool empty() const { return begin() == end(); }

  // Returns the first/last payload for the RPC. size() must be > 0.
  const ConstByteSpan& front() const { return *begin(); }
  const ConstByteSpan& back() const { return *std::prev(end()); }

  iterator begin() const { return iterator(view_.begin()); }
  iterator end() const { return iterator(view_.end()); }

 private:
  friend class internal::test::FakeChannelOutput;

  template <typename>
  friend class NanopbPayloadsView;

  template <typename>
  friend class PwpbPayloadsView;

  template <auto kMethod>
  using MethodInfo = internal::MethodInfo<kMethod>;

  using PacketType = internal::pwpb::PacketType;

  template <auto kMethod>
  static constexpr PayloadsView For(const Vector<internal::Packet>& packets,
                                    uint32_t channel_id) {
    constexpr auto kTypes = PacketTypesWithPayload(MethodInfo<kMethod>::kType);
    return PayloadsView(packets,
                        std::get<0>(kTypes),
                        std::get<1>(kTypes),
                        channel_id,
                        MethodInfo<kMethod>::kServiceId,
                        MethodInfo<kMethod>::kMethodId);
  }

  constexpr PayloadsView(const Vector<internal::Packet>& packets,
                         MethodType method_type,
                         uint32_t channel_id,
                         uint32_t service_id,
                         uint32_t method_id)
      : PayloadsView(packets,
                     std::get<0>(PacketTypesWithPayload(method_type)),
                     std::get<1>(PacketTypesWithPayload(method_type)),
                     channel_id,
                     service_id,
                     method_id) {}

  constexpr PayloadsView(const Vector<internal::Packet>& packets,
                         PacketType packet_type_1,
                         PacketType packet_type_2,
                         uint32_t channel_id,
                         uint32_t service_id,
                         uint32_t method_id)
      : view_(packets,
              internal::test::PacketFilter(packet_type_1,
                                           packet_type_2,
                                           channel_id,
                                           service_id,
                                           method_id)) {}

  static constexpr std::tuple<PacketType, PacketType> PacketTypesWithPayload(
      MethodType method_type) {
    switch (method_type) {
      case MethodType::kUnary:
        return {PacketType::REQUEST, PacketType::RESPONSE};
      case MethodType::kServerStreaming:
        return {PacketType::REQUEST, PacketType::SERVER_STREAM};
      case MethodType::kClientStreaming:
        return {PacketType::CLIENT_STREAM, PacketType::RESPONSE};
      case MethodType::kBidirectionalStreaming:
        return {PacketType::CLIENT_STREAM, PacketType::SERVER_STREAM};
    }

// Workaround for GCC 8 bug: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=86678
#if defined(__GNUC__) && __GNUC__ < 9
    return {};
#else
    PW_ASSERT(false);
#endif  // defined(__GNUC__) && __GNUC__ < 9
  }

  internal::test::PacketsView view_;
};

// Class for iterating over RPC statuses associated witha particular RPC. This
// is used to iterate over the user RPC statuses and or protocol errors for a
// particular RPC.
class StatusView {
 public:
  class iterator : public containers::WrappedIterator<
                       iterator,
                       internal::test::PacketsView::iterator,
                       Status> {
   public:
    constexpr iterator() = default;

    // Access the status (rather than packet) with operator* and operator->.
    const Status& operator*() const { return value().status(); }
    const Status* operator->() const { return &value().status(); }

   private:
    friend class StatusView;

    constexpr iterator(const internal::test::PacketsView::iterator& it)
        : containers::WrappedIterator<iterator,
                                      internal::test::PacketsView::iterator,
                                      Status>(it) {}
  };

  using const_iterator = iterator;

  const Status& operator[](size_t index) const {
    auto it = begin();
    std::advance(it, index);
    return *it;
  }

  // Number of statuses in this view.
  size_t size() const { return view_.size(); }
  bool empty() const { return begin() == end(); }

  // Returns the first/last payload for the RPC. size() must be > 0.
  const Status& front() const { return *begin(); }
  const Status& back() const { return *std::prev(end()); }

  iterator begin() const { return iterator(view_.begin()); }
  iterator end() const { return iterator(view_.end()); }

 private:
  friend class internal::test::FakeChannelOutput;

  template <auto kMethod>
  using MethodInfo = internal::MethodInfo<kMethod>;

  using PacketType = internal::pwpb::PacketType;

  constexpr StatusView(const Vector<internal::Packet>& packets,
                       PacketType packet_type_1,
                       PacketType packet_type_2,
                       uint32_t channel_id,
                       uint32_t service_id,
                       uint32_t method_id)
      : view_(packets,
              internal::test::PacketFilter(packet_type_1,
                                           packet_type_2,
                                           channel_id,
                                           service_id,
                                           method_id)) {}

  internal::test::PacketsView view_;
};

}  // namespace pw::rpc
