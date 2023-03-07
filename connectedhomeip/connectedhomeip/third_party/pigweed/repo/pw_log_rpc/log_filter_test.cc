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

#include <array>
#include <cstddef>
#include <cstring>

#include "gtest/gtest.h"
#include "pw_bytes/endian.h"
#include "pw_log/levels.h"
#include "pw_log/log.h"
#include "pw_log/proto/log.pwpb.h"
#include "pw_log/proto_utils.h"
#include "pw_log_rpc/log_filter_map.h"
#include "pw_log_tokenized/metadata.h"
#include "pw_result/result.h"
#include "pw_status/status.h"
#include "pw_status/try.h"

namespace pw::log_rpc {
namespace {

namespace FilterRule = ::pw::log::pwpb::FilterRule;

constexpr uint32_t kSampleModule = 0x1234;
constexpr uint32_t kSampleFlags = 0x3;
const std::array<std::byte, cfg::kMaxThreadNameBytes - 7> kSampleThread = {
    std::byte('R'), std::byte('P'), std::byte('C')};
constexpr char kSampleMessage[] = "message";
constexpr auto kSampleModuleLittleEndian =
    bytes::CopyInOrder<uint32_t>(endian::little, kSampleModule);

// Creates and encodes a log entry in the provided buffer.
template <uintptr_t log_level, uintptr_t module, uintptr_t flags>
Result<ConstByteSpan> EncodeLogEntry(std::string_view message,
                                     ByteSpan buffer,
                                     ConstByteSpan thread) {
  auto metadata = log_tokenized::Metadata::Set<log_level, module, flags, 0>();
  return log::EncodeTokenizedLog(metadata,
                                 as_bytes(span<const char>(message)),
                                 /*ticks_since_epoch=*/0,
                                 thread,
                                 buffer);
}

Status EncodeFilterRule(const Filter::Rule& rule,
                        FilterRule::StreamEncoder& encoder) {
  PW_TRY(
      encoder.WriteLevelGreaterThanOrEqual(rule.level_greater_than_or_equal));
  PW_TRY(encoder.WriteModuleEquals(rule.module_equals));
  PW_TRY(encoder.WriteAnyFlagsSet(rule.any_flags_set));
  PW_TRY(encoder.WriteThreadEquals(rule.thread_equals));
  return encoder.WriteAction(static_cast<FilterRule::Action>(rule.action));
}

Result<ConstByteSpan> EncodeFilter(const Filter& filter, ByteSpan buffer) {
  log::pwpb::Filter::MemoryEncoder encoder(buffer);
  for (auto& rule : filter.rules()) {
    FilterRule::StreamEncoder rule_encoder = encoder.GetRuleEncoder();
    PW_TRY(EncodeFilterRule(rule, rule_encoder));
  }
  return ConstByteSpan(encoder);
}

void VerifyRule(const Filter::Rule& rule, const Filter::Rule& expected_rule) {
  EXPECT_EQ(rule.level_greater_than_or_equal,
            expected_rule.level_greater_than_or_equal);
  EXPECT_EQ(rule.module_equals, expected_rule.module_equals);
  EXPECT_EQ(rule.any_flags_set, expected_rule.any_flags_set);
  EXPECT_EQ(rule.thread_equals, expected_rule.thread_equals);
  EXPECT_EQ(rule.action, expected_rule.action);
}

TEST(FilterMap, RetrieveFiltersById) {
  const std::array<std::byte, cfg::kMaxFilterIdBytes> filter_id1{
      std::byte(0xfe), std::byte(0xed), std::byte(0xba), std::byte(0xb1)};
  const std::array<std::byte, cfg::kMaxFilterIdBytes> filter_id2{
      std::byte(0xca), std::byte(0xfe), std::byte(0xc0), std::byte(0xc0)};
  const std::array<std::byte, cfg::kMaxFilterIdBytes> filter_id3{
      std::byte(0xfa), std::byte(0xfe), std::byte(0xf1), std::byte(0xf0)};
  std::array<Filter, 3> filters = {
      Filter(filter_id1, {}),
      Filter(filter_id2, {}),
      Filter(filter_id3, {}),
  };

  FilterMap filter_map(filters);

  // Check that each filters() element points to the same object provided.
  span<const Filter> filter_list = filter_map.filters();
  ASSERT_EQ(filter_list.size(), filters.size());
  size_t i = 0;
  for (auto& filter : filter_list) {
    EXPECT_EQ(&filter, &filters[i++]);
  }

  auto filter_result = filter_map.GetFilterFromId(filter_id3);
  ASSERT_TRUE(filter_result.ok());
  EXPECT_EQ(filter_result.value(), &filters[2]);

  filter_result = filter_map.GetFilterFromId(filter_id2);
  ASSERT_TRUE(filter_result.ok());
  EXPECT_EQ(filter_result.value(), &filters[1]);

  filter_result = filter_map.GetFilterFromId(filter_id1);
  ASSERT_TRUE(filter_result.ok());
  EXPECT_EQ(filter_result.value(), &filters[0]);

  const std::array<std::byte, cfg::kMaxFilterIdBytes> invalid_id{
      std::byte(0xd0), std::byte(0x1c), std::byte(0xe7), std::byte(0xea)};
  filter_result = filter_map.GetFilterFromId(invalid_id);
  ASSERT_EQ(filter_result.status(), Status::NotFound());
}

TEST(Filter, UpdateFilterRules) {
  const std::array<std::byte, cfg::kMaxFilterIdBytes> filter_id{
      std::byte(0xba), std::byte(0x1d), std::byte(0xba), std::byte(0xb1)};
  std::array<Filter::Rule, 4> rules;
  const std::array<Filter::Rule, 4> new_rules{{
      {
          .action = Filter::Rule::Action::kKeep,
          .level_greater_than_or_equal = FilterRule::Level::DEBUG_LEVEL,
          .any_flags_set = 0x0f,
          .module_equals{std::byte(123)},
          .thread_equals{},
      },
      {
          .action = Filter::Rule::Action::kInactive,
          .level_greater_than_or_equal = FilterRule::Level::ANY_LEVEL,
          .any_flags_set = 0xef,
          .module_equals{},
          .thread_equals{std::byte('L'), std::byte('O'), std::byte('G')},
      },
      {
          .action = Filter::Rule::Action::kKeep,
          .level_greater_than_or_equal = FilterRule::Level::INFO_LEVEL,
          .any_flags_set = 0x1234,
          .module_equals{std::byte(99)},
          .thread_equals{},
      },
      {
          .action = Filter::Rule::Action::kDrop,
          .level_greater_than_or_equal = FilterRule::Level::ANY_LEVEL,
          .any_flags_set = 0,
          .module_equals{std::byte(4)},
          .thread_equals{std::byte('P'),
                         std::byte('O'),
                         std::byte('W'),
                         std::byte('E'),
                         std::byte('R')},
      },
  }};

  Filter filter(filter_id, rules);
  const Filter new_filter(filter_id,
                          const_cast<std::array<Filter::Rule, 4>&>(new_rules));
  std::byte buffer[256];
  auto encode_result = EncodeFilter(new_filter, buffer);
  ASSERT_EQ(encode_result.status(), OkStatus());
  EXPECT_EQ(filter.UpdateRulesFromProto(encode_result.value()), OkStatus());

  size_t i = 0;
  for (const auto& rule : filter.rules()) {
    VerifyRule(rule, new_rules[i++]);
  }

  // A new filter with no rules should clear filter.
  const Filter empty_filter(filter_id, {});
  std::memset(buffer, 0, sizeof(buffer));
  encode_result = EncodeFilter(empty_filter, buffer);
  ASSERT_EQ(encode_result.status(), OkStatus());
  EXPECT_EQ(filter.UpdateRulesFromProto(encode_result.value()), OkStatus());
  const Filter::Rule empty_rule{
      .action = Filter::Rule::Action::kInactive,
      .level_greater_than_or_equal = FilterRule::Level::ANY_LEVEL,
      .any_flags_set = 0,
      .module_equals{},
      .thread_equals{},
  };
  for (const auto& rule : filter.rules()) {
    VerifyRule(rule, empty_rule);
  }
  EXPECT_TRUE(empty_filter.rules().empty());

  // Passing a new filter with less rules.
  const std::array<Filter::Rule, 2> few_rules{{
      {
          .action = Filter::Rule::Action::kInactive,
          .level_greater_than_or_equal = FilterRule::Level::ANY_LEVEL,
          .any_flags_set = 0xef,
          .module_equals{},
          .thread_equals{},
      },
      {
          .action = Filter::Rule::Action::kKeep,
          .level_greater_than_or_equal = FilterRule::Level::INFO_LEVEL,
          .any_flags_set = 0x1234,
          .module_equals{std::byte(99)},
          .thread_equals{std::byte('P'),
                         std::byte('O'),
                         std::byte('W'),
                         std::byte('E'),
                         std::byte('R')},
      },
  }};
  const Filter filter_few_rules(
      filter_id, const_cast<std::array<Filter::Rule, 2>&>(few_rules));
  std::memset(buffer, 0, sizeof(buffer));
  encode_result = EncodeFilter(filter_few_rules, buffer);
  ASSERT_EQ(encode_result.status(), OkStatus());
  EXPECT_EQ(filter.UpdateRulesFromProto(encode_result.value()), OkStatus());
  i = 0;
  for (const auto& rule : filter.rules()) {
    if (i >= few_rules.size()) {
      VerifyRule(rule, empty_rule);
    } else {
      VerifyRule(rule, few_rules[i++]);
    }
  }

  // Passing a new filter with extra rules.
  const std::array<Filter::Rule, 6> extra_rules{{
      {
          .action = Filter::Rule::Action::kKeep,
          .level_greater_than_or_equal = FilterRule::Level::DEBUG_LEVEL,
          .any_flags_set = 0x0f,
          .module_equals{std::byte(123)},
          .thread_equals{std::byte('P'),
                         std::byte('O'),
                         std::byte('W'),
                         std::byte('E'),
                         std::byte('R')},
      },
      {
          .action = Filter::Rule::Action::kInactive,
          .level_greater_than_or_equal = FilterRule::Level::ANY_LEVEL,
          .any_flags_set = 0xef,
          .module_equals{},
          .thread_equals{},
      },
      {
          .action = Filter::Rule::Action::kInactive,
          .level_greater_than_or_equal = FilterRule::Level::ANY_LEVEL,
          .any_flags_set = 0xef,
          .module_equals{},
          .thread_equals{},
      },
      {
          .action = Filter::Rule::Action::kKeep,
          .level_greater_than_or_equal = FilterRule::Level::INFO_LEVEL,
          .any_flags_set = 0x1234,
          .module_equals{std::byte(99)},
          .thread_equals{std::byte('L'), std::byte('O'), std::byte('G')},
      },
      {
          .action = Filter::Rule::Action::kDrop,
          .level_greater_than_or_equal = FilterRule::Level::ANY_LEVEL,
          .any_flags_set = 0,
          .module_equals{std::byte(4)},
          .thread_equals{std::byte('L'), std::byte('O'), std::byte('G')},
      },
      {
          .action = Filter::Rule::Action::kKeep,
          .level_greater_than_or_equal = FilterRule::Level::INFO_LEVEL,
          .any_flags_set = 0x1234,
          .module_equals{std::byte('M'),
                         std::byte('0'),
                         std::byte('L'),
                         std::byte('O'),
                         std::byte('G')},
          .thread_equals{},
      },
  }};
  const Filter filter_extra_rules(
      filter_id, const_cast<std::array<Filter::Rule, 6>&>(extra_rules));
  std::memset(buffer, 0, sizeof(buffer));
  encode_result = EncodeFilter(filter_extra_rules, buffer);
  ASSERT_EQ(encode_result.status(), OkStatus());
  EXPECT_EQ(filter.UpdateRulesFromProto(encode_result.value()), OkStatus());
  i = 0;
  for (const auto& rule : filter.rules()) {
    VerifyRule(rule, extra_rules[i++]);
  }

  // A filter with no rules buffer cannot get rules updated.
  Filter filter_no_rules(filter_id, {});
  EXPECT_EQ(filter_no_rules.UpdateRulesFromProto(encode_result.value()),
            Status::FailedPrecondition());
}

TEST(FilterTest, FilterLogsRuleDefaultDrop) {
  const std::array<Filter::Rule, 2> rules{{
      {
          .action = Filter::Rule::Action::kKeep,
          .level_greater_than_or_equal = FilterRule::Level::INFO_LEVEL,
          .any_flags_set = kSampleFlags,
          .module_equals{kSampleModuleLittleEndian.begin(),
                         kSampleModuleLittleEndian.end()},
          .thread_equals{kSampleThread.begin(), kSampleThread.end()},
      },
      // This rule catches all logs.
      {
          .action = Filter::Rule::Action::kDrop,
          .level_greater_than_or_equal = FilterRule::Level::ANY_LEVEL,
          .any_flags_set = 0,
          .module_equals = {},
          .thread_equals{},
      },
  }};
  const std::array<std::byte, cfg::kMaxFilterIdBytes> filter_id{
      std::byte(0xfe), std::byte(0xed), std::byte(0xba), std::byte(0xb1)};
  const Filter filter(filter_id,
                      const_cast<std::array<Filter::Rule, 2>&>(rules));

  std::array<std::byte, 50> buffer;
  const Result<ConstByteSpan> log_entry_info =
      EncodeLogEntry<PW_LOG_LEVEL_INFO, kSampleModule, kSampleFlags>(
          kSampleMessage, buffer, kSampleThread);
  ASSERT_EQ(log_entry_info.status(), OkStatus());
  EXPECT_FALSE(filter.ShouldDropLog(log_entry_info.value()));

  buffer.fill(std::byte(0));
  const Result<ConstByteSpan> log_entry_debug =
      EncodeLogEntry<PW_LOG_LEVEL_DEBUG, kSampleModule, kSampleFlags>(
          kSampleMessage, buffer, kSampleThread);
  ASSERT_EQ(log_entry_debug.status(), OkStatus());
  EXPECT_TRUE(filter.ShouldDropLog(log_entry_debug.value()));

  buffer.fill(std::byte(0));
  const Result<ConstByteSpan> log_entry_warn =
      EncodeLogEntry<PW_LOG_LEVEL_WARN, kSampleModule, kSampleFlags>(
          kSampleMessage, buffer, kSampleThread);
  ASSERT_EQ(log_entry_warn.status(), OkStatus());
  EXPECT_FALSE(filter.ShouldDropLog(log_entry_warn.value()));

  buffer.fill(std::byte(0));
  const Result<ConstByteSpan> log_entry_error =
      EncodeLogEntry<PW_LOG_LEVEL_ERROR, kSampleModule, kSampleFlags>(
          kSampleMessage, buffer, kSampleThread);
  ASSERT_EQ(log_entry_error.status(), OkStatus());
  EXPECT_FALSE(filter.ShouldDropLog(log_entry_error.value()));

  buffer.fill(std::byte(0));
  const Result<ConstByteSpan> log_entry_info_different =
      EncodeLogEntry<PW_LOG_LEVEL_INFO, 0, 0>(kSampleMessage, buffer, {});
  ASSERT_EQ(log_entry_info_different.status(), OkStatus());
  EXPECT_TRUE(filter.ShouldDropLog(log_entry_info_different.value()));
  // Because the last rule catches all logs, the filter default action is not
  // applied.
  const Filter filter_default_drop(
      filter_id, const_cast<std::array<Filter::Rule, 2>&>(rules));
  EXPECT_TRUE(
      filter_default_drop.ShouldDropLog(log_entry_info_different.value()));

  buffer.fill(std::byte(0));
  const Result<ConstByteSpan> log_entry_same_flags =
      EncodeLogEntry<0, 0, kSampleFlags>(kSampleMessage, buffer, {});
  ASSERT_EQ(log_entry_same_flags.status(), OkStatus());
  EXPECT_TRUE(filter.ShouldDropLog(log_entry_same_flags.value()));

  buffer.fill(std::byte(0));
  const Result<ConstByteSpan> log_entry_same_module =
      EncodeLogEntry<0, kSampleModule, 0>(kSampleMessage, buffer, {});
  ASSERT_EQ(log_entry_same_module.status(), OkStatus());
  EXPECT_TRUE(filter.ShouldDropLog(log_entry_same_module.value()));

  buffer.fill(std::byte(0));
  const Result<ConstByteSpan> log_entry_same_thread =
      EncodeLogEntry<0, 0, 0>(kSampleMessage, buffer, kSampleThread);
  ASSERT_EQ(log_entry_same_thread.status(), OkStatus());
  EXPECT_TRUE(filter.ShouldDropLog(log_entry_same_thread.value()));
}

TEST(FilterTest, FilterLogsKeepLogsWhenNoRuleMatches) {
  // There is no rule that catches all logs.
  const std::array<Filter::Rule, 1> rules{{
      {
          .action = Filter::Rule::Action::kKeep,
          .level_greater_than_or_equal = FilterRule::Level::INFO_LEVEL,
          .any_flags_set = kSampleFlags,
          .module_equals = {kSampleModuleLittleEndian.begin(),
                            kSampleModuleLittleEndian.end()},
          .thread_equals = {kSampleThread.begin(), kSampleThread.end()},
      },
  }};

  // Filters should not share rules if they are mutable, to avoid race
  // conditions.
  const std::array<std::byte, cfg::kMaxFilterIdBytes> filter_id{
      std::byte(0xfe), std::byte(0xed), std::byte(0xba), std::byte(0xb1)};
  const Filter filter(filter_id,
                      const_cast<std::array<Filter::Rule, 1>&>(rules));

  std::array<std::byte, 50> buffer;
  const Result<ConstByteSpan> log_entry_info =
      EncodeLogEntry<PW_LOG_LEVEL_INFO, kSampleModule, kSampleFlags>(
          kSampleMessage, buffer, kSampleThread);
  ASSERT_EQ(log_entry_info.status(), OkStatus());
  EXPECT_FALSE(filter.ShouldDropLog(log_entry_info.value()));

  buffer.fill(std::byte(0));
  const Result<ConstByteSpan> log_entry_debug =
      EncodeLogEntry<PW_LOG_LEVEL_DEBUG, kSampleModule, kSampleFlags>(
          kSampleMessage, buffer, kSampleThread);
  ASSERT_EQ(log_entry_debug.status(), OkStatus());
  EXPECT_FALSE(filter.ShouldDropLog(log_entry_debug.value()));

  buffer.fill(std::byte(0));
  const Result<ConstByteSpan> log_entry_warn =
      EncodeLogEntry<PW_LOG_LEVEL_WARN, kSampleModule, kSampleFlags>(
          kSampleMessage, buffer, kSampleThread);
  ASSERT_EQ(log_entry_warn.status(), OkStatus());
  EXPECT_FALSE(filter.ShouldDropLog(log_entry_warn.value()));

  buffer.fill(std::byte(0));
  const Result<ConstByteSpan> log_entry_error =
      EncodeLogEntry<PW_LOG_LEVEL_ERROR, kSampleModule, kSampleFlags>(
          kSampleMessage, buffer, kSampleThread);
  ASSERT_EQ(log_entry_error.status(), OkStatus());
  EXPECT_FALSE(filter.ShouldDropLog(log_entry_error.value()));

  buffer.fill(std::byte(0));
  const Result<ConstByteSpan> log_entry_info_different =
      EncodeLogEntry<PW_LOG_LEVEL_INFO, 0, 0>(kSampleMessage, buffer, {});
  ASSERT_EQ(log_entry_info_different.status(), OkStatus());
  EXPECT_FALSE(filter.ShouldDropLog(log_entry_info_different.value()));

  buffer.fill(std::byte(0));
  const Result<ConstByteSpan> log_entry_same_flags =
      EncodeLogEntry<0, 0, kSampleFlags>(kSampleMessage, buffer, {});
  ASSERT_EQ(log_entry_same_flags.status(), OkStatus());
  EXPECT_FALSE(filter.ShouldDropLog(log_entry_same_flags.value()));

  buffer.fill(std::byte(0));
  const Result<ConstByteSpan> log_entry_same_module =
      EncodeLogEntry<0, kSampleModule, 0>(kSampleMessage, buffer, {});
  ASSERT_EQ(log_entry_same_module.status(), OkStatus());
  EXPECT_FALSE(filter.ShouldDropLog(log_entry_same_module.value()));

  buffer.fill(std::byte(0));
  const Result<ConstByteSpan> log_entry_same_thread =
      EncodeLogEntry<0, 0, 0>(kSampleMessage, buffer, kSampleThread);
  ASSERT_EQ(log_entry_same_thread.status(), OkStatus());
  EXPECT_FALSE(filter.ShouldDropLog(log_entry_same_thread.value()));
}

TEST(FilterTest, FilterLogsKeepLogsWhenRulesEmpty) {
  // Filters should not share rules if they are mutable, to avoid race
  // conditions.
  const std::array<std::byte, cfg::kMaxFilterIdBytes> filter_id{
      std::byte(0xfe), std::byte(0xed), std::byte(0xba), std::byte(0xb1)};
  const Filter filter(filter_id, {});

  std::array<std::byte, 50> buffer;
  const Result<ConstByteSpan> log_entry_info =
      EncodeLogEntry<PW_LOG_LEVEL_INFO, kSampleModule, kSampleFlags>(
          kSampleMessage, buffer, kSampleThread);
  ASSERT_EQ(log_entry_info.status(), OkStatus());
  EXPECT_FALSE(filter.ShouldDropLog(log_entry_info.value()));

  buffer.fill(std::byte(0));
  const Result<ConstByteSpan> log_entry_debug =
      EncodeLogEntry<PW_LOG_LEVEL_DEBUG, kSampleModule, kSampleFlags>(
          kSampleMessage, buffer, kSampleThread);
  ASSERT_EQ(log_entry_debug.status(), OkStatus());
  EXPECT_FALSE(filter.ShouldDropLog(log_entry_debug.value()));

  buffer.fill(std::byte(0));
  const Result<ConstByteSpan> log_entry_warn =
      EncodeLogEntry<PW_LOG_LEVEL_WARN, kSampleModule, kSampleFlags>(
          kSampleMessage, buffer, kSampleThread);
  ASSERT_EQ(log_entry_warn.status(), OkStatus());
  EXPECT_FALSE(filter.ShouldDropLog(log_entry_warn.value()));

  buffer.fill(std::byte(0));
  const Result<ConstByteSpan> log_entry_error =
      EncodeLogEntry<PW_LOG_LEVEL_ERROR, kSampleModule, kSampleFlags>(
          kSampleMessage, buffer, kSampleThread);
  ASSERT_EQ(log_entry_error.status(), OkStatus());
  EXPECT_FALSE(filter.ShouldDropLog(log_entry_error.value()));

  buffer.fill(std::byte(0));
  const Result<ConstByteSpan> log_entry_info_different =
      EncodeLogEntry<PW_LOG_LEVEL_INFO, 0, 0>(kSampleMessage, buffer, {});
  ASSERT_EQ(log_entry_info_different.status(), OkStatus());
  EXPECT_FALSE(filter.ShouldDropLog(log_entry_info_different.value()));

  buffer.fill(std::byte(0));
  const Result<ConstByteSpan> log_entry_same_flags =
      EncodeLogEntry<0, 0, kSampleFlags>(kSampleMessage, buffer, {});
  ASSERT_EQ(log_entry_same_flags.status(), OkStatus());
  EXPECT_FALSE(filter.ShouldDropLog(log_entry_same_flags.value()));

  buffer.fill(std::byte(0));
  const Result<ConstByteSpan> log_entry_same_module =
      EncodeLogEntry<0, kSampleModule, 0>(kSampleMessage, buffer, {});
  ASSERT_EQ(log_entry_same_module.status(), OkStatus());
  EXPECT_FALSE(filter.ShouldDropLog(log_entry_same_module.value()));

  buffer.fill(std::byte(0));
  const Result<ConstByteSpan> log_entry_same_thread =
      EncodeLogEntry<0, 0, 0>(kSampleMessage, buffer, kSampleThread);
  ASSERT_EQ(log_entry_same_thread.status(), OkStatus());
  EXPECT_FALSE(filter.ShouldDropLog(log_entry_same_thread.value()));
}

TEST(FilterTest, FilterLogsFirstRuleWins) {
  const std::array<Filter::Rule, 2> rules{{
      {
          .action = Filter::Rule::Action::kKeep,
          .level_greater_than_or_equal = FilterRule::Level::INFO_LEVEL,
          .any_flags_set = kSampleFlags,
          .module_equals = {kSampleModuleLittleEndian.begin(),
                            kSampleModuleLittleEndian.end()},
          .thread_equals = {kSampleThread.begin(), kSampleThread.end()},
      },
      {
          .action = Filter::Rule::Action::kDrop,
          .level_greater_than_or_equal = FilterRule::Level::INFO_LEVEL,
          .any_flags_set = kSampleFlags,
          .module_equals = {kSampleModuleLittleEndian.begin(),
                            kSampleModuleLittleEndian.end()},
          .thread_equals = {kSampleThread.begin(), kSampleThread.end()},
      },
  }};
  const std::array<Filter::Rule, 2> rules_reversed{{
      {
          .action = Filter::Rule::Action::kDrop,
          .level_greater_than_or_equal = FilterRule::Level::INFO_LEVEL,
          .any_flags_set = kSampleFlags,
          .module_equals = {kSampleModuleLittleEndian.begin(),
                            kSampleModuleLittleEndian.end()},
          .thread_equals = {kSampleThread.begin(), kSampleThread.end()},
      },
      {
          .action = Filter::Rule::Action::kKeep,
          .level_greater_than_or_equal = FilterRule::Level::INFO_LEVEL,
          .any_flags_set = kSampleFlags,
          .module_equals = {kSampleModuleLittleEndian.begin(),
                            kSampleModuleLittleEndian.end()},
          .thread_equals = {kSampleThread.begin(), kSampleThread.end()},
      },
  }};
  const std::array<std::byte, cfg::kMaxFilterIdBytes> filter_id1{
      std::byte(0xfe), std::byte(0xed), std::byte(0xba), std::byte(0xb1)};
  const std::array<std::byte, cfg::kMaxFilterIdBytes> filter_id2{
      std::byte(0), std::byte(0), std::byte(0), std::byte(2)};
  const Filter filter(filter_id1,
                      const_cast<std::array<Filter::Rule, 2>&>(rules));
  const Filter filter_reverse_rules(
      filter_id2, const_cast<std::array<Filter::Rule, 2>&>(rules_reversed));

  std::array<std::byte, 50> buffer;
  const Result<ConstByteSpan> log_entry_info =
      EncodeLogEntry<PW_LOG_LEVEL_INFO, kSampleModule, kSampleFlags>(
          kSampleMessage, buffer, kSampleThread);
  ASSERT_EQ(log_entry_info.status(), OkStatus());
  EXPECT_FALSE(filter.ShouldDropLog(log_entry_info.value()));
  EXPECT_TRUE(filter_reverse_rules.ShouldDropLog(log_entry_info.value()));
}

TEST(FilterTest, DropFilterRuleDueToThreadName) {
  const std::array<std::byte, cfg::kMaxThreadNameBytes - 7> kDropThread = {
      std::byte('L'), std::byte('O'), std::byte('G')};
  const std::array<Filter::Rule, 2> rules{{
      {
          .action = Filter::Rule::Action::kKeep,
          .level_greater_than_or_equal = FilterRule::Level::INFO_LEVEL,
          .any_flags_set = kSampleFlags,
          .module_equals = {kSampleModuleLittleEndian.begin(),
                            kSampleModuleLittleEndian.end()},
          .thread_equals = {kDropThread.begin(), kDropThread.end()},
      },
      {
          .action = Filter::Rule::Action::kDrop,
          .level_greater_than_or_equal = FilterRule::Level::INFO_LEVEL,
          .any_flags_set = kSampleFlags,
          .module_equals = {kSampleModuleLittleEndian.begin(),
                            kSampleModuleLittleEndian.end()},
          .thread_equals = {kSampleThread.begin(), kSampleThread.end()},
      },
  }};

  const std::array<Filter::Rule, 2> drop_rule{{
      {
          .action = Filter::Rule::Action::kDrop,
          .level_greater_than_or_equal = FilterRule::Level::INFO_LEVEL,
          .any_flags_set = kSampleFlags,
          .module_equals = {kSampleModuleLittleEndian.begin(),
                            kSampleModuleLittleEndian.end()},
          .thread_equals = {kDropThread.begin(), kDropThread.end()},
      },
  }};

  const std::array<std::byte, cfg::kMaxFilterIdBytes> filter_id1{
      std::byte(0xba), std::byte(0x1d), std::byte(0xba), std::byte(0xb1)};
  // A filter's thread_equals name that does and does not match the log's thread
  // name.
  const Filter filter(filter_id1,
                      const_cast<std::array<Filter::Rule, 2>&>(rules));
  const std::array<std::byte, cfg::kMaxFilterIdBytes> filter_id2{
      std::byte(0), std::byte(0), std::byte(0), std::byte(2)};
  // A filter's thread_equals name that does not match the log's thread name.
  const Filter filter_with_unregistered_filter_rule(
      filter_id2, const_cast<std::array<Filter::Rule, 2>&>(drop_rule));
  std::array<std::byte, 50> buffer;
  const Result<ConstByteSpan> log_entry_thread =
      EncodeLogEntry<PW_LOG_LEVEL_INFO, kSampleModule, kSampleFlags>(
          kSampleMessage, buffer, kSampleThread);
  ASSERT_EQ(log_entry_thread.status(), OkStatus());
  // Set filter rules to kDrop to showcase the output difference.
  // Drop_rule not being dropped, while rules is dropped successfully.
  EXPECT_TRUE(filter.ShouldDropLog(log_entry_thread.value()));
  EXPECT_FALSE(filter_with_unregistered_filter_rule.ShouldDropLog(
      log_entry_thread.value()));
}

TEST(FilterTest, UpdateFilterWithLargeThreadNamePasses) {
  // Threads are limited to a size of kMaxThreadNameBytes.
  // However, the excess bytes will not be in the updated rules.
  const std::array<std::byte, cfg::kMaxThreadNameBytes + 1>
      kThreadNameLongerThanAllowed = {
          std::byte('L'),
          std::byte('O'),
          std::byte('C'),
          std::byte('A'),
          std::byte('L'),
          std::byte('E'),
          std::byte('G'),
          std::byte('R'),
          std::byte('E'),
          std::byte('S'),
          std::byte('S'),
      };

  const std::array<Filter::Rule, 2> rule{{
      {
          .action = Filter::Rule::Action::kKeep,
          .level_greater_than_or_equal = FilterRule::Level::INFO_LEVEL,
          .any_flags_set = kSampleFlags,
          .module_equals = {kSampleModuleLittleEndian.begin(),
                            kSampleModuleLittleEndian.end()},
          .thread_equals = {kThreadNameLongerThanAllowed.begin(),
                            kThreadNameLongerThanAllowed.end()},
      },
      {
          .action = Filter::Rule::Action::kKeep,
          .level_greater_than_or_equal = FilterRule::Level::INFO_LEVEL,
          .any_flags_set = kSampleFlags,
          .module_equals = {kSampleModuleLittleEndian.begin(),
                            kSampleModuleLittleEndian.end()},
          .thread_equals = {kSampleThread.begin(), kSampleThread.end()},
      },
  }};

  const std::array<std::byte, cfg::kMaxFilterIdBytes> filter_id{
      std::byte(0xba), std::byte(0x1d), std::byte(0xba), std::byte(0xb1)};
  Filter filter(filter_id, const_cast<std::array<Filter::Rule, 2>&>(rule));
  std::byte buffer[256];
  auto encode_result = EncodeFilter(filter, buffer);
  ASSERT_EQ(encode_result.status(), OkStatus());
  EXPECT_EQ(filter.UpdateRulesFromProto(encode_result.value()), OkStatus());
  size_t i = 0;
  for (const auto& rules : filter.rules()) {
    VerifyRule(rules, rule[i++]);
  }
}

TEST(FilterTest, UpdateFilterWithLargeThreadNameFails) {
  const std::array<Filter::Rule, 1> rule_with_more_than_ten_bytes{{{
      .action = Filter::Rule::Action::kKeep,
      .level_greater_than_or_equal = FilterRule::Level::INFO_LEVEL,
      .any_flags_set = kSampleFlags,
      .module_equals = {kSampleModuleLittleEndian.begin(),
                        kSampleModuleLittleEndian.end()},
      .thread_equals = {kSampleThread.begin(), kSampleThread.end()},
  }}};
  const std::array<std::byte, cfg::kMaxFilterIdBytes> filter_id{
      std::byte(0xba), std::byte(0x1d), std::byte(0xba), std::byte(0xb1)};
  Filter filter(
      filter_id,
      const_cast<std::array<Filter::Rule, 1>&>(rule_with_more_than_ten_bytes));
  std::byte buffer[256];
  log::pwpb::Filter::MemoryEncoder encoder(buffer);
  {
    std::array<const std::byte, cfg::kMaxThreadNameBytes + 1>
        kThreadNameLongerThanAllowed = {
            std::byte('L'),
            std::byte('O'),
            std::byte('C'),
            std::byte('A'),
            std::byte('L'),
            std::byte('E'),
            std::byte('G'),
            std::byte('R'),
            std::byte('E'),
            std::byte('S'),
            std::byte('S'),
        };
    // Stream encoder writes to the buffer when it goes out of scope.
    FilterRule::StreamEncoder rule_encoder = encoder.GetRuleEncoder();
    ASSERT_EQ(rule_encoder.WriteThreadEquals(kThreadNameLongerThanAllowed),
              OkStatus());
  }
  EXPECT_EQ(filter.UpdateRulesFromProto(ConstByteSpan(encoder)),
            Status::InvalidArgument());
}
}  // namespace
}  // namespace pw::log_rpc
