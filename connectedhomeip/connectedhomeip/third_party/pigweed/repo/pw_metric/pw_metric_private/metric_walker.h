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

#include "pw_assert/check.h"
#include "pw_containers/intrusive_list.h"
#include "pw_containers/vector.h"
#include "pw_metric/metric.h"
#include "pw_status/status.h"
#include "pw_tokenizer/tokenize.h"

namespace pw::metric::internal {

class MetricWriter {
 public:
  virtual ~MetricWriter() = default;
  virtual Status Write(const Metric& metric, const Vector<Token>& path) = 0;
};

// Walk a metric tree recursively; passing metrics with their path (names) to a
// MetricWriter that can consume them.
class MetricWalker {
 public:
  MetricWalker(MetricWriter& writer) : writer_(writer) {}

  Status Walk(const IntrusiveList<Metric>& metrics) {
    for (const auto& m : metrics) {
      ScopedName scoped_name(m.name(), *this);
      PW_TRY(writer_.Write(m, path_));
    }
    return OkStatus();
  }

  Status Walk(const IntrusiveList<Group>& groups) {
    for (const auto& g : groups) {
      PW_TRY(Walk(g));
    }
    return OkStatus();
  }

  Status Walk(const Group& group) {
    ScopedName scoped_name(group.name(), *this);
    PW_TRY(Walk(group.children()));
    PW_TRY(Walk(group.metrics()));
    return OkStatus();
  }

 private:
  // Exists to safely push/pop parent groups from the explicit stack.
  struct ScopedName {
    ScopedName(Token name, MetricWalker& rhs) : walker(rhs) {
      // Metrics are too deep; bump path_ capacity.
      PW_ASSERT(walker.path_.size() < walker.path_.capacity());
      walker.path_.push_back(name);
    }
    ~ScopedName() { walker.path_.pop_back(); }
    MetricWalker& walker;
  };

  Vector<Token, /*capacity=*/4> path_;
  MetricWriter& writer_;
};

}  // namespace pw::metric::internal
