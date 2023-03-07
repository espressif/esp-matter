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

// clang-format off
#include "pw_rpc/internal/log_config.h"  // PW_LOG_* macros must be first.

#include "pw_rpc/internal/fake_channel_output.h"
// clang-format on

#include "pw_assert/check.h"
#include "pw_log/log.h"
#include "pw_result/result.h"
#include "pw_rpc/internal/packet.h"

namespace pw::rpc::internal::test {

void FakeChannelOutput::clear() {
  LockGuard lock(mutex_);
  payloads_.clear();
  packets_.clear();
  send_status_ = OkStatus();
  return_after_packet_count_ = -1;
}

Status FakeChannelOutput::HandlePacket(span<const std::byte> buffer) {
  // If the buffer is empty, this is just releasing an unused buffer.
  if (buffer.empty()) {
    return OkStatus();
  }

  if (return_after_packet_count_ == 0) {
    return send_status_;
  }
  if (return_after_packet_count_ > 0 &&
      return_after_packet_count_ == static_cast<int>(packets_.size())) {
    // Disable behavior.
    return_after_packet_count_ = -1;
    return send_status_;
  }

  Result<Packet> result = Packet::FromBuffer(buffer);
  PW_CHECK_OK(result.status());

  PW_CHECK(!packets_.full(),
           "Attempted to store more than %u packets. Increase the kMaxPackets "
           "template arg to store more packets.",
           static_cast<unsigned>(packets_.size()));

  packets_.push_back(*result);
  Packet& packet = packets_.back();

  CopyPayloadToBuffer(packet);

  switch (packet.type()) {
    case pwpb::PacketType::REQUEST:
      return OkStatus();
    case pwpb::PacketType::RESPONSE:
      total_response_packets_ += 1;
      return OkStatus();
    case pwpb::PacketType::CLIENT_STREAM:
      return OkStatus();
    case pwpb::PacketType::DEPRECATED_SERVER_STREAM_END:
      PW_CRASH("Deprecated PacketType %d", static_cast<int>(packet.type()));
    case pwpb::PacketType::CLIENT_ERROR:
      PW_LOG_WARN("FakeChannelOutput received client error: %s",
                  packet.status().str());
      return OkStatus();
    case pwpb::PacketType::SERVER_ERROR:
      PW_LOG_WARN("FakeChannelOutput received server error: %s",
                  packet.status().str());
      return OkStatus();
    case pwpb::PacketType::DEPRECATED_CANCEL:
    case pwpb::PacketType::SERVER_STREAM:
    case pwpb::PacketType::CLIENT_STREAM_END:
      return OkStatus();
  }
  PW_CRASH("Unhandled PacketType %d", static_cast<int>(result.value().type()));
}

void FakeChannelOutput::CopyPayloadToBuffer(Packet& packet) {
  const ConstByteSpan& payload = packet.payload();
  if (payload.empty()) {
    return;
  }

  const size_t available_bytes = payloads_.max_size() - payloads_.size();
  PW_CHECK_UINT_GE(available_bytes,
                   payload.size(),
                   "Ran out of payload buffer space. Increase "
                   "kPayloadBufferSizeBytes (%u) or use smaller payloads.",
                   static_cast<unsigned>(payloads_.max_size()));

  const size_t start = payloads_.size();
  payloads_.resize(payloads_.size() + payload.size());
  std::memcpy(&payloads_[start], payload.data(), payload.size());
  packet.set_payload(span(&payloads_[start], payload.size()));
}

void FakeChannelOutput::LogPackets() const {
  LockGuard lock(mutex_);

  PW_LOG_INFO("%u packets have been sent through this FakeChannelOutput",
              static_cast<unsigned>(packets_.size()));

  for (unsigned i = 0; i < packets_.size(); ++i) {
    PW_LOG_INFO("[packet %u/%u]", i + 1, unsigned(packets_.size()));
    PW_LOG_INFO("        type: %u", unsigned(packets_[i].type()));
    PW_LOG_INFO("  channel_id: %u", unsigned(packets_[i].channel_id()));
    PW_LOG_INFO("  service_id: %08x", unsigned(packets_[i].service_id()));
    PW_LOG_INFO("   method_id: %08x", unsigned(packets_[i].method_id()));
    PW_LOG_INFO("     call_id: %u", unsigned(packets_[i].call_id()));
    PW_LOG_INFO("      status: %s", packets_[i].status().str());
    PW_LOG_INFO("     payload: %u B", unsigned(packets_[i].payload().size()));
  }
}

}  // namespace pw::rpc::internal::test
