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

#include "pw_rpc/internal/endpoint.h"
// clang-format on

#include "pw_log/log.h"
#include "pw_rpc/internal/lock.h"
#include "pw_toolchain/no_destructor.h"

namespace pw::rpc::internal {

RpcLock& rpc_lock() {
  static NoDestructor<RpcLock> lock;
  return *lock;
}

Endpoint::~Endpoint() {
  // Since the calls remove themselves from the Endpoint in
  // CloseAndSendResponse(), close responders until no responders remain.
  while (!calls_.empty()) {
    calls_.front().CloseAndSendResponse(OkStatus()).IgnoreError();
  }
}

Result<Packet> Endpoint::ProcessPacket(span<const std::byte> data,
                                       Packet::Destination destination) {
  Result<Packet> result = Packet::FromBuffer(data);

  if (!result.ok()) {
    PW_LOG_WARN("Failed to decode pw_rpc packet");
    return Status::DataLoss();
  }

  Packet& packet = *result;

  if (packet.channel_id() == Channel::kUnassignedChannelId ||
      packet.service_id() == 0 || packet.method_id() == 0) {
    PW_LOG_WARN("Received malformed pw_rpc packet");
    return Status::DataLoss();
  }

  if (packet.destination() != destination) {
    return Status::InvalidArgument();
  }

  return result;
}

void Endpoint::RegisterCall(Call& call) {
  Call* const existing_call = FindCallById(
      call.channel_id_locked(), call.service_id(), call.method_id(), call.id());

  calls_.push_front(call);

  if (existing_call != nullptr) {
    // TODO(b/234876851): Ensure call object is locked when calling callback.
    //     For on_error, could potentially move the callback and call it after
    //     the lock is released.
    existing_call->HandleError(Status::Cancelled());
    rpc_lock().lock();
  }
}

Call* Endpoint::FindCallById(uint32_t channel_id,
                             uint32_t service_id,
                             uint32_t method_id,
                             uint32_t call_id) {
  for (Call& call : calls_) {
    if (channel_id == call.channel_id_locked() &&
        service_id == call.service_id() && method_id == call.method_id()) {
      if (call_id == call.id() || call_id == kOpenCallId) {
        return &call;
      }
      if (call.id() == kOpenCallId) {
        // Calls with ID of `kOpenCallId` were unrequested, and
        // are updated to have the call ID of the first matching request.
        call.set_id(call_id);
        return &call;
      }
    }
  }
  return nullptr;
}

Status Endpoint::CloseChannel(uint32_t channel_id) {
  LockGuard lock(rpc_lock());

  Channel* channel = channels_.Get(channel_id);
  if (channel == nullptr) {
    return Status::NotFound();
  }
  channel->Close();

  // Close pending calls on the channel that's going away.
  AbortCalls(AbortIdType::kChannel, channel_id);
  return OkStatus();
}

void Endpoint::AbortCalls(AbortIdType type, uint32_t id) {
  auto previous = calls_.before_begin();
  auto current = calls_.begin();

  while (current != calls_.end()) {
    if (id == (type == AbortIdType::kChannel ? current->channel_id_locked()
                                             : current->service_id())) {
      current->Abort();
      current = calls_.erase_after(previous);  // previous stays the same
    } else {
      previous = current;
      ++current;
    }
  }
}

}  // namespace pw::rpc::internal
