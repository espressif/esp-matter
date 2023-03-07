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
#include <iterator>
#include <limits>

#include "pw_bytes/span.h"
#include "pw_containers/vector.h"
#include "pw_function/function.h"
#include "pw_rpc/channel.h"
#include "pw_rpc/internal/lock.h"
#include "pw_rpc/internal/method_info.h"
#include "pw_rpc/internal/packet.h"
#include "pw_rpc/method_type.h"
#include "pw_rpc/payloads_view.h"
#include "pw_sync/lock_annotations.h"

namespace pw::rpc {
namespace internal {

// Forward declare for a friend statement.
template <class, size_t, size_t, size_t>
class ForwardingChannelOutput;

}  // namespace internal
}  // namespace pw::rpc

namespace pw::rpc {

class FakeServer;

namespace internal::test {

// A ChannelOutput implementation that stores outgoing packets.
class FakeChannelOutput : public ChannelOutput {
 public:
  FakeChannelOutput(const FakeChannelOutput&) = delete;
  FakeChannelOutput(FakeChannelOutput&&) = delete;

  FakeChannelOutput& operator=(const FakeChannelOutput&) = delete;
  FakeChannelOutput& operator=(FakeChannelOutput&&) = delete;

  Status last_status() const PW_LOCKS_EXCLUDED(mutex_) {
    LockGuard lock(mutex_);
    PW_ASSERT(total_response_packets_ > 0);
    return packets_.back().status();
  }

  // Returns a view of the payloads seen for this RPC.
  //
  // !!! WARNING !!!
  //
  // Access to the FakeChannelOutput through the PayloadsView is NOT
  // synchronized! The PayloadsView is immediately invalidated if any thread
  // accesses the FakeChannelOutput.
  template <auto kMethod>
  PayloadsView payloads(uint32_t channel_id = Channel::kUnassignedChannelId)
      const PW_LOCKS_EXCLUDED(mutex_) {
    LockGuard lock(mutex_);
    return PayloadsView(packets_,
                        MethodInfo<kMethod>::kType,
                        channel_id,
                        MethodInfo<kMethod>::kServiceId,
                        MethodInfo<kMethod>::kMethodId);
  }

  PayloadsView payloads(MethodType type,
                        uint32_t channel_id,
                        uint32_t service_id,
                        uint32_t method_id) const PW_LOCKS_EXCLUDED(mutex_) {
    LockGuard lock(mutex_);
    return PayloadsView(packets_, type, channel_id, service_id, method_id);
  }

  // Returns a number of the payloads seen for this RPC.
  template <auto kMethod>
  size_t total_payloads(uint32_t channel_id = Channel::kUnassignedChannelId)
      const PW_LOCKS_EXCLUDED(mutex_) {
    LockGuard lock(mutex_);
    return PayloadsView(packets_,
                        MethodInfo<kMethod>::kType,
                        channel_id,
                        MethodInfo<kMethod>::kServiceId,
                        MethodInfo<kMethod>::kMethodId)
        .size();
  }

  // Returns a number of the payloads seen for this RPC.
  size_t total_payloads(MethodType type,
                        uint32_t channel_id,
                        uint32_t service_id,
                        uint32_t method_id) const PW_LOCKS_EXCLUDED(mutex_) {
    LockGuard lock(mutex_);
    return PayloadsView(packets_, type, channel_id, service_id, method_id)
        .size();
  }

  // Returns a view of the final statuses seen for this RPC. Only relevant for
  // checking packets sent by a server.
  //
  // !!! WARNING !!!
  //
  // Access to the FakeChannelOutput through the StatusView is NOT
  // synchronized! The StatusView is immediately invalidated if any thread
  // accesses the FakeChannelOutput.
  template <auto kMethod>
  StatusView completions(uint32_t channel_id = Channel::kUnassignedChannelId)
      const PW_LOCKS_EXCLUDED(mutex_) {
    LockGuard lock(mutex_);
    return StatusView(packets_,
                      internal::pwpb::PacketType::RESPONSE,
                      internal::pwpb::PacketType::RESPONSE,
                      channel_id,
                      MethodInfo<kMethod>::kServiceId,
                      MethodInfo<kMethod>::kMethodId);
  }

  // Returns a view of the pw_rpc server or client errors seen for this RPC.
  //
  // !!! WARNING !!!
  //
  // Access to the FakeChannelOutput through the StatusView is NOT
  // synchronized! The StatusView is immediately invalidated if any thread
  // accesses the FakeChannelOutput.
  template <auto kMethod>
  StatusView errors(uint32_t channel_id = Channel::kUnassignedChannelId) const
      PW_LOCKS_EXCLUDED(mutex_) {
    LockGuard lock(mutex_);
    return StatusView(packets_,
                      internal::pwpb::PacketType::CLIENT_ERROR,
                      internal::pwpb::PacketType::SERVER_ERROR,
                      channel_id,
                      MethodInfo<kMethod>::kServiceId,
                      MethodInfo<kMethod>::kMethodId);
  }

