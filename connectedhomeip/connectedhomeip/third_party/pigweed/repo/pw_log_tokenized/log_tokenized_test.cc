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

#define PW_LOG_MODULE_NAME "log module name!"

// Configure the module so that the test runs against known values.
#undef PW_LOG_TOKENIZED_LEVEL_BITS
#undef PW_LOG_TOKENIZED_MODULE_BITS
#undef PW_LOG_TOKENIZED_FLAG_BITS
#undef PW_LOG_TOKENIZED_LINE_BITS

#define PW_LOG_TOKENIZED_LEVEL_BITS 3
#define PW_LOG_TOKENIZED_MODULE_BITS 16
#define PW_LOG_TOKENIZED_FLAG_BITS 2
#define PW_LOG_TOKENIZED_LINE_BITS 11

#include "pw_log_tokenized/log_tokenized.h"

#include "gtest/gtest.h"
#include "pw_log_tokenized_private/test_utils.h"

namespace pw::log_tokenized {
namespace {

TEST(LogTokenized, FormatString) {
  PW_LOG_TOKENIZED_TO_GLOBAL_HANDLER_WITH_PAYLOAD(63, 1023, "hello %d", 1);
  EXPECT_STREQ(last_log.format_string,
               "■msg♦hello %d■module♦log module name!■file♦" __FILE__);
}

constexpr uintptr_t kModuleToken =
    PW_TOKENIZER_STRING_TOKEN(PW_LOG_MODULE_NAME) &
    ((1u << PW_LOG_TOKENIZED_MODULE_BITS) - 1);

TEST(LogTokenized, LogMetadata_LevelTooLarge_Clamps) {
  auto check_metadata = [] {
    Metadata metadata = Metadata(last_log.metadata);
    EXPECT_EQ(metadata.level(), 7u);
    EXPECT_EQ(metadata.flags(), 0u);
    EXPECT_EQ(metadata.module(), kModuleToken);
    EXPECT_TRUE(metadata.line_number() == 55u || metadata.line_number() == 45u);
  };

  PW_LOG_TOKENIZED_TO_GLOBAL_HANDLER_WITH_PAYLOAD(8, 0, "hello");
  check_metadata();

  pw_log_tokenized_Test_LogMetadata_LevelTooLarge_Clamps();
  check_metadata();
}

TEST(LogTokenized, LogMetadata_TooManyFlags_Truncates) {
  auto check_metadata = [] {
    Metadata metadata = Metadata(last_log.metadata);
    EXPECT_EQ(metadata.level(), 1u);
    EXPECT_EQ(metadata.flags(), 0b11u);
    EXPECT_EQ(metadata.module(), kModuleToken);
    EXPECT_TRUE(metadata.line_number() == 71u || metadata.line_number() == 49u);
  };

  PW_LOG_TOKENIZED_TO_GLOBAL_HANDLER_WITH_PAYLOAD(1, 0xFFFFFFFF, "hello");
  check_metadata();

  pw_log_tokenized_Test_LogMetadata_TooManyFlags_Truncates();
  check_metadata();
}

TEST(LogTokenized, LogMetadata_VariousValues) {
  auto check_metadata = [] {
    Metadata metadata = Metadata(last_log.metadata);
    EXPECT_EQ(metadata.level(), 6u);
    EXPECT_EQ(metadata.flags(), 3u);
    EXPECT_EQ(metadata.module(), kModuleToken);
    EXPECT_EQ(last_log.arg_count, 1u);
    EXPECT_TRUE(metadata.line_number() == 88u || metadata.line_number() == 53u);
  };

  PW_LOG_TOKENIZED_TO_GLOBAL_HANDLER_WITH_PAYLOAD(6, 3, "hello%s", "?");
  check_metadata();

  pw_log_tokenized_Test_LogMetadata_LogMetadata_VariousValues();
  check_metadata();
}

TEST(LogTokenized, LogMetadata_Zero) {
  auto check_metadata = [] {
    Metadata metadata = Metadata(last_log.metadata);
    EXPECT_EQ(metadata.level(), 0u);
    EXPECT_EQ(metadata.flags(), 0u);
    EXPECT_EQ(metadata.module(), kModuleToken);
    EXPECT_EQ(last_log.arg_count, 0u);
    EXPECT_TRUE(metadata.line_number() == 106u ||
                metadata.line_number() == 57u);
  };

  PW_LOG_TOKENIZED_TO_GLOBAL_HANDLER_WITH_PAYLOAD(0, 0, "hello");
  check_metadata();

  pw_log_tokenized_Test_LogMetadata_LogMetadata_Zero();
  check_metadata();
}

TEST(LogTokenized, LogMetadata_MaxValues) {
#line 2047
  PW_LOG_TOKENIZED_TO_GLOBAL_HANDLER_WITH_PAYLOAD(7, 3, "hello %d", 1);

  Metadata metadata = Metadata(last_log.metadata);
  EXPECT_EQ(metadata.line_number(), 2047u);
  EXPECT_EQ(metadata.level(), 7u);
  EXPECT_EQ(metadata.flags(), 3u);
  EXPECT_EQ(metadata.module(), kModuleToken);
  EXPECT_EQ(last_log.arg_count, 1u);
}

TEST(LogTokenized, LogMetadata_LineNumberTooLarge_IsZero) {
#line 2048  // At 11 bits, the largest representable line is 2047
  PW_LOG_TOKENIZED_TO_GLOBAL_HANDLER_WITH_PAYLOAD(7, 3, "hello %d", 1);
  EXPECT_EQ(Metadata(last_log.metadata).line_number(), 0u);

#line 2049
  PW_LOG_TOKENIZED_TO_GLOBAL_HANDLER_WITH_PAYLOAD(7, 3, "hello %d", 1);
  EXPECT_EQ(Metadata(last_log.metadata).line_number(), 0u);

#line 99999
  PW_LOG_TOKENIZED_TO_GLOBAL_HANDLER_WITH_PAYLOAD(7, 3, "hello %d", 1);
  EXPECT_EQ(Metadata(last_log.metadata).line_number(), 0u);
}

}  // namespace
}  // namespace pw::log_tokenized
