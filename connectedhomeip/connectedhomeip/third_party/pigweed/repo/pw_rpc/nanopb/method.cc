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
#include "pw_rpc/internal/log_config.h" // PW_LOG_* macros must be first.

#include "pw_rpc/nanopb/internal/method.h"
// clang-format on

#include "pb_decode.h"
#include "pb_encode.h"
#include "pw_log/log.h"
#include "pw_rpc/internal/packet.h"

namespace pw::rpc {

namespace internal {

void NanopbMethod::CallSynchronousUnary(const CallContext& context,
                                        const Packet& request,
                                        void* request_struct,
                                        void* response_struct) const {
  if (!DecodeRequest(context, request, request_struct)) {
    rpc_lock().unlock();
    return;
  }

  NanopbServerCall responder(context.ClaimLocked(), MethodType::kUnary);
  rpc_lock().unlock();
  const Status status = function_.synchronous_unary(
      context.service(), request_struct, response_struct);
  responder.SendUnaryResponse(response_struct, status).IgnoreError();
}

void NanopbMethod::CallUnaryRequest(const CallContext& context,
                                    MethodType type,
                                    const Packet& request,
                                    void* request_struct) const {
  if (!DecodeRequest(context, request, request_struct)) {
    rpc_lock().unlock();
    return;
  }

  NanopbServerCall server_writer(context.ClaimLocked(), type);
  rpc_lock().unlock();
  function_.unary_request(context.service(), request_struct, server_writer);
}

bool NanopbMethod::DecodeRequest(const CallContext& context,
                                 const Packet& request,
                                 void* proto_struct) const {
  if (serde_.DecodeRequest(request.payload(), proto_struct)) {
    return true;
  }

  // The channel is known to exist. It was found when the request was processed
  // and the lock has been held since, so GetInternalChannel cannot fail.
  static_cast<internal::Channel*>(
      context.server().GetInternalChannel(context.channel_id()))
      ->Send(Packet::ServerError(request, Status::DataLoss()))
      .IgnoreError();
  PW_LOG_WARN("Nanopb failed to decode request payload from channel %u",
              unsigned(context.channel_id()));
  return false;
}

}  // namespace internal
}  // namespace pw::rpc
