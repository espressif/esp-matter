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

// clang-format off
#include "pw_rpc/internal/log_config.h"  // PW_LOG_* macros must be first.

#include "pw_rpc/client.h"
// clang-format on

#include "pw_log/log.h"
#include "pw_rpc/internal/client_call.h"
#include "pw_rpc/internal/packet.h"
#include "pw_status/try.h"

namespace pw::rpc {
namespace {

using internal::Packet;
using internal::pwpb::PacketType;

}  // namespace

Status Client::ProcessPacket(ConstByteSpan data) {
  PW_TRY_ASSIGN(Packet packet, Endpoint::ProcessPacket(data, Packet::kClient));

  // Find an existing call for this RPC, if any.
  internal::rpc_lock().lock();
  internal::ClientCall* call =
      static_cast<internal::ClientCall*>(FindCall(packet));

  internal::Channel* channel = GetInternalChannel(packet.channel_id());

  if (channel == nullptr) {
    internal::rpc_lock().unlock();
    PW_LOG_WARN("RPC client received a packet for an unregistered channel");
    return Status::Unavailable();
  }

  if (call == nullptr) {
    // The call for the packet does not exist. If the packet is a server stream
    // message, notify the server so that it can kill the stream. Otherwise,
    // silently drop the packet (as it would terminate the RPC anyway).
    if (packet.type() == PacketType::SERVER_STREAM) {
      channel->Send(Packet::ClientError(packet, Status::FailedPrecondition()))
          .IgnoreError();
      PW_LOG_WARN("RPC client received stream message for an unknown call");
    }
    internal::rpc_lock().unlock();
    return OkStatus();  // OK since the packet was handled
  }

  switch (packet.type()) {
    case PacketType::RESPONSE:
      // RPCs without a server stream include a payload with the final packet.
      if (call->has_server_stream()) {
        static_cast<internal::StreamResponseClientCall&>(*call).HandleCompleted(
            packet.status());
      } else {
        static_cast<internal::UnaryResponseClientCall&>(*call).HandleCompleted(
            packet.payload(), packet.status());
      }
      break;
    case PacketType::SERVER_ERROR:
      call->HandleError(packet.status());
      break;
    case PacketType::SERVER_STREAM:
      if (call->has_server_stream()) {
        call->HandlePayload(packet.payload());
      } else {
        // Report the error to the server so it can abort the RPC.
        channel->Send(Packet::ClientError(packet, Status::InvalidArgument()))
            .IgnoreError();  // Errors are logged in Channel::Send.
        call->HandleError(Status::InvalidArgument());
        PW_LOG_DEBUG("Received SERVER_STREAM for RPC without a server stream");
      }
      break;

    case PacketType::REQUEST:
    case PacketType::CLIENT_STREAM:
    case PacketType::DEPRECATED_SERVER_STREAM_END:
    case PacketType::CLIENT_ERROR:
    case PacketType::DEPRECATED_CANCEL:
    case PacketType::CLIENT_STREAM_END:
    default:
      internal::rpc_lock().unlock();
      PW_LOG_WARN("pw_rpc client unable to handle packet of type %u",
                  static_cast<unsigned>(packet.type()));
  }

  return OkStatus();  // OK since the packet was handled
}

}  // namespace pw::rpc
