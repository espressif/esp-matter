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

#include "pw_transfer/client.h"

#include "pw_log/log.h"

namespace pw::transfer {

Status Client::Read(uint32_t resource_id,
                    stream::Writer& output,
                    CompletionFunc&& on_completion,
                    chrono::SystemClock::duration timeout,
                    ProtocolVersion protocol_version) {
  if (on_completion == nullptr ||
      protocol_version == ProtocolVersion::kUnknown) {
    return Status::InvalidArgument();
  }

  if (!has_read_stream_) {
    rpc::RawClientReaderWriter read_stream = client_.Read(
        [this](ConstByteSpan chunk) {
          transfer_thread_.ProcessClientChunk(chunk);
        },
        [this](Status status) {
          OnRpcError(status, internal::TransferType::kReceive);
        });
    transfer_thread_.SetClientReadStream(read_stream);
    has_read_stream_ = true;
  }

  transfer_thread_.StartClientTransfer(internal::TransferType::kReceive,
                                       protocol_version,
                                       resource_id,
                                       &output,
                                       max_parameters_,
                                       std::move(on_completion),
                                       timeout,
                                       max_retries_,
                                       max_lifetime_retries_);
  return OkStatus();
}

Status Client::Write(uint32_t resource_id,
                     stream::Reader& input,
                     CompletionFunc&& on_completion,
                     chrono::SystemClock::duration timeout,
                     ProtocolVersion protocol_version) {
  if (on_completion == nullptr ||
      protocol_version == ProtocolVersion::kUnknown) {
    return Status::InvalidArgument();
  }

  if (!has_write_stream_) {
    rpc::RawClientReaderWriter write_stream = client_.Write(
        [this](ConstByteSpan chunk) {
          transfer_thread_.ProcessClientChunk(chunk);
        },
        [this](Status status) {
          OnRpcError(status, internal::TransferType::kTransmit);
        });
    transfer_thread_.SetClientWriteStream(write_stream);
    has_write_stream_ = true;
  }

  transfer_thread_.StartClientTransfer(internal::TransferType::kTransmit,
                                       protocol_version,
                                       resource_id,
                                       &input,
                                       max_parameters_,
                                       std::move(on_completion),
                                       timeout,
                                       max_retries_,
                                       max_lifetime_retries_);

  return OkStatus();
}

void Client::CancelTransfer(uint32_t resource_id) {
  transfer_thread_.EndClientTransfer(
      resource_id, Status::Cancelled(), /*send_status_chunk=*/true);
}

void Client::OnRpcError(Status status, internal::TransferType type) {
  bool is_write_error = type == internal::TransferType::kTransmit;

  PW_LOG_ERROR("Client %s stream terminated with status %d",
               is_write_error ? "Write()" : "Read()",
               status.code());

  if (is_write_error) {
    has_write_stream_ = false;
  } else {
    has_read_stream_ = false;
  }
}

}  // namespace pw::transfer
