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
#pragma once

#include <cstring>

#include "pw_log/log.h"
#include "pw_metric/metric.h"
#include "pw_metric_proto/metric_service.rpc.pb.h"
#include "pw_span/span.h"

namespace pw::metric {

// The MetricService will send metrics when requested by Get(). For now, each
// Get() request results in a stream of responses, containing the metrics from
// the supplied list of groups and metrics. This includes recursive traversal
// of subgroups. In the future, filtering will be supported.
//
// An important limitation of the current implementation is that the Get()
// method is blocking, and sends all metrics at once (though batched). In the
// future, we may switch to offering an async version where the Get() method
// returns immediately, and someone else is responsible for pumping the queue.
class MetricService final
    : public proto::pw_rpc::nanopb::MetricService::Service<MetricService> {
 public:
  MetricService(const IntrusiveList<Metric>& metrics,
                const IntrusiveList<Group>& groups)
      : metrics_(metrics), groups_(groups) {}

  void Get(const pw_metric_proto_MetricRequest& request,
           ServerWriter<pw_metric_proto_MetricResponse>& response);

 private:
  const IntrusiveList<Metric>& metrics_;
  const IntrusiveList<Group>& groups_;
};

}  // namespace pw::metric