  // Returns a view of the client stream end packets seen for this RPC. Only
  // relevant for checking packets sent by a client.
  template <auto kMethod>
  size_t client_stream_end_packets(
      uint32_t channel_id = Channel::kUnassignedChannelId) const
      PW_LOCKS_EXCLUDED(mutex_) {
    LockGuard lock(mutex_);
    return internal::test::PacketsView(
               packets_,
               internal::test::PacketFilter(
                   internal::pwpb::PacketType::CLIENT_STREAM_END,
                   internal::pwpb::PacketType::CLIENT_STREAM_END,
                   channel_id,
                   MethodInfo<kMethod>::kServiceId,
                   MethodInfo<kMethod>::kMethodId))
        .size();
  }

  // The maximum number of packets this FakeChannelOutput can store. Attempting
  // to store more packets than this is an error.
  size_t max_packets() const PW_LOCKS_EXCLUDED(mutex_) {
    LockGuard lock(mutex_);
    return packets_.max_size();
  }

  // The total number of packets that have been sent.
  size_t total_packets() const PW_LOCKS_EXCLUDED(mutex_) {
    LockGuard lock(mutex_);
    return packets_.size();
  }

  // Set to true if a RESPONSE packet is seen.
  bool done() const PW_LOCKS_EXCLUDED(mutex_) {
    LockGuard lock(mutex_);
    return total_response_packets_ > 0;
  }

  // Clears and resets the FakeChannelOutput.
  void clear() PW_LOCKS_EXCLUDED(mutex_);

  // Returns `status` for all future Send calls. Enables packet processing if
  // `status` is OK.
  void set_send_status(Status status) PW_LOCKS_EXCLUDED(mutex_) {
    LockGuard lock(mutex_);
    send_status_ = status;
    return_after_packet_count_ = status.ok() ? -1 : 0;
  }

  // Returns `status` once after the specified positive number of packets.
  void set_send_status(Status status, int return_after_packet_count)
      PW_LOCKS_EXCLUDED(mutex_) {
    LockGuard lock(mutex_);
    PW_ASSERT(!status.ok());
    PW_ASSERT(return_after_packet_count > 0);
    send_status_ = status;
    return_after_packet_count_ = return_after_packet_count;
  }

  // Logs which packets have been sent for debugging purposes.
  void LogPackets() const PW_LOCKS_EXCLUDED(mutex_);

  // Processes buffer according to packet type and `return_after_packet_count_`
  // value as follows:
  // When positive, returns `send_status_` once,
  // When equals 0, returns `send_status_` in all future calls,
  // When negative, ignores `send_status_` processes buffer.
  Status Send(ConstByteSpan buffer) final PW_LOCKS_EXCLUDED(mutex_) {
    LockGuard lock(mutex_);
    const Status status = HandlePacket(buffer);
    if (on_send_ != nullptr) {
      on_send_(buffer, status);
    }
    return status;
  }

  // Gives access to the last received internal::Packet. This is hidden by the
  // raw/Nanopb implementations, since it gives access to an internal class.
  const Packet& last_packet() const PW_LOCKS_EXCLUDED(mutex_) {
    LockGuard lock(mutex_);
    PW_ASSERT(!packets_.empty());
    return packets_.back();
  }

  // The on_send callback is called every time Send() is called. It is passed
  // the contents of the packet and the status to be returned from Send().
  //
  // DANGER: Do NOT call any FakeChannelOutput functions or functions that call
  // FakeChannelOutput functions. That will result in infinite recursion or
  // deadlocks.
  void set_on_send(Function<void(ConstByteSpan, Status)>&& on_send)
      PW_LOCKS_EXCLUDED(mutex_) {
    LockGuard lock(mutex_);
    on_send_ = std::move(on_send);
  }

 protected:
  FakeChannelOutput(Vector<Packet>& packets, Vector<std::byte>& payloads)
      : ChannelOutput("pw::rpc::internal::test::FakeChannelOutput"),
        packets_(packets),
        payloads_(payloads) {}

  const Vector<Packet>& packets() const PW_EXCLUSIVE_LOCKS_REQUIRED(mutex_) {
    return packets_;
  }

  RpcLock& mutex() const { return mutex_; }

 private:
  friend class rpc::FakeServer;
  template <class, size_t, size_t, size_t>
  friend class internal::ForwardingChannelOutput;

  Status HandlePacket(ConstByteSpan buffer) PW_EXCLUSIVE_LOCKS_REQUIRED(mutex_);
  void CopyPayloadToBuffer(Packet& packet) PW_EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  int return_after_packet_count_ PW_GUARDED_BY(mutex_) = -1;
  unsigned total_response_packets_ PW_GUARDED_BY(mutex_) = 0;

  Vector<Packet>& packets_ PW_GUARDED_BY(mutex_);
  Vector<std::byte>& payloads_ PW_GUARDED_BY(mutex_);
  Status send_status_ PW_GUARDED_BY(mutex_) = OkStatus();
  Function<void(ConstByteSpan, Status)> on_send_ PW_GUARDED_BY(mutex_);

  mutable RpcLock mutex_;
};

// Adds the packet output buffer to a FakeChannelOutput.
template <size_t kMaxPackets, size_t kPayloadsBufferSizeBytes>
class FakeChannelOutputBuffer : public FakeChannelOutput {
 protected:
  FakeChannelOutputBuffer()
      : FakeChannelOutput(packets_array_, payloads_array_), payloads_array_{} {}

  Vector<std::byte, kPayloadsBufferSizeBytes> payloads_array_;
  Vector<Packet, kMaxPackets> packets_array_;
};

}  // namespace internal::test
}  // namespace pw::rpc
