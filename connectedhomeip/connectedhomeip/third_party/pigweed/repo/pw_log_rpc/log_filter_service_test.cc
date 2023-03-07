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

#include "pw_log_rpc/log_filter_service.h"

#include <array>
#include <cstdint>
#include <limits>

#include "gtest/gtest.h"
#include "pw_bytes/endian.h"
#include "pw_log/proto/log.pwpb.h"
#include "pw_log_rpc/log_filter.h"
#include "pw_log_rpc/log_filter_map.h"
#include "pw_protobuf/bytes_utils.h"
#include "pw_protobuf/decoder.h"
#include "pw_result/result.h"
#include "pw_rpc/channel.h"
#include "pw_rpc/raw/test_method_context.h"

namespace pw::log_rpc {
namespace {

namespace FilterRule = ::pw::log::pwpb::FilterRule;
namespace GetFilterRequest = ::pw::log::pwpb::GetFilterRequest;
namespace SetFilterRequest = ::pw::log::pwpb::SetFilterRequest;

class FilterServiceTest : public ::testing::Test {
 public:
  FilterServiceTest() : filter_map_(filters_) {}

 protected:
  FilterMap filter_map_;
  static constexpr size_t kMaxFilterRules = 4;
  std::array<Filter::Rule, kMaxFilterRules> rules1_;
  std::array<Filter::Rule, kMaxFilterRules> rules2_;
  std::array<Filter::Rule, kMaxFilterRules> rules3_;
  static constexpr std::array<std::byte, cfg::kMaxFilterIdBytes> filter_id1_{
      std::byte(65), std::byte(66), std::byte(67), std::byte(0)};
  static constexpr std::array<std::byte, cfg::kMaxFilterIdBytes> filter_id2_{
      std::byte(68), std::byte(69), std::byte(70), std::byte(0)};
  static constexpr std::array<std::byte, cfg::kMaxFilterIdBytes> filter_id3_{
      std::byte(71), std::byte(72), std::byte(73), std::byte(0)};
  static constexpr size_t kMaxFilters = 3;
  std::array<Filter, kMaxFilters> filters_ = {
      Filter(filter_id1_, rules1_),
      Filter(filter_id2_, rules2_),
      Filter(filter_id3_, rules3_),
  };
};

TEST_F(FilterServiceTest, GetFilterIds) {
  PW_RAW_TEST_METHOD_CONTEXT(FilterService, ListFilterIds, 1)
  context(filter_map_);
  context.call({});
  ASSERT_EQ(OkStatus(), context.status());
  ASSERT_TRUE(context.done());
  ASSERT_EQ(context.responses().size(), 1u);
  protobuf::Decoder decoder(context.responses()[0]);

  for (const auto& filter : filter_map_.filters()) {
    ASSERT_EQ(decoder.Next(), OkStatus());
    ASSERT_EQ(decoder.FieldNumber(), 1u);  // filter_id
    ConstByteSpan filter_id;
    ASSERT_EQ(decoder.ReadBytes(&filter_id), OkStatus());
    ASSERT_EQ(filter_id.size(), filter.id().size());
    EXPECT_EQ(
        std::memcmp(filter_id.data(), filter.id().data(), filter_id.size()), 0);
  }
  EXPECT_FALSE(decoder.Next().ok());

  // No IDs reported when the filter map is empty.
  FilterMap empty_filter_map({});
  PW_RAW_TEST_METHOD_CONTEXT(FilterService, ListFilterIds, 1)
  no_filter_context(empty_filter_map);
  no_filter_context.call({});
  ASSERT_EQ(OkStatus(), no_filter_context.status());
  ASSERT_TRUE(no_filter_context.done());
  ASSERT_EQ(no_filter_context.responses().size(), 1u);
  protobuf::Decoder no_filter_decoder(no_filter_context.responses()[0]);
  uint32_t filter_count = 0;
  while (no_filter_decoder.Next().ok()) {
    EXPECT_EQ(no_filter_decoder.FieldNumber(), 1u);  // filter_id
    ++filter_count;
  }
  EXPECT_EQ(filter_count, 0u);
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

Status EncodeFilter(const Filter& filter,
                    log::pwpb::Filter::StreamEncoder& encoder) {
  for (auto& rule : filter.rules()) {
    FilterRule::StreamEncoder rule_encoder = encoder.GetRuleEncoder();
    PW_TRY(EncodeFilterRule(rule, rule_encoder));
  }
  return OkStatus();
}

Result<ConstByteSpan> EncodeFilterRequest(const Filter& filter,
                                          ByteSpan buffer) {
  stream::MemoryWriter writer(buffer);
  std::byte encode_buffer[256];
  protobuf::StreamEncoder encoder(writer, encode_buffer);
  PW_TRY(encoder.WriteBytes(
      static_cast<uint32_t>(SetFilterRequest::Fields::FILTER_ID), filter.id()));
  {
    log::pwpb::Filter::StreamEncoder filter_encoder = encoder.GetNestedEncoder(
        static_cast<uint32_t>(SetFilterRequest::Fields::FILTER));
    PW_TRY(EncodeFilter(filter, filter_encoder));
  }  // Let the StreamEncoder destructor finalize the data.
  return ConstByteSpan(writer.data(), writer.bytes_written());
}

void VerifyRule(const Filter::Rule& rule, const Filter::Rule& expected_rule) {
  EXPECT_EQ(rule.level_greater_than_or_equal,
            expected_rule.level_greater_than_or_equal);
  EXPECT_EQ(rule.module_equals, expected_rule.module_equals);
  EXPECT_EQ(rule.any_flags_set, expected_rule.any_flags_set);
  EXPECT_EQ(rule.thread_equals, expected_rule.thread_equals);
  EXPECT_EQ(rule.action, expected_rule.action);
}

TEST_F(FilterServiceTest, SetFilterRules) {
  const std::array<Filter::Rule, kMaxFilterRules> new_rules{{
      {
          .action = Filter::Rule::Action::kKeep,
          .level_greater_than_or_equal = FilterRule::Level::DEBUG_LEVEL,
          .any_flags_set = 0x0f,
          .module_equals{std::byte(123)},
          .thread_equals{std::byte('L'), std::byte('O'), std::byte('G')},
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
          .thread_equals{std::byte('P'),
                         std::byte('O'),
                         std::byte('W'),
                         std::byte('E'),
                         std::byte('R')},
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
  const Filter new_filter(
      filters_[0].id(),
      const_cast<std::array<Filter::Rule, kMaxFilterRules>&>(new_rules));

  std::byte request_buffer[512];
  const auto request = EncodeFilterRequest(new_filter, request_buffer);
  ASSERT_EQ(request.status(), OkStatus());

  PW_RAW_TEST_METHOD_CONTEXT(FilterService, SetFilter, 1)
  context(filter_map_);
  context.call(request.value());
  ASSERT_EQ(OkStatus(), context.status());

  size_t i = 0;
  for (const auto& rule : filters_[0].rules()) {
    VerifyRule(rule, new_rules[i++]);
  }
}

TEST_F(FilterServiceTest, SetFilterRulesWhenUsedByDrain) {
  const std::array<Filter::Rule, kMaxFilterRules> new_filter_rules{{
      {
          .action = Filter::Rule::Action::kKeep,
          .level_greater_than_or_equal = FilterRule::Level::CRITICAL_LEVEL,
          .any_flags_set = 0xfd,
          .module_equals{std::byte(543)},
          .thread_equals{std::byte('M'),
                         std::byte('0'),
                         std::byte('L'),
                         std::byte('O'),
                         std::byte('G')},
      },
      {
          .action = Filter::Rule::Action::kInactive,
          .level_greater_than_or_equal = FilterRule::Level::ANY_LEVEL,
          .any_flags_set = 0xca,
          .module_equals{},
          .thread_equals{},
      },
      {
          .action = Filter::Rule::Action::kKeep,
          .level_greater_than_or_equal = FilterRule::Level::INFO_LEVEL,
          .any_flags_set = 0xabcd,
          .module_equals{std::byte(9000)},
          .thread_equals{std::byte('P'),
                         std::byte('O'),
                         std::byte('W'),
                         std::byte('E'),
                         std::byte('R')},
      },
      {
          .action = Filter::Rule::Action::kDrop,
          .level_greater_than_or_equal = FilterRule::Level::ANY_LEVEL,
          .any_flags_set = 0,
          .module_equals{std::byte(123)},
          .thread_equals{std::byte('P'),
                         std::byte('O'),
                         std::byte('W'),
                         std::byte('E'),
                         std::byte('R')},
      },
  }};
  Filter& filter = filters_[0];
  const Filter new_filter(
      filter.id(),
      const_cast<std::array<Filter::Rule, kMaxFilterRules>&>(new_filter_rules));

  std::byte request_buffer[256];
  const auto request = EncodeFilterRequest(new_filter, request_buffer);
  ASSERT_EQ(request.status(), OkStatus());

  PW_RAW_TEST_METHOD_CONTEXT(FilterService, SetFilter, 1)
  context(filter_map_);
  context.call(request.value());
  ASSERT_EQ(OkStatus(), context.status());

  size_t i = 0;
  for (const auto& rule : filter.rules()) {
    VerifyRule(rule, new_filter_rules[i++]);
  }

  // An empty request should not modify the filter.
  PW_RAW_TEST_METHOD_CONTEXT(FilterService, SetFilter, 1)
  context_no_filter(filter_map_);
  context_no_filter.call({});
  EXPECT_EQ(Status::OutOfRange(), context_no_filter.status());

  i = 0;
  for (const auto& rule : filter.rules()) {
    VerifyRule(rule, new_filter_rules[i++]);
  }

  // A new request for logs with a new filter updates filter.
  const std::array<Filter::Rule, kMaxFilterRules> second_filter_rules{{
      {
          .action = Filter::Rule::Action::kKeep,
          .level_greater_than_or_equal = FilterRule::Level::DEBUG_LEVEL,
          .any_flags_set = 0xab,
          .module_equals{},
          .thread_equals{},
      },
      {
          .action = Filter::Rule::Action::kDrop,
          .level_greater_than_or_equal = FilterRule::Level::ANY_LEVEL,
          .any_flags_set = 0x11,
          .module_equals{std::byte(34)},
          .thread_equals{std::byte('L'), std::byte('O'), std::byte('G')},
      },
      {
          .action = Filter::Rule::Action::kKeep,
          .level_greater_than_or_equal = FilterRule::Level::ANY_LEVEL,
          .any_flags_set = 0xef,
          .module_equals{std::byte(23)},
          .thread_equals{std::byte('R'), std::byte('P'), std::byte('C')},
      },
      {
          .action = Filter::Rule::Action::kDrop,
          .level_greater_than_or_equal = FilterRule::Level::ANY_LEVEL,
          .any_flags_set = 0x0f,
          .module_equals{},
          .thread_equals{std::byte('R'), std::byte('P'), std::byte('C')},
      },
  }};
  const Filter second_filter(
      filter.id(),
      const_cast<std::array<Filter::Rule, kMaxFilterRules>&>(
          second_filter_rules));

  std::memset(request_buffer, 0, sizeof(request_buffer));
  const auto second_filter_request =
      EncodeFilterRequest(second_filter, request_buffer);
  ASSERT_EQ(second_filter_request.status(), OkStatus());
  PW_RAW_TEST_METHOD_CONTEXT(FilterService, SetFilter, 1)
  context_new_filter(filter_map_);
  context_new_filter.call(second_filter_request.value());
  ASSERT_EQ(OkStatus(), context.status());

  i = 0;
  for (const auto& rule : filter.rules()) {
    VerifyRule(rule, second_filter_rules[i++]);
  }
}

void VerifyFilterRule(protobuf::Decoder& decoder,
                      const Filter::Rule& expected_rule) {
  ASSERT_TRUE(decoder.Next().ok());
  ASSERT_EQ(
      decoder.FieldNumber(),
      static_cast<uint32_t>(FilterRule::Fields::LEVEL_GREATER_THAN_OR_EQUAL));
  FilterRule::Level level_greater_than_or_equal;
  ASSERT_EQ(decoder.ReadUint32(
                reinterpret_cast<uint32_t*>(&level_greater_than_or_equal)),
            OkStatus());
  EXPECT_EQ(level_greater_than_or_equal,
            expected_rule.level_greater_than_or_equal);

  ASSERT_TRUE(decoder.Next().ok());
  ASSERT_EQ(decoder.FieldNumber(),
            static_cast<uint32_t>(FilterRule::Fields::MODULE_EQUALS));
  ConstByteSpan module_equals;
  ASSERT_EQ(decoder.ReadBytes(&module_equals), OkStatus());
  ASSERT_EQ(module_equals.size(), expected_rule.module_equals.size());
  EXPECT_EQ(std::memcmp(module_equals.data(),
                        expected_rule.module_equals.data(),
                        module_equals.size()),
            0);

  ASSERT_TRUE(decoder.Next().ok());
  ASSERT_EQ(decoder.FieldNumber(),
            static_cast<uint32_t>(FilterRule::Fields::ANY_FLAGS_SET));
  uint32_t any_flags_set;
  ASSERT_EQ(decoder.ReadUint32(&any_flags_set), OkStatus());
  EXPECT_EQ(any_flags_set, expected_rule.any_flags_set);

  ASSERT_TRUE(decoder.Next().ok());
  ASSERT_EQ(decoder.FieldNumber(),
            static_cast<uint32_t>(FilterRule::Fields::ACTION));
  Filter::Rule::Action action;
  ASSERT_EQ(decoder.ReadUint32(reinterpret_cast<uint32_t*>(&action)),
            OkStatus());
  EXPECT_EQ(action, expected_rule.action);

  ASSERT_TRUE(decoder.Next().ok());
  ASSERT_EQ(decoder.FieldNumber(),
            static_cast<uint32_t>(FilterRule::Fields::THREAD_EQUALS));
  ConstByteSpan thread;
  ASSERT_EQ(decoder.ReadBytes(&thread), OkStatus());
  ASSERT_EQ(thread.size(), expected_rule.thread_equals.size());
  EXPECT_EQ(
      std::memcmp(
          thread.data(), expected_rule.thread_equals.data(), thread.size()),
      0);
}

void VerifyFilterRules(protobuf::Decoder& decoder,
                       span<const Filter::Rule> expected_rules) {
  size_t rules_found = 0;
  while (decoder.Next().ok()) {
    ConstByteSpan rule;
    EXPECT_TRUE(decoder.ReadBytes(&rule).ok());
    protobuf::Decoder rule_decoder(rule);
    if (rules_found >= expected_rules.size()) {
      break;
    }
    VerifyFilterRule(rule_decoder, expected_rules[rules_found]);
    ++rules_found;
  }
  EXPECT_EQ(rules_found, expected_rules.size());
}

TEST_F(FilterServiceTest, GetFilterRules) {
  PW_RAW_TEST_METHOD_CONTEXT(FilterService, GetFilter, 1)
  context(filter_map_);

  std::byte request_buffer[64];
  GetFilterRequest::MemoryEncoder encoder(request_buffer);
  ASSERT_EQ(OkStatus(), encoder.WriteFilterId(filter_id1_));
  const auto request = ConstByteSpan(encoder);
  context.call(request);
  ASSERT_EQ(OkStatus(), context.status());
  ASSERT_TRUE(context.done());
  ASSERT_EQ(context.responses().size(), 1u);

  // Verify against empty rules.
  protobuf::Decoder decoder(context.responses()[0]);
  VerifyFilterRules(decoder, rules1_);

  // Partially populate rules.
  rules1_[0].action = Filter::Rule::Action::kKeep;
  rules1_[0].level_greater_than_or_equal = FilterRule::Level::DEBUG_LEVEL;
  rules1_[0].any_flags_set = 0xab;
  const std::array<std::byte, 2> module1{std::byte(123), std::byte(0xab)};
  rules1_[0].module_equals.assign(module1.begin(), module1.end());
  const std::array<std::byte, 4> thread1{
      std::byte('H'), std::byte('O'), std::byte('S'), std::byte('T')};
  rules1_[0].thread_equals.assign(thread1.begin(), thread1.end());
  rules1_[1].action = Filter::Rule::Action::kDrop;
  rules1_[1].level_greater_than_or_equal = FilterRule::Level::ERROR_LEVEL;
  rules1_[1].any_flags_set = 0;

  PW_RAW_TEST_METHOD_CONTEXT(FilterService, GetFilter, 1)
  context2(filter_map_);
  context2.call(request);
  ASSERT_EQ(OkStatus(), context2.status());
  ASSERT_EQ(context2.responses().size(), 1u);
  protobuf::Decoder decoder2(context2.responses()[0]);
  VerifyFilterRules(decoder2, rules1_);

  // Modify the rest of the filter rules.
  rules1_[2].action = Filter::Rule::Action::kKeep;
  rules1_[2].level_greater_than_or_equal = FilterRule::Level::FATAL_LEVEL;
  rules1_[2].any_flags_set = 0xcd;
  const std::array<std::byte, 2> module2{std::byte(1), std::byte(2)};
  rules1_[2].module_equals.assign(module2.begin(), module2.end());
  const std::array<std::byte, 3> thread2{
      std::byte('A'), std::byte('P'), std::byte('P')};
  rules1_[2].thread_equals.assign(thread2.begin(), thread2.end());
  rules1_[3].action = Filter::Rule::Action::kInactive;

  PW_RAW_TEST_METHOD_CONTEXT(FilterService, GetFilter, 1)
  context3(filter_map_);
  context3.call(request);
  ASSERT_EQ(OkStatus(), context3.status());
  ASSERT_EQ(context3.responses().size(), 1u);
  protobuf::Decoder decoder3(context3.responses()[0]);
  VerifyFilterRules(decoder3, rules1_);
}

}  // namespace
}  // namespace pw::log_rpc
