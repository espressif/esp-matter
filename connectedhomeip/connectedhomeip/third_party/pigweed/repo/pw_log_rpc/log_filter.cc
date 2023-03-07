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

#include "pw_log_rpc/log_filter.h"

#include "pw_log/levels.h"
#include "pw_protobuf/decoder.h"
#include "pw_status/try.h"

namespace pw::log_rpc {
namespace {

namespace FilterRule = ::pw::log::pwpb::FilterRule;
namespace LogEntry = ::pw::log::pwpb::LogEntry;

// Returns true if the provided log parameters match the given filter rule.
bool IsRuleMet(const Filter::Rule& rule,
               uint32_t level,
               ConstByteSpan module,
               uint32_t flags,
               ConstByteSpan thread) {
  if (level < static_cast<uint32_t>(rule.level_greater_than_or_equal)) {
    return false;
  }
  if ((rule.any_flags_set != 0) && ((flags & rule.any_flags_set) == 0)) {
    return false;
  }
  if (!rule.module_equals.empty() && !std::equal(module.begin(),
                                                 module.end(),
                                                 rule.module_equals.begin(),
                                                 rule.module_equals.end())) {
    return false;
  }
  if (!rule.thread_equals.empty() && !std::equal(thread.begin(),
                                                 thread.end(),
                                                 rule.thread_equals.begin(),
                                                 rule.thread_equals.end())) {
    return false;
  }
  return true;
}

}  // namespace

Status Filter::UpdateRulesFromProto(ConstByteSpan buffer) {
  if (rules_.empty()) {
    return Status::FailedPrecondition();
  }

  // Reset rules.
  for (auto& rule : rules_) {
    rule = {};
  }

  protobuf::Decoder decoder(buffer);
  Status status;
  for (size_t i = 0; (i < rules_.size()) && (status = decoder.Next()).ok();
       ++i) {
    ConstByteSpan rule_buffer;
    PW_TRY(decoder.ReadBytes(&rule_buffer));
    protobuf::Decoder rule_decoder(rule_buffer);
    while ((status = rule_decoder.Next()).ok()) {
      switch (static_cast<FilterRule::Fields>(rule_decoder.FieldNumber())) {
        case FilterRule::Fields::LEVEL_GREATER_THAN_OR_EQUAL:
          PW_TRY(rule_decoder.ReadUint32(reinterpret_cast<uint32_t*>(
              &rules_[i].level_greater_than_or_equal)));
          break;
        case FilterRule::Fields::MODULE_EQUALS: {
          ConstByteSpan module;
          PW_TRY(rule_decoder.ReadBytes(&module));
          if (module.size() > rules_[i].module_equals.max_size()) {
            return Status::InvalidArgument();
          }
          rules_[i].module_equals.assign(module.begin(), module.end());
        } break;
        case FilterRule::Fields::ANY_FLAGS_SET:
          PW_TRY(rule_decoder.ReadUint32(&rules_[i].any_flags_set));
          break;
        case FilterRule::Fields::ACTION:
          PW_TRY(rule_decoder.ReadUint32(
              reinterpret_cast<uint32_t*>(&rules_[i].action)));
          break;
        case FilterRule::Fields::THREAD_EQUALS: {
          ConstByteSpan thread;
          PW_TRY(rule_decoder.ReadBytes(&thread));
          if (thread.size() > rules_[i].thread_equals.max_size()) {
            return Status::InvalidArgument();
          }
          rules_[i].thread_equals.assign(thread.begin(), thread.end());
        } break;
      }
    }
  }
  return status.IsOutOfRange() ? OkStatus() : status;
}

bool Filter::ShouldDropLog(ConstByteSpan entry) const {
  if (rules_.empty()) {
    return false;
  }

  uint32_t log_level = 0;
  ConstByteSpan log_module;
  ConstByteSpan log_thread;
  uint32_t log_flags = 0;
  protobuf::Decoder decoder(entry);
  while (decoder.Next().ok()) {
    const auto field_num = static_cast<LogEntry::Fields>(decoder.FieldNumber());

    if (field_num == LogEntry::Fields::LINE_LEVEL) {
      if (decoder.ReadUint32(&log_level).ok()) {
        log_level &= PW_LOG_LEVEL_BITMASK;
      }

    } else if (field_num == LogEntry::Fields::MODULE) {
      decoder.ReadBytes(&log_module).IgnoreError();

    } else if (field_num == LogEntry::Fields::FLAGS) {
      decoder.ReadUint32(&log_flags).IgnoreError();

    } else if (field_num == LogEntry::Fields::THREAD) {
      decoder.ReadBytes(&log_thread).IgnoreError();
    }
  }

  // Follow the action of the first rule whose condition is met.
  for (const auto& rule : rules_) {
    if (rule.action == Filter::Rule::Action::kInactive) {
      continue;
    }
    if (IsRuleMet(rule, log_level, log_module, log_flags, log_thread)) {
      return rule.action == Filter::Rule::Action::kDrop;
    }
  }

  return false;
}

}  // namespace pw::log_rpc
