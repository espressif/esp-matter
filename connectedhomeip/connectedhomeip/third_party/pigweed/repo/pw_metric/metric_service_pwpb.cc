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

#include "pw_metric/metric_service_pwpb.h"

#include <cstring>

#include "pw_assert/check.h"
#include "pw_containers/vector.h"
#include "pw_metric/metric.h"
#include "pw_metric_private/metric_walker.h"
#include "pw_metric_proto/metric_service.pwpb.h"
#include "pw_preprocessor/util.h"
#include "pw_rpc/raw/server_reader_writer.h"
#include "pw_span/span.h"
#include "pw_status/status.h"
#include "pw_status/try.h"

namespace pw::metric {

// TODO(amontanez): Make this follow the metric_service.options configuration.
constexpr size_t kMaxNumPackedEntries = 3;

namespace {

class PwpbMetricWriter : public virtual internal::MetricWriter {
 public:
  PwpbMetricWriter(span<std::byte> response,
                   rpc::RawServerWriter& response_writer)
      : response_(response),
        response_writer_(response_writer),
        encoder_(response) {}

  // TODO(keir): Figure out a pw_rpc mechanism to fill a streaming packet based
  // on transport MTU, rather than having this as a static knob. For example,
  // some transports may be able to fit 30 metrics; others, only 5.
  Status Write(const Metric& metric, const Vector<Token>& path) override {
    {  // Scope to control proto_encoder lifetime.

      // Grab the next available Metric slot to write to in the response.
      proto::pwpb::Metric::StreamEncoder proto_encoder =
          encoder_.GetMetricsEncoder();
      PW_TRY(proto_encoder.WriteTokenPath(path));
      // Encode the metric value.
      if (metric.is_float()) {
        PW_TRY(proto_encoder.WriteAsFloat(metric.as_float()));
      } else {
        PW_TRY(proto_encoder.WriteAsInt(metric.as_int()));
      }

      metrics_count++;
    }

    if (metrics_count == kMaxNumPackedEntries) {
      return Flush();
    }
    return OkStatus();
  }

  Status Flush() {
    Status status;
    if (metrics_count) {
      status = response_writer_.Write(encoder_);
      // Different way to clear MemoryEncoder. Copy constructor is disabled
      // for memory encoder, and there is no "clear()" method.
      encoder_.~MemoryEncoder();
      new (&encoder_) proto::pwpb::MetricRequest::MemoryEncoder(response_);
      metrics_count = 0;
    }
    return status;
  }

 private:
  span<std::byte> response_;
  // This RPC stream writer handle must be valid for the metric writer
  // lifetime.
  rpc::RawServerWriter& response_writer_;
  proto::pwpb::MetricRequest::MemoryEncoder encoder_;
  size_t metrics_count = 0;
};
}  // namespace

void MetricService::Get(ConstByteSpan /*request*/,
                        rpc::RawServerWriter& raw_response) {
  // For now, ignore the request and just stream all the metrics back.
  // TODO(amontanez): Make this follow the metric_service.options configuration.
  constexpr size_t kSizeOfOneMetric =
      pw::metric::proto::pwpb::MetricResponse::kMaxEncodedSizeBytes +
      pw::metric::proto::pwpb::Metric::kMaxEncodedSizeBytes;
  constexpr size_t kEncodeBufferSize = kMaxNumPackedEntries * kSizeOfOneMetric;

  std::array<std::byte, kEncodeBufferSize> encode_buffer;

  PwpbMetricWriter writer(encode_buffer, raw_response);
  internal::MetricWalker walker(writer);

  // This will stream all the metrics in the span of this Get() method call.
  // This will have the effect of blocking the RPC thread until all the metrics
  // are sent. That is likely to cause problems if there are many metrics, or
  // if other RPCs are higher priority and should complete first.
  //
  // In the future, this should be replaced with an optional async solution
  // that puts the application in control of when the response batches are sent.

  // Propagate status through walker.
  Status status;
  status.Update(walker.Walk(metrics_));
  status.Update(walker.Walk(groups_));
  status.Update(writer.Flush());
  raw_response.Finish(status).IgnoreError();
}
}  // namespace pw::metric
