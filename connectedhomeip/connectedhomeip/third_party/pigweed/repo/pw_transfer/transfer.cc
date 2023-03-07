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

#include "pw_transfer/transfer.h"

#include "pw_assert/check.h"
#include "pw_log/log.h"
#include "pw_status/try.h"
#include "pw_transfer/internal/chunk.h"

namespace pw::transfer {

void TransferService::HandleChunk(ConstByteSpan message,
                                  internal::TransferType type) {
  Result<internal::Chunk> chunk = internal::Chunk::Parse(message);
  if (!chunk.ok()) {
    PW_LOG_ERROR("Failed to decode transfer chunk: %d", chunk.status().code());
    return;
  }

  if (chunk->IsInitialChunk()) {
    uint32_t session_id =
        chunk->is_legacy() ? chunk->session_id() : GenerateNewSessionId();
    uint32_t resource_id =
        chunk->is_legacy() ? chunk->session_id() : chunk->resource_id().value();

    thread_.StartServerTransfer(type,
                                chunk->protocol_version(),
                                session_id,
                                resource_id,
                                message,
                                max_parameters_,
                                chunk_timeout_,
                                max_retries_,
                                max_lifetime_retries_);
  } else {
    thread_.ProcessServerChunk(message);
  }
}

}  // namespace pw::transfer
