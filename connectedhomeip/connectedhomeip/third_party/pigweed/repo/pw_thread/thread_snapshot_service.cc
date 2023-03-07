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

#include "pw_thread/thread_snapshot_service.h"

#include "pw_containers/vector.h"
#include "pw_log/log.h"
#include "pw_protobuf/decoder.h"
#include "pw_rpc/raw/server_reader_writer.h"
#include "pw_span/span.h"
#include "pw_status/status.h"
#include "pw_status/try.h"
#include "pw_thread/thread_info.h"
#include "pw_thread/thread_iteration.h"
#include "pw_thread_private/thread_snapshot_service.h"
#include "pw_thread_protos/thread.pwpb.h"
#include "pw_thread_protos/thread_snapshot_service.pwpb.h"

namespace pw::thread::proto {

Status ProtoEncodeThreadInfo(SnapshotThreadInfo::StreamEncoder& encoder,
                             const ThreadInfo& thread_info) {
  // Grab the next available Thread slot to write to in the response.
  Thread::StreamEncoder proto_encoder = encoder.GetThreadsEncoder();
  if (thread_info.thread_name().has_value()) {
    PW_TRY(proto_encoder.WriteName(thread_info.thread_name().value()));
  } else {
    // Name is necessary to identify thread.
    return Status::FailedPrecondition();
  }
  if (thread_info.stack_low_addr().has_value()) {
    PW_TRY(proto_encoder.WriteStackEndPointer(
        thread_info.stack_low_addr().value()));
  }
  if (thread_info.stack_high_addr().has_value()) {
    PW_TRY(proto_encoder.WriteStackStartPointer(
        thread_info.stack_high_addr().value()));
  } else {
    // Need stack start pointer to contextualize estimated peak.
    return Status::FailedPrecondition();
  }
  if (thread_info.stack_pointer().has_value()) {
    PW_TRY(
        proto_encoder.WriteStackPointer(thread_info.stack_pointer().value()));
  }

  if (thread_info.stack_peak_addr().has_value()) {
    PW_TRY(proto_encoder.WriteStackPointerEstPeak(
        thread_info.stack_peak_addr().value()));
  } else {
    // Peak stack usage reporting is not supported.
    return Status::Unimplemented();
  }

  return proto_encoder.status();
}

void ErrorLog(Status status) {
  if (status == Status::Unimplemented()) {
    PW_LOG_ERROR(
        "Peak stack usage reporting not supported by your current OS or "
        "configuration.");
  } else if (status == Status::FailedPrecondition()) {
    PW_LOG_ERROR("Thread missing information needed by service.");
  } else if (status == Status::ResourceExhausted()) {
    PW_LOG_ERROR("Buffer capacity limit exceeded.");
  } else if (status != OkStatus()) {
    PW_LOG_ERROR(
        "Failure with error code %d, RPC service was unable to capture thread "
        "information",
        status.code());
  }
}

Status DecodeThreadName(ConstByteSpan serialized_path,
                        ConstByteSpan& thread_name) {
  protobuf::Decoder decoder(serialized_path);
  Status status;
  while (decoder.Next().ok()) {
    switch (decoder.FieldNumber()) {
      case static_cast<uint32_t>(Thread::Fields::NAME): {
        status.Update(decoder.ReadBytes(&thread_name));
      }
    }
  }
  return status;
}

void ThreadSnapshotService::GetPeakStackUsage(
    ConstByteSpan request, rpc::RawServerWriter& response_writer) {
  // For now, ignore the request and just stream all the thread information
  // back.
  struct IterationInfo {
    SnapshotThreadInfo::MemoryEncoder encoder;
    Status status;
    ConstByteSpan name;

    // For sending out data by chunks.
    Vector<size_t>& thread_proto_indices;
  };

  ConstByteSpan name_request;
  if (!request.empty()) {
    if (const auto status = DecodeThreadName(request, name_request);
        !status.ok()) {
      PW_LOG_ERROR("Service unable to decode thread name with error code %d",
                   status.code());
    }
  }

  IterationInfo iteration_info{
      SnapshotThreadInfo::MemoryEncoder(encode_buffer_),
      OkStatus(),
      name_request,
      thread_proto_indices_};

  iteration_info.thread_proto_indices.clear();
  iteration_info.thread_proto_indices.push_back(iteration_info.encoder.size());

  auto cb = [&iteration_info](const ThreadInfo& thread_info) {
    if (!iteration_info.name.empty() && thread_info.thread_name().has_value()) {
      if (std::equal(thread_info.thread_name().value().begin(),
                     thread_info.thread_name().value().end(),
                     iteration_info.name.begin())) {
        iteration_info.status.Update(
            ProtoEncodeThreadInfo(iteration_info.encoder, thread_info));
        iteration_info.thread_proto_indices.push_back(
            iteration_info.encoder.size());
        return false;
      }
    } else {
      iteration_info.status.Update(
          ProtoEncodeThreadInfo(iteration_info.encoder, thread_info));
      iteration_info.thread_proto_indices.push_back(
          iteration_info.encoder.size());
    }
    return iteration_info.status.ok();
  };
  if (const auto status = ForEachThread(cb); !status.ok()) {
    PW_LOG_ERROR("Failed to capture thread information, error %d",
                 status.code());
  }

  // This logging action is external to thread iteration because it is
  // unsafe to log within ForEachThread() when the scheduler is disabled.
  ErrorLog(iteration_info.status);

  Status status;
  if (iteration_info.encoder.size() && iteration_info.status.ok()) {
    // Must subtract 1 because the last boundary index of thread_proto_indices
    // is the end of the last submessage, and NOT the start of another.
    size_t last_start_index = iteration_info.thread_proto_indices.size() - 1;
    for (size_t i = 0; i < last_start_index; i += num_bundled_threads_) {
      const size_t num_threads =
          std::min(num_bundled_threads_, last_start_index - i);

      // Sending out a bundle of threads at a time.
      const size_t bundle_size =
          iteration_info.thread_proto_indices[i + num_threads] -
          iteration_info.thread_proto_indices[i];

      ConstByteSpan thread =
          ConstByteSpan(iteration_info.encoder.data() +
                            iteration_info.thread_proto_indices[i],
                        bundle_size);

      if (bundle_size) {
        status.Update(response_writer.Write(thread));
      }
      if (!status.ok()) {
        PW_LOG_ERROR(
            "Failed to send response with error code %d, packet may be too "
            "large to send",
            status.code());
      }
    }
  }

  if (response_writer.Finish(status) != OkStatus()) {
    PW_LOG_ERROR(
        "Failed to close stream for GetPeakStackUsage() with error code %d",
        status.code());
  }
}

}  // namespace pw::thread::proto
