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

#include "pw_rpc/raw/server_reader_writer.h"
#include "pw_span/span.h"
#include "pw_status/status.h"
#include "pw_thread/config.h"
#include "pw_thread/thread_info.h"
#include "pw_thread_protos/thread.pwpb.h"
#include "pw_thread_protos/thread_snapshot_service.pwpb.h"
#include "pw_thread_protos/thread_snapshot_service.raw_rpc.pb.h"

namespace pw::thread::proto {

Status ProtoEncodeThreadInfo(proto::SnapshotThreadInfo::StreamEncoder& encoder,
                             const ThreadInfo& thread_info);

// Calculates encoded buffer size based on code gen constants.
constexpr size_t RequiredServiceBufferSize(
    size_t num_threads = PW_THREAD_MAXIMUM_THREADS) {
  constexpr size_t kSizeOfResponse =
      proto::SnapshotThreadInfo::kMaxEncodedSizeBytes +
      Thread::kMaxEncodedSizeBytes;
  return kSizeOfResponse * num_threads;
}

// The ThreadSnapshotService will return peak stack usage across running
// threads when requested by GetPeak().
//
// Parameter encode_buffer: buffer where thread information is encoded. Size
// depends on RequiredBufferSize().
//
// Parameter thread_proto_indices: array keeping track of thread boundaries in
// the encode buffer. The service uses these indices to send response data out
// in bundles.
//
// Parameter num_bundled_threads: constant describing number of threads per
// bundle in response.
class ThreadSnapshotService
    : public pw_rpc::raw::ThreadSnapshotService::Service<
          ThreadSnapshotService> {
 public:
  constexpr ThreadSnapshotService(
      span<std::byte> encode_buffer,
      Vector<size_t>& thread_proto_indices,
      size_t num_bundled_threads = PW_THREAD_NUM_BUNDLED_THREADS)
      : encode_buffer_(encode_buffer),
        thread_proto_indices_(thread_proto_indices),
        num_bundled_threads_(num_bundled_threads) {}
  void GetPeakStackUsage(ConstByteSpan request, rpc::RawServerWriter& response);

 private:
  span<std::byte> encode_buffer_;
  Vector<size_t>& thread_proto_indices_;
  size_t num_bundled_threads_;
};

// A ThreadSnapshotService that allocates required buffers based on the
// number of running threads on a device.
template <size_t kNumThreads = PW_THREAD_MAXIMUM_THREADS>
class ThreadSnapshotServiceBuffer : public ThreadSnapshotService {
 public:
  ThreadSnapshotServiceBuffer()
      : ThreadSnapshotService(encode_buffer_, thread_proto_indices_) {}

 private:
  std::array<std::byte, RequiredServiceBufferSize(kNumThreads)> encode_buffer_;
  // + 1 is needed to account for extra index that comes with the first
  // submessage start or the last submessage end.
  Vector<size_t, kNumThreads + 1> thread_proto_indices_;
};

}  // namespace pw::thread::proto
