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
#include "pw_rpc/internal/log_config.h" // PW_LOG_* macros must be first.

#include "pw_rpc/raw/client_testing.h"
// clang-format on

#include "pw_assert/check.h"
#include "pw_log/log.h"
#include "pw_rpc/client.h"
#include "pw_rpc/internal/lock.h"

namespace pw::rpc {

void FakeServer::CheckProcessPacket(internal::pwpb::PacketType type,
                                    uint32_t service_id,
                                    uint32_t method_id,
                                    std::optional<uint32_t> call_id,
                                    ConstByteSpan payload,
                                    Status status) const {
  if (Status process_packet_status =
          ProcessPacket(type, service_id, method_id, call_id, payload, status);
      !process_packet_status.ok()) {
    PW_LOG_CRITICAL("Failed to process packet in pw::rpc::FakeServer");
    PW_LOG_CRITICAL(
        "Packet contents\ntype: %u\nchannel_id: %u\nservice_id: %08x\n"
        "method_id: %08x\npayload: %u bytes\nstatus: %s",
        static_cast<unsigned>(type),
        static_cast<unsigned>(channel_id_),
        static_cast<unsigned>(service_id),
        static_cast<unsigned>(method_id),
        static_cast<unsigned>(payload.size()),
        status.str());
    PW_CHECK_OK(process_packet_status);
  }
}

Status FakeServer::ProcessPacket(internal::pwpb::PacketType type,
                                 uint32_t service_id,
                                 uint32_t method_id,
                                 std::optional<uint32_t> call_id,
                                 ConstByteSpan payload,
                                 Status status) const {
  if (!call_id.has_value()) {
    internal::LockGuard lock(output_.mutex_);
    auto view = internal::test::PacketsView(
        output_.packets(),
        internal::test::PacketFilter(internal::pwpb::PacketType::REQUEST,
                                     internal::pwpb::PacketType::RESPONSE,
                                     channel_id_,
                                     service_id,
                                     method_id));

    // Re-use the call ID of the most recent packet for this RPC.
    if (!view.empty()) {
      call_id = view.back().call_id();
    }
  }

  auto packet_encoding_result =
      internal::Packet(
          type, channel_id_, service_id, method_id, *call_id, payload, status)
          .Encode(packet_buffer_);
  PW_CHECK_OK(packet_encoding_result.status());
  return client_.ProcessPacket(*packet_encoding_result);
}

}  // namespace pw::rpc
