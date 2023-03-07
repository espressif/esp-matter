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

#include "pw_assert/check.h"
#include "pw_bloat/bloat_this_binary.h"
#include "pw_log/log.h"
#include "pw_metric/metric.h"

PW_METRIC_GROUP(group_foo, "some_group");
PW_METRIC(group_foo, metric_x, "some_metric", 14u);

int volatile* unoptimizable;

int main() {
  pw::bloat::BloatThisBinary();

  if (*unoptimizable) {
    metric_x.Increment();
  }
  metric_x.Increment();

  // Ensure log and assert aren't optimized out.
  PW_CHECK_INT_GE(*unoptimizable, 0, "Ensure this CHECK logic stays");
  PW_LOG_INFO("Ensure logs are pulled in: %d", *unoptimizable);

  // Ensure metric_x isn't optimized out.
  PW_LOG_INFO("metric_x: %d", static_cast<int>(metric_x.value()));

  // Trigger pulling in the dump code. Dump twice to cover cost in more_metrics.
  group_foo.Dump();
  group_foo.Dump();

  return *unoptimizable;
}
