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

#include "pw_log/proto_utils.h"

#include "gtest/gtest.h"
#include "pw_bytes/span.h"
#include "pw_containers/algorithm.h"
#include "pw_log/levels.h"
#include "pw_log/proto/log.pwpb.h"
#include "pw_protobuf/bytes_utils.h"
#include "pw_protobuf/decoder.h"

namespace pw::log {

void VerifyTokenizedLogEntry(pw::protobuf::Decoder& entry_decoder,
                             pw::log_tokenized::Metadata expected_metadata,
                             ConstByteSpan expected_tokenized_data,
                             const int64_t expected_timestamp,
                             ConstByteSpan expected_thread_name) {
  ConstByteSpan tokenized_data;
  EXPECT_TRUE(entry_decoder.Next().ok());  // message [tokenized]
  EXPECT_EQ(entry_decoder.FieldNumber(),
            static_cast<uint32_t>(log::pwpb::LogEntry::Fields::MESSAGE));
  EXPECT_TRUE(entry_decoder.ReadBytes(&tokenized_data).ok());
  EXPECT_TRUE(std::memcmp(tokenized_data.data(),
                          expected_tokenized_data.data(),
                          expected_tokenized_data.size()) == 0);

  uint32_t line_level;
  EXPECT_TRUE(entry_decoder.Next().ok());  // line_level
  EXPECT_EQ(entry_decoder.FieldNumber(),
            static_cast<uint32_t>(log::pwpb::LogEntry::Fields::LINE_LEVEL));
  EXPECT_TRUE(entry_decoder.ReadUint32(&line_level).ok());

  uint32_t line_number;
  uint8_t level;
  std::tie(line_number, level) = UnpackLineLevel(line_level);
  EXPECT_EQ(expected_metadata.level(), level);
  EXPECT_EQ(expected_metadata.line_number(), line_number);

  if (expected_metadata.flags() != 0) {
    uint32_t flags;
    EXPECT_TRUE(entry_decoder.Next().ok());  // flags
    EXPECT_EQ(entry_decoder.FieldNumber(),
              static_cast<uint32_t>(log::pwpb::LogEntry::Fields::FLAGS));
    EXPECT_TRUE(entry_decoder.ReadUint32(&flags).ok());
    EXPECT_EQ(expected_metadata.flags(), flags);
  }

  int64_t timestamp;
  EXPECT_TRUE(entry_decoder.Next().ok());  // timestamp
  EXPECT_TRUE(
      entry_decoder.FieldNumber() ==
          static_cast<uint32_t>(log::pwpb::LogEntry::Fields::TIMESTAMP) ||
      entry_decoder.FieldNumber() ==
          static_cast<uint32_t>(
              log::pwpb::LogEntry::Fields::TIME_SINCE_LAST_ENTRY));
  EXPECT_TRUE(entry_decoder.ReadInt64(&timestamp).ok());
  EXPECT_EQ(expected_timestamp, timestamp);

  if (expected_metadata.module() != 0) {
    EXPECT_TRUE(entry_decoder.Next().ok());  // module name
    EXPECT_EQ(entry_decoder.FieldNumber(),
              static_cast<uint32_t>(log::pwpb::LogEntry::Fields::MODULE));
    const Result<uint32_t> module =
        protobuf::DecodeBytesToUint32(entry_decoder);
    ASSERT_TRUE(module.ok());
    EXPECT_EQ(expected_metadata.module(), module.value());
  }

  if (!expected_thread_name.empty()) {
    ConstByteSpan tokenized_thread_name;
    EXPECT_TRUE(entry_decoder.Next().ok());  // thread [tokenized]
    EXPECT_EQ(entry_decoder.FieldNumber(),
              static_cast<uint32_t>(log::pwpb::LogEntry::Fields::THREAD));
    EXPECT_TRUE(entry_decoder.ReadBytes(&tokenized_thread_name).ok());
    EXPECT_TRUE(std::memcmp(tokenized_thread_name.data(),
                            expected_thread_name.data(),
                            expected_thread_name.size()) == 0);
  }
}

void VerifyLogEntry(pw::protobuf::Decoder& entry_decoder,
                    int expected_level,
                    unsigned int expected_flags,
                    std::string_view expected_module,
                    std::string_view expected_thread_name,
                    std::string_view expected_file_name,
                    int expected_line_number,
                    int64_t expected_ticks_since_epoch,
                    std::string_view expected_message) {
  std::string_view message;
  EXPECT_TRUE(entry_decoder.Next().ok());  // message
  EXPECT_EQ(entry_decoder.FieldNumber(),
            static_cast<uint32_t>(log::pwpb::LogEntry::Fields::MESSAGE));
  EXPECT_TRUE(entry_decoder.ReadString(&message).ok());
  EXPECT_TRUE(pw::containers::Equal(message, expected_message));

  uint32_t line_level;
  EXPECT_TRUE(entry_decoder.Next().ok());  // line_level
  EXPECT_EQ(entry_decoder.FieldNumber(),
            static_cast<uint32_t>(log::pwpb::LogEntry::Fields::LINE_LEVEL));
  EXPECT_TRUE(entry_decoder.ReadUint32(&line_level).ok());
  uint32_t line_number;
  uint8_t level;
  std::tie(line_number, level) = UnpackLineLevel(line_level);
  EXPECT_EQ(static_cast<unsigned int>(expected_line_number), line_number);
  EXPECT_EQ(expected_level, level);

  if (expected_flags != 0) {
    uint32_t flags;
    EXPECT_TRUE(entry_decoder.Next().ok());  // flags
    EXPECT_EQ(entry_decoder.FieldNumber(),
              static_cast<uint32_t>(log::pwpb::LogEntry::Fields::FLAGS));
    EXPECT_TRUE(entry_decoder.ReadUint32(&flags).ok());
    EXPECT_EQ(expected_flags, flags);
  }

  int64_t timestamp;
  EXPECT_TRUE(entry_decoder.Next().ok());  // timestamp
  EXPECT_TRUE(
      entry_decoder.FieldNumber() ==
          static_cast<uint32_t>(log::pwpb::LogEntry::Fields::TIMESTAMP) ||
      entry_decoder.FieldNumber() ==
          static_cast<uint32_t>(
              log::pwpb::LogEntry::Fields::TIME_SINCE_LAST_ENTRY));
  EXPECT_TRUE(entry_decoder.ReadInt64(&timestamp).ok());
  EXPECT_EQ(expected_ticks_since_epoch, timestamp);

  if (!expected_module.empty()) {
    std::string_view module_name;
    EXPECT_TRUE(entry_decoder.Next().ok());  // module
    EXPECT_EQ(entry_decoder.FieldNumber(),
              static_cast<uint32_t>(log::pwpb::LogEntry::Fields::MODULE));
    EXPECT_TRUE(entry_decoder.ReadString(&module_name).ok());
    EXPECT_TRUE(pw::containers::Equal(module_name, expected_module));
  }

  if (!expected_file_name.empty()) {
    std::string_view file_name;
    EXPECT_TRUE(entry_decoder.Next().ok());  // file
    EXPECT_EQ(entry_decoder.FieldNumber(),
              static_cast<uint32_t>(log::pwpb::LogEntry::Fields::FILE));
    EXPECT_TRUE(entry_decoder.ReadString(&file_name).ok());
    EXPECT_TRUE(pw::containers::Equal(file_name, expected_file_name));
  }

  if (!expected_thread_name.empty()) {
    std::string_view thread_name;
    EXPECT_TRUE(entry_decoder.Next().ok());  // file
    EXPECT_EQ(entry_decoder.FieldNumber(),
              static_cast<uint32_t>(log::pwpb::LogEntry::Fields::THREAD));
    EXPECT_TRUE(entry_decoder.ReadString(&thread_name).ok());
    EXPECT_TRUE(pw::containers::Equal(thread_name, expected_thread_name));
  }
}

TEST(UtilsTest, LineLevelPacking) {
  constexpr uint8_t kExpectedLevel = PW_LOG_LEVEL_ERROR;
  constexpr uint32_t kExpectedLine = 1234567;
  constexpr uint32_t kExpectedLineLevel =
      (kExpectedLine << PW_LOG_LEVEL_BITS) |
      (kExpectedLevel & PW_LOG_LEVEL_BITMASK);

  EXPECT_EQ(kExpectedLineLevel, PackLineLevel(kExpectedLine, kExpectedLevel));
}

TEST(UtilsTest, LineLevelUnpacking) {
  constexpr uint8_t kExpectedLevel = PW_LOG_LEVEL_ERROR;
  constexpr uint32_t kExpectedLine = 1234567;
  constexpr uint32_t kExpectedLineLevel =
      (kExpectedLine << PW_LOG_LEVEL_BITS) |
      (kExpectedLevel & PW_LOG_LEVEL_BITMASK);

  uint32_t line_number;
  uint8_t level;
  std::tie(line_number, level) = UnpackLineLevel(kExpectedLineLevel);

  EXPECT_EQ(kExpectedLine, line_number);
  EXPECT_EQ(kExpectedLevel, level);
}

TEST(UtilsTest, LineLevelPackAndUnpack) {
  constexpr uint8_t kExpectedLevel = PW_LOG_LEVEL_ERROR;
  constexpr uint32_t kExpectedLine = 1234567;

  uint32_t line_number;
  uint8_t level;
  std::tie(line_number, level) =
      UnpackLineLevel(PackLineLevel(kExpectedLine, kExpectedLevel));

  EXPECT_EQ(kExpectedLine, line_number);
  EXPECT_EQ(kExpectedLevel, level);
}

TEST(UtilsTest, EncodeTokenizedLog) {
  constexpr std::byte kTokenizedData[1] = {std::byte(0x01)};
  constexpr int64_t kExpectedTimestamp = 1;
  constexpr std::byte kExpectedThreadName[1] = {std::byte(0x02)};
  std::byte encode_buffer[32];

  pw::log_tokenized::Metadata metadata =
      pw::log_tokenized::Metadata::Set<1, 2, 3, 4>();

  Result<ConstByteSpan> result = EncodeTokenizedLog(metadata,
                                                    kTokenizedData,
                                                    kExpectedTimestamp,
                                                    kExpectedThreadName,
                                                    encode_buffer);
  EXPECT_TRUE(result.ok());

  pw::protobuf::Decoder log_decoder(result.value());
  VerifyTokenizedLogEntry(log_decoder,
                          metadata,
                          kTokenizedData,
                          kExpectedTimestamp,
                          kExpectedThreadName);

  result = EncodeTokenizedLog(metadata,
                              reinterpret_cast<const uint8_t*>(kTokenizedData),
                              sizeof(kTokenizedData),
                              kExpectedTimestamp,
                              kExpectedThreadName,
                              encode_buffer);
  EXPECT_TRUE(result.ok());

  log_decoder.Reset(result.value());
  VerifyTokenizedLogEntry(log_decoder,
                          metadata,
                          kTokenizedData,
                          kExpectedTimestamp,
                          kExpectedThreadName);
}

TEST(UtilsTest, EncodeTokenizedLog_EmptyFlags) {
  constexpr std::byte kTokenizedData[1] = {std::byte(0x01)};
  constexpr int64_t kExpectedTimestamp = 1;
  constexpr std::byte kExpectedThreadName[1] = {std::byte(0x02)};
  std::byte encode_buffer[32];

  // Create an empty flags set.
  pw::log_tokenized::Metadata metadata =
      pw::log_tokenized::Metadata::Set<1, 2, 0, 4>();

  Result<ConstByteSpan> result = EncodeTokenizedLog(metadata,
                                                    kTokenizedData,
                                                    kExpectedTimestamp,
                                                    kExpectedThreadName,
                                                    encode_buffer);
  EXPECT_TRUE(result.ok());

  pw::protobuf::Decoder log_decoder(result.value());
  VerifyTokenizedLogEntry(log_decoder,
                          metadata,
                          kTokenizedData,
                          kExpectedTimestamp,
                          kExpectedThreadName);
}

TEST(UtilsTest, EncodeTokenizedLog_InsufficientSpace) {
  constexpr std::byte kTokenizedData[1] = {std::byte(0x01)};
  constexpr int64_t kExpectedTimestamp = 1;
  constexpr std::byte kExpectedThreadName[1] = {std::byte(0x02)};
  std::byte encode_buffer[1];

  pw::log_tokenized::Metadata metadata =
      pw::log_tokenized::Metadata::Set<1, 2, 3, 4>();

  Result<ConstByteSpan> result = EncodeTokenizedLog(metadata,
                                                    kTokenizedData,
                                                    kExpectedTimestamp,
                                                    kExpectedThreadName,
                                                    encode_buffer);
  EXPECT_TRUE(result.status().IsResourceExhausted());
}

TEST(UtilsTest, EncodeLog) {
  constexpr int kExpectedLevel = PW_LOG_LEVEL_INFO;
  constexpr unsigned int kExpectedFlags = 2;
  constexpr std::string_view kExpectedModule("TST");
  constexpr std::string_view kExpectedThread("thread");
  constexpr std::string_view kExpectedFile("proto_test.cc");
  constexpr int kExpectedLine = 14;
  constexpr int64_t kExpectedTimestamp = 1;
  constexpr std::string_view kExpectedMessage("msg");
  std::byte encode_buffer[64];

  Result<ConstByteSpan> result = EncodeLog(kExpectedLevel,
                                           kExpectedFlags,
                                           kExpectedModule,
                                           kExpectedThread,
                                           kExpectedFile,
                                           kExpectedLine,
                                           kExpectedTimestamp,
                                           kExpectedMessage,
                                           encode_buffer);
  EXPECT_TRUE(result.ok());

  pw::protobuf::Decoder log_decoder(result.value());
  VerifyLogEntry(log_decoder,
                 kExpectedLevel,
                 kExpectedFlags,
                 kExpectedModule,
                 kExpectedThread,
                 kExpectedFile,
                 kExpectedLine,
                 kExpectedTimestamp,
                 kExpectedMessage);
}

TEST(UtilsTest, EncodeLog_EmptyFlags) {
  constexpr int kExpectedLevel = PW_LOG_LEVEL_INFO;
  constexpr unsigned int kExpectedFlags = 0;
  constexpr std::string_view kExpectedModule("TST");
  constexpr std::string_view kExpectedThread("thread");
  constexpr std::string_view kExpectedFile("proto_test.cc");
  constexpr int kExpectedLine = 14;
  constexpr int64_t kExpectedTimestamp = 1;
  constexpr std::string_view kExpectedMessage("msg");
  std::byte encode_buffer[64];

  Result<ConstByteSpan> result = EncodeLog(kExpectedLevel,
                                           kExpectedFlags,
                                           kExpectedModule,
                                           kExpectedThread,
                                           kExpectedFile,
                                           kExpectedLine,
                                           kExpectedTimestamp,
                                           kExpectedMessage,
                                           encode_buffer);
  EXPECT_TRUE(result.ok());

  pw::protobuf::Decoder log_decoder(result.value());
  VerifyLogEntry(log_decoder,
                 kExpectedLevel,
                 kExpectedFlags,
                 kExpectedModule,
                 kExpectedThread,
                 kExpectedFile,
                 kExpectedLine,
                 kExpectedTimestamp,
                 kExpectedMessage);
}

TEST(UtilsTest, EncodeLog_EmptyFile) {
  constexpr int kExpectedLevel = PW_LOG_LEVEL_INFO;
  constexpr unsigned int kExpectedFlags = 0;
  constexpr std::string_view kExpectedModule("TST");
  constexpr std::string_view kExpectedThread("thread");
  constexpr std::string_view kExpectedFile;
  constexpr int kExpectedLine = 14;
  constexpr int64_t kExpectedTimestamp = 1;
  constexpr std::string_view kExpectedMessage("msg");
  std::byte encode_buffer[64];

  Result<ConstByteSpan> result = EncodeLog(kExpectedLevel,
                                           kExpectedFlags,
                                           kExpectedModule,
                                           kExpectedThread,
                                           kExpectedFile,
                                           kExpectedLine,
                                           kExpectedTimestamp,
                                           kExpectedMessage,
                                           encode_buffer);
  EXPECT_TRUE(result.ok());

  pw::protobuf::Decoder log_decoder(result.value());
  VerifyLogEntry(log_decoder,
                 kExpectedLevel,
                 kExpectedFlags,
                 kExpectedModule,
                 kExpectedThread,
                 kExpectedFile,
                 kExpectedLine,
                 kExpectedTimestamp,
                 kExpectedMessage);
}

TEST(UtilsTest, EncodeLog_EmptyModule) {
  constexpr int kExpectedLevel = PW_LOG_LEVEL_INFO;
  constexpr unsigned int kExpectedFlags = 3;
  constexpr std::string_view kExpectedModule;
  constexpr std::string_view kExpectedThread("thread");
  constexpr std::string_view kExpectedFile("test.cc");
  constexpr int kExpectedLine = 14;
  constexpr int64_t kExpectedTimestamp = 1;
  constexpr std::string_view kExpectedMessage("msg");
  std::byte encode_buffer[64];

  Result<ConstByteSpan> result = EncodeLog(kExpectedLevel,
                                           kExpectedFlags,
                                           kExpectedModule,
                                           kExpectedThread,
                                           kExpectedFile,
                                           kExpectedLine,
                                           kExpectedTimestamp,
                                           kExpectedMessage,
                                           encode_buffer);
  EXPECT_TRUE(result.ok());

  pw::protobuf::Decoder log_decoder(result.value());
  VerifyLogEntry(log_decoder,
                 kExpectedLevel,
                 kExpectedFlags,
                 kExpectedModule,
                 kExpectedThread,
                 kExpectedFile,
                 kExpectedLine,
                 kExpectedTimestamp,
                 kExpectedMessage);
}

TEST(UtilsTest, EncodeLog_EmptyThread) {
  constexpr int kExpectedLevel = PW_LOG_LEVEL_INFO;
  constexpr unsigned int kExpectedFlags = 2;
  constexpr std::string_view kExpectedModule("TST");
  constexpr std::string_view kExpectedThread;
  constexpr std::string_view kExpectedFile("proto_test.cc");
  constexpr int kExpectedLine = 14;
  constexpr int64_t kExpectedTimestamp = 1;
  constexpr std::string_view kExpectedMessage("msg");
  std::byte encode_buffer[64];

  Result<ConstByteSpan> result = EncodeLog(kExpectedLevel,
                                           kExpectedFlags,
                                           kExpectedModule,
                                           kExpectedThread,
                                           kExpectedFile,
                                           kExpectedLine,
                                           kExpectedTimestamp,
                                           kExpectedMessage,
                                           encode_buffer);
  EXPECT_TRUE(result.ok());

  pw::protobuf::Decoder log_decoder(result.value());
  VerifyLogEntry(log_decoder,
                 kExpectedLevel,
                 kExpectedFlags,
                 kExpectedModule,
                 kExpectedThread,
                 kExpectedFile,
                 kExpectedLine,
                 kExpectedTimestamp,
                 kExpectedMessage);
}

TEST(UtilsTest, EncodeLog_EmptyMessage) {
  constexpr int kExpectedLevel = PW_LOG_LEVEL_INFO;
  constexpr unsigned int kExpectedFlags = 0;
  constexpr std::string_view kExpectedModule;
  constexpr std::string_view kExpectedThread;
  constexpr std::string_view kExpectedFile;
  constexpr int kExpectedLine = 14;
  constexpr int64_t kExpectedTimestamp = 1;
  constexpr std::string_view kExpectedMessage;
  std::byte encode_buffer[64];

  Result<ConstByteSpan> result = EncodeLog(kExpectedLevel,
                                           kExpectedFlags,
                                           kExpectedModule,
                                           kExpectedThread,
                                           kExpectedFile,
                                           kExpectedLine,
                                           kExpectedTimestamp,
                                           kExpectedMessage,
                                           encode_buffer);

  EXPECT_TRUE(result.status().IsInvalidArgument());
}

TEST(UtilsTest, EncodeLog_InsufficientSpace) {
  constexpr int kExpectedLevel = PW_LOG_LEVEL_INFO;
  constexpr unsigned int kExpectedFlags = 0;
  constexpr std::string_view kExpectedModule;
  constexpr std::string_view kExpectedThread;
  constexpr std::string_view kExpectedFile;
  constexpr int kExpectedLine = 14;
  constexpr int64_t kExpectedTimestamp = 1;
  constexpr std::string_view kExpectedMessage("msg");
  std::byte encode_buffer[1];

  Result<ConstByteSpan> result = EncodeLog(kExpectedLevel,
                                           kExpectedFlags,
                                           kExpectedModule,
                                           kExpectedThread,
                                           kExpectedFile,
                                           kExpectedLine,
                                           kExpectedTimestamp,
                                           kExpectedMessage,
                                           encode_buffer);

  EXPECT_TRUE(result.status().IsResourceExhausted());
}

}  // namespace pw::log
