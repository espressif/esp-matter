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

#include "pw_containers/intrusive_list.h"
#include "pw_metric/metric.h"
#include "pw_tokenizer/tokenize.h"

namespace pw::metric {

// TODO(keir): Add protection to IntrusiveList to detect the unitialized case,
// which can happen with the global static constructors in the -O0 case.
extern IntrusiveList<Group> global_groups;
extern IntrusiveList<Metric> global_metrics;

// Define a metric that is registered in pw::metric::global_metrics.
//
// This is useful for cases where uncoordinated instrumentation with metrics is
// needed; for example, when instrumenting legacy code where plumbing a metric
// group around by dependency injection is infeasible.
#define PW_METRIC_GLOBAL(variable_name, metric_name, init)                    \
  static constexpr uint32_t variable_name##_token =                           \
      PW_TOKENIZE_STRING_DOMAIN("metrics", #metric_name);                     \
  ::pw::metric::TypedMetric<_PW_METRIC_FLOAT_OR_UINT32(init)> variable_name = \
      {variable_name##_token, init, ::pw::metric::global_metrics}

// Define a group that is registered in pw::metric::global_groups.
#define PW_METRIC_GROUP_GLOBAL(variable_name, group_name)     \
  static constexpr uint32_t variable_name##_token =           \
      PW_TOKENIZE_STRING_DOMAIN("metrics", #group_name);      \
  ::pw::metric::Group variable_name = {variable_name##_token, \
                                       ::pw::metric::global_groups}

}  // namespace pw::metric
