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

#include <cstddef>
#include <cstring>

#include "pw_assert/assert.h"
#include "pw_bytes/span.h"
#include "pw_containers/vector.h"
#include "pw_log/proto/log.pwpb.h"
#include "pw_log_rpc/internal/config.h"
#include "pw_span/span.h"
#include "pw_status/status.h"

namespace pw::log_rpc {

// A Filter is a collection of rules used to check if a log entry can be kept
// or dropped wherever the filter is placed in the log path.
class Filter {
 public:
  struct Rule {
    // Action to perform if the rule is met.
    enum class Action {
      kInactive = 0,  // Ignore this rule.
      kKeep = 1,
      kDrop = 2,
    };
    Action action = Action::kInactive;

    // Checks if the log level is greater or equal to this value when it
    // does not equal NOT_SET.
    log::pwpb::FilterRule::Level level_greater_than_or_equal =
        log::pwpb::FilterRule::Level::ANY_LEVEL;

    // Checks if the log entry has any flag is set when it doesn't equal 0.
    uint32_t any_flags_set = 0;

    // Checks if the log entry module equals this value when not empty.
    Vector<std::byte, cfg::kMaxModuleNameBytes> module_equals{};

    // Checks if the log entry thread equals this value when not empty.
    Vector<std::byte, cfg::kMaxThreadNameBytes> thread_equals{};
  };

  Filter(span<const std::byte> id, span<Rule> rules) : rules_(rules) {
    PW_ASSERT(!id.empty());
    id_.assign(id.begin(), id.end());
  }

  // Not copyable.
  Filter(const Filter&) = delete;
  Filter& operator=(const Filter&) = delete;

  ConstByteSpan id() const { return ConstByteSpan(id_.data(), id_.size()); }
  span<const Rule> rules() const { return rules_; }

  // Verifies a log entry against the filter's rules in the order they were
  // provided, stopping at the first rule that matches.
  // Returns true when the log should be dropped, false otherwise. Defaults to
  // false if there are no rules, or no rules were matched.
  bool ShouldDropLog(ConstByteSpan entry) const;

  // Decodes and updates the filter's rules given a buffer with a proto-encoded
  // log::Filter message. If there are more rules than this filter can hold, the
  // extra rules are discarded.
  //
  // Return values:
  // OK - rules were updated successfully.
  // FAILED_PRECONDITION - the provided buffer is empty.
  // Forwarded errors from protobuff::decoder.
  Status UpdateRulesFromProto(ConstByteSpan buffer);

 private:
  Vector<std::byte, cfg::kMaxFilterIdBytes> id_;
  span<Rule> rules_;
};

}  // namespace pw::log_rpc
