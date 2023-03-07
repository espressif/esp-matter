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

#define PW_LOG_MODULE_NAME "TRN"

#include "pw_transfer/internal/server_context.h"

#include "pw_assert/check.h"
#include "pw_log/log.h"
#include "pw_status/try.h"
#include "pw_transfer/internal/chunk.h"
#include "pw_transfer/transfer.pwpb.h"
#include "pw_varint/varint.h"

namespace pw::transfer::internal {

Status ServerContext::FinalCleanup(const Status status) {
  PW_DCHECK(active());

  // If no handler is set, then the Prepare call failed. Nothing to do.
  if (handler_ == nullptr) {
    return OkStatus();
  }

  Handler& handler = *handler_;
  handler_ = nullptr;

  if (type() == TransferType::kTransmit) {
    handler.FinalizeRead(status);
    return OkStatus();
  }

  if (Status finalized = handler.FinalizeWrite(status); !finalized.ok()) {
    PW_LOG_ERROR(
        "FinalizeWrite() for transfer %u failed with status %u; aborting with "
        "DATA_LOSS",
        static_cast<unsigned>(handler.id()),
        static_cast<int>(finalized.code()));
    return Status::DataLoss();
  }

  return OkStatus();
}

}  // namespace pw::transfer::internal
