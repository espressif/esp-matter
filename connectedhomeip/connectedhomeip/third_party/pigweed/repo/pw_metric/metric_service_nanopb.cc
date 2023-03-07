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

#include "pw_metric/metric_service_nanopb.h"

#include <cstring>

#include "pw_assert/check.h"
#include "pw_containers/vector.h"
#include "pw_metric/metric.h"
#include "pw_metric_private/metric_walker.h"
#include "pw_preprocessor/util.h"
#include "pw_span/span.h"

namespace pw::metric {
namespace {

class NanopbMetricWriter : public virtual internal::MetricWriter {
 public:
  NanopbMetricWriter(
      MetricService::ServerWriter<pw_metric_proto_MetricResponse>&
          response_writer)
      : response_(pw_metric_proto_MetricResponse_init_zero),
        response_writer_(response_writer) {}

  // TODO(keir): Figure out a pw_rpc mechanism to fill a streaming packet based
  // on transport MTU, rather than having this as a static knob. For example,
  // some transports may be able to fit 30 metrics; others, only 5.
  Status Write(const Metric& metric, const Vector<Token>& path) override {
    // Nanopb doesn't offer an easy way to do bounds checking, so use span's
    // type deduction magic to figure out the max size.
    span<pw_metric_proto_Metric> metrics(response_.metrics);
    PW_CHECK_INT_LT(response_.metrics_count, metrics.size());

    // Grab the next available Metric slot to write to in the response.
    pw_metric_proto_Metric& proto_metric =
        response_.metrics[response_.metrics_count];

    // Copy the path.
    span<Token> proto_path(proto_metric.token_path);
    PW_CHECK_INT_LE(path.size(), proto_path.size());
    std::copy(path.begin(), path.end(), proto_path.begin());
    proto_metric.token_path_count = path.size();

    // Copy the metric value.
    if (metric.is_float()) {
      proto_metric.value.as_float = metric.as_float();
      proto_metric.which_value = pw_metric_proto_Metric_as_float_tag;
    } else {
      proto_metric.value.as_int = metric.as_int();
      proto_metric.which_value = pw_metric_proto_Metric_as_int_tag;
    }

    // Move write head to the next slot.
    response_.metrics_count++;

    // If the metric response object is full, send the response and reset.
    // TODO(keir): Support runtime batch sizes < max proto size.
    if (response_.metrics_count == metrics.size()) {
      Flush();
    }

    return OkStatus();
  }

  void Flush() {
    if (response_.metrics_count) {
      response_writer_.Write(response_)
          .IgnoreError();  // TODO(b/242598609): Handle Status properly
      response_ = pw_metric_proto_MetricResponse_init_zero;
    }
  }

 private:
  pw_metric_proto_MetricResponse response_;
  // This RPC stream writer handle must be valid for the metric writer lifetime.
  MetricService::ServerWriter<pw_metric_proto_MetricResponse>& response_writer_;
};

}  // namespace

void MetricService::Get(
    const pw_metric_proto_MetricRequest& /* request */,
    ServerWriter<pw_metric_proto_MetricResponse>& response) {
  // For now, ignore the request and just stream all the metrics back.
  NanopbMetricWriter writer(response);
  internal::MetricWalker walker(writer);

  // This will stream all the metrics in the span of this Get() method call.
  // This will have the effect of blocking the RPC thread until all the metrics
  // are sent. That is likely to cause problems if there are many metrics, or
  // if other RPCs are higher priority and should complete first.
  //
  // In the future, this should be replaced with an optional async solution
  // that puts the application in control of when the response batches are sent.
  walker.Walk(metrics_).IgnoreError();
  walker.Walk(groups_).IgnoreError();
  writer.Flush();
}

}  // namespace pw::metric
