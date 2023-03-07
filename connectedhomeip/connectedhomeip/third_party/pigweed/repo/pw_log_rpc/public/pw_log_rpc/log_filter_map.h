// Copyright 2021 The Pigweed Authors
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

#include "pw_bytes/span.h"
#include "pw_log_rpc/log_filter.h"
#include "pw_result/result.h"
#include "pw_span/span.h"

namespace pw::log_rpc {

// Holds an inmutable Filter map, ordered by filter ID to facilitate set up.
class FilterMap {
 public:
  explicit constexpr FilterMap(span<Filter> filters) : filters_(filters) {}

  // Not copyable nor movable.
  FilterMap(FilterMap const&) = delete;
  FilterMap& operator=(FilterMap const&) = delete;
  FilterMap(FilterMap&&) = delete;
  FilterMap& operator=(FilterMap&&) = delete;

  Result<Filter*> GetFilterFromId(ConstByteSpan id) const {
    for (auto& filter : filters_) {
      if (id.size() == filter.id().size() &&
          std::memcmp(id.data(), filter.id().data(), id.size()) == 0) {
        return &filter;
      }
    }
    return Status::NotFound();
  }

  const span<Filter>& filters() const { return filters_; }

 protected:
  const span<Filter> filters_;
};

}  // namespace pw::log_rpc
