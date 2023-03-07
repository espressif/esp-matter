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

#include "pw_hex_dump/hex_dump.h"

#include <array>
#include <cinttypes>
#include <cstdint>
#include <cstring>
#include <string_view>

#include "gtest/gtest.h"
#include "pw_log/log.h"
#include "pw_span/span.h"

namespace pw::dump {
namespace {

std::array<const std::byte, 33> source_data = {
    std::byte(0xa4), std::byte(0xcc), std::byte(0x32), std::byte(0x62),
    std::byte(0x9b), std::byte(0x46), std::byte(0x38), std::byte(0x1a),
    std::byte(0x23), std::byte(0x1a), std::byte(0x2a), std::byte(0x7a),
    std::byte(0xbc), std::byte(0xe2), std::byte(0x40), std::byte(0xa0),
    std::byte(0xff), std::byte(0x33), std::byte(0xe5), std::byte(0x2b),
    std::byte(0x9e), std::byte(0x9f), std::byte(0x6b), std::byte(0x3c),
    std::byte(0xbe), std::byte(0x9b), std::byte(0x89), std::byte(0x3c),
    std::byte(0x7e), std::byte(0x4a), std::byte(0x7a), std::byte(0x48),
    std::byte(0x18)};

std::array<const std::byte, 15> short_string = {
    std::byte('m'),
    std::byte('y'),
    std::byte(' '),
    std::byte('t'),
    std::byte('e'),
    std::byte('s'),
    std::byte('t'),
    std::byte(' '),
    std::byte('s'),
    std::byte('t'),
    std::byte('r'),
    std::byte('i'),
    std::byte('n'),
    std::byte('g'),
    std::byte('\n'),
};

class HexDump : public ::testing::Test {
 protected:
  HexDump() { dumper_ = FormattedHexDumper(dest_, default_flags_); }

  // Sufficiently large destination buffer to hold line-by-line formatted hex
  // dump.
  std::array<char, 256> dest_ = {0};
  FormattedHexDumper dumper_;
  FormattedHexDumper::Flags default_flags_ = {
      .bytes_per_line = 16,
      .group_every = 1,
      .show_ascii = false,
      .show_header = false,
      .prefix_mode = FormattedHexDumper::AddressMode::kDisabled};
};

class SmallBuffer : public ::testing::Test {
 protected:
  SmallBuffer() {
    // Disable address prefix for most of the tests as it's platform-specific.
    dumper_ = FormattedHexDumper(dest_, default_flags_);
  }

  // Small destination buffer that should be inadequate in some cases.
  std::array<char, 7> dest_ = {0};
  FormattedHexDumper dumper_;
  FormattedHexDumper::Flags default_flags_ = {
      .bytes_per_line = 16,
      .group_every = 1,
      .show_ascii = false,
      .show_header = false,
      .prefix_mode = FormattedHexDumper::AddressMode::kDisabled};
};

// On platforms where uintptr_t is 32-bit this evaluates to a 10-byte string
// where hex_string is prefixed with "0x". On 64-bit targets, this expands to
// an 18-byte string with the significant bytes are zero padded.
#define EXPECTED_SIGNIFICANT_BYTES(hex_string)                    \
  sizeof(uintptr_t) == sizeof(uint64_t) ? "0x00000000" hex_string \
                                        : "0x" hex_string

TEST_F(HexDump, DumpAddr_ZeroSizeT) {
  constexpr const char* expected = EXPECTED_SIGNIFICANT_BYTES("00000000");
  size_t zero = 0;
  EXPECT_EQ(DumpAddr(dest_, zero), OkStatus());
  EXPECT_STREQ(expected, dest_.data());
}

TEST_F(HexDump, DumpAddr_NonzeroSizeT) {
  constexpr const char* expected = EXPECTED_SIGNIFICANT_BYTES("deadbeef");
  size_t nonzero = 0xDEADBEEF;
  EXPECT_TRUE(DumpAddr(dest_, nonzero).ok());
  EXPECT_STREQ(expected, dest_.data());
}

TEST_F(HexDump, DumpAddr_ZeroPtr) {
  constexpr const char* expected = EXPECTED_SIGNIFICANT_BYTES("00000000");
  uintptr_t zero = 0;
  EXPECT_TRUE(DumpAddr(dest_, reinterpret_cast<const void*>(zero)).ok());
  EXPECT_STREQ(expected, dest_.data());
}

TEST_F(HexDump, DumpAddr_NonzeroPtr) {
  constexpr const char* expected = EXPECTED_SIGNIFICANT_BYTES("deadbeef");
  uintptr_t nonzero = 0xDEADBEEF;
  EXPECT_TRUE(DumpAddr(dest_, reinterpret_cast<const void*>(nonzero)).ok());
  EXPECT_STREQ(expected, dest_.data());
}

TEST_F(HexDump, FormattedHexDump_Defaults) {
  constexpr const char* expected =
      "a4 cc 32 62 9b 46 38 1a 23 1a 2a 7a bc e2 40 a0  ..2b.F8.#.*z..@.";
  default_flags_.show_ascii = true;
  dumper_ = FormattedHexDumper(dest_, default_flags_);
  EXPECT_TRUE(dumper_.BeginDump(source_data).ok());
  EXPECT_TRUE(dumper_.DumpLine().ok());
  EXPECT_STREQ(expected, dest_.data());
}

TEST_F(HexDump, FormattedHexDump_DefaultHeader) {
  constexpr const char* expected =
      "00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f";

  default_flags_.show_header = true;
  dumper_ = FormattedHexDumper(dest_, default_flags_);
  EXPECT_TRUE(dumper_.BeginDump(source_data).ok());
  EXPECT_TRUE(dumper_.DumpLine().ok());
  EXPECT_STREQ(expected, dest_.data());
}

TEST_F(HexDump, FormattedHexDump_DumpEntireBuffer) {
  constexpr size_t kTestBytesPerLine = 8;

  default_flags_.bytes_per_line = kTestBytesPerLine;
  dumper_ = FormattedHexDumper(dest_, default_flags_);

  EXPECT_TRUE(dumper_.BeginDump(source_data).ok());
  for (size_t i = 0; i < source_data.size(); i += kTestBytesPerLine) {
    EXPECT_TRUE(dumper_.DumpLine().ok());
  }
  EXPECT_EQ(dumper_.DumpLine(), Status::ResourceExhausted());
}

// This test is provided for convenience of debugging, as it actually logs the
// dump.
TEST_F(HexDump, FormattedHexDump_LogDump) {
  default_flags_.show_ascii = true;
  default_flags_.show_header = true;
  default_flags_.prefix_mode = FormattedHexDumper::AddressMode::kOffset;
  dumper_ = FormattedHexDumper(dest_, default_flags_);

  EXPECT_TRUE(dumper_.BeginDump(source_data).ok());
  // Dump data.
  while (dumper_.DumpLine().ok()) {
    PW_LOG_INFO("%s", dest_.data());
  }
  EXPECT_EQ(dumper_.DumpLine(), Status::ResourceExhausted());
}

TEST_F(HexDump, FormattedHexDump_NoSpaces) {
  constexpr const char* expected = "a4cc32629b46381a231a2a7abce240a0";

  default_flags_.group_every = 0;
  dumper_ = FormattedHexDumper(dest_, default_flags_);

  EXPECT_TRUE(dumper_.BeginDump(source_data).ok());
  EXPECT_TRUE(dumper_.DumpLine().ok());
  EXPECT_STREQ(expected, dest_.data());
}

TEST_F(HexDump, FormattedHexDump_SetGroupEveryByte) {
  constexpr const char* expected =
      "a4 cc 32 62 9b 46 38 1a 23 1a 2a 7a bc e2 40 a0";
  default_flags_.group_every = 1;
  dumper_ = FormattedHexDumper(dest_, default_flags_);
  EXPECT_TRUE(dumper_.BeginDump(source_data).ok());
  EXPECT_TRUE(dumper_.DumpLine().ok());
  EXPECT_STREQ(expected, dest_.data());
}

TEST_F(HexDump, FormattedHexDump_SetGroupEveryThreeBytes) {
  constexpr const char* expected = "a4cc32 629b46 381a23 1a2a7a bce240 a0";

  default_flags_.group_every = 3;
  dumper_ = FormattedHexDumper(dest_, default_flags_);

  EXPECT_TRUE(dumper_.BeginDump(source_data).ok());
  EXPECT_TRUE(dumper_.DumpLine().ok());
  EXPECT_STREQ(expected, dest_.data());
}

TEST_F(HexDump, FormattedHexDump_TwoLines) {
  constexpr const char* expected1 = "a4 cc 32 62 9b 46 38 1a";
  constexpr const char* expected2 = "23 1a 2a 7a bc e2 40 a0";

  default_flags_.bytes_per_line = 8;
  dumper_ = FormattedHexDumper(dest_, default_flags_);

  EXPECT_TRUE(dumper_.BeginDump(source_data).ok());
  // Dump first line.
  EXPECT_TRUE(dumper_.DumpLine().ok());
  EXPECT_STREQ(expected1, dest_.data());
  // Dump second line.
  EXPECT_TRUE(dumper_.DumpLine().ok());
  EXPECT_STREQ(expected2, dest_.data());
}

TEST_F(HexDump, FormattedHexDump_LastLineCheck) {
  constexpr const char* expected1 = "a4cc32629b46381a 231a2a7abce240a0";
  constexpr const char* expected2 = "ff33e52b9e9f6b3c be9b893c7e4a7a48";
  constexpr const char* expected3 = "18";

  default_flags_.bytes_per_line = 16;
  default_flags_.group_every = 8;
  dumper_ = FormattedHexDumper(dest_, default_flags_);

  EXPECT_TRUE(dumper_.BeginDump(source_data).ok());
  // Dump first line.
  EXPECT_TRUE(dumper_.DumpLine().ok());
  EXPECT_STREQ(expected1, dest_.data());
  // Dump second line.
  EXPECT_TRUE(dumper_.DumpLine().ok());
  EXPECT_STREQ(expected2, dest_.data());
  // Dump third line.
  EXPECT_TRUE(dumper_.DumpLine().ok());
  EXPECT_STREQ(expected3, dest_.data());
}

TEST_F(HexDump, FormattedHexDump_Ascii) {
  constexpr const char* expected1 = "6d 79 20 74 65 73 74 20  my test ";
  constexpr const char* expected2 = "73 74 72 69 6e 67 0a     string.";

  default_flags_.bytes_per_line = 8;
  default_flags_.show_ascii = true;
  dumper_ = FormattedHexDumper(dest_, default_flags_);

  EXPECT_TRUE(dumper_.BeginDump(short_string).ok());
  // Dump first line.
  EXPECT_TRUE(dumper_.DumpLine().ok());
  EXPECT_STREQ(expected1, dest_.data());
  // Dump second line.
  EXPECT_TRUE(dumper_.DumpLine().ok());
  EXPECT_STREQ(expected2, dest_.data());
}

TEST_F(HexDump, FormattedHexDump_AsciiHeader) {
  constexpr const char* expected0 = "00       04        Text";
  constexpr const char* expected1 = "6d792074 65737420  my test ";
  constexpr const char* expected2 = "73747269 6e670a    string.";

  default_flags_.bytes_per_line = 8;
  default_flags_.group_every = 4;
  default_flags_.show_ascii = true;
  default_flags_.show_header = true;
  dumper_ = FormattedHexDumper(dest_, default_flags_);

  EXPECT_TRUE(dumper_.BeginDump(short_string).ok());
  // Dump header.
  EXPECT_TRUE(dumper_.DumpLine().ok());
  EXPECT_STREQ(expected0, dest_.data());
  // Dump first line.
  EXPECT_TRUE(dumper_.DumpLine().ok());
  EXPECT_STREQ(expected1, dest_.data());
  // Dump second line.
  EXPECT_TRUE(dumper_.DumpLine().ok());
  EXPECT_STREQ(expected2, dest_.data());
}

TEST_F(HexDump, FormattedHexDump_AsciiHeaderGroupEvery) {
  constexpr const char* expected0 = "00 01 02 03 04 05 06 07  Text";
  constexpr const char* expected1 = "6d 79 20 74 65 73 74 20  my test ";
  constexpr const char* expected2 = "73 74 72 69 6e 67 0a     string.";

  default_flags_.bytes_per_line = 8;
  default_flags_.group_every = 1;
  default_flags_.show_ascii = true;
  default_flags_.show_header = true;
  dumper_ = FormattedHexDumper(dest_, default_flags_);

  EXPECT_TRUE(dumper_.BeginDump(short_string).ok());
  // Dump header.
  EXPECT_TRUE(dumper_.DumpLine().ok());
  EXPECT_STREQ(expected0, dest_.data());
  // Dump first line.
  EXPECT_TRUE(dumper_.DumpLine().ok());
  EXPECT_STREQ(expected1, dest_.data());
  // Dump second line.
  EXPECT_TRUE(dumper_.DumpLine().ok());
  EXPECT_STREQ(expected2, dest_.data());
}

TEST_F(HexDump, FormattedHexDump_OffsetPrefix) {
  constexpr const char* expected1 = "0000:";
  constexpr const char* expected2 = "0010:";

  default_flags_.bytes_per_line = 16;
  default_flags_.prefix_mode = FormattedHexDumper::AddressMode::kOffset;
  dumper_ = FormattedHexDumper(dest_, default_flags_);

  EXPECT_TRUE(dumper_.BeginDump(source_data).ok());
  // Dump first line.
  EXPECT_TRUE(dumper_.DumpLine().ok());
  // Truncate string to only contain the offset.
  dest_[strlen(expected1)] = '\0';
  EXPECT_STREQ(expected1, dest_.data());

  // Dump second line.
  EXPECT_TRUE(dumper_.DumpLine().ok());
  // Truncate string to only contain the offset.
  dest_[strlen(expected2)] = '\0';
  EXPECT_STREQ(expected2, dest_.data());
}

TEST_F(HexDump, FormattedHexDump_OffsetPrefix_ShortLine) {
  constexpr const char* expected = "0000:";

  default_flags_.bytes_per_line = 16;
  default_flags_.prefix_mode = FormattedHexDumper::AddressMode::kOffset;
  dumper_ = FormattedHexDumper(dest_, default_flags_);

  EXPECT_TRUE(dumper_.BeginDump(pw::span(source_data).first(8)).ok());
  // Dump first and only line.
  EXPECT_TRUE(dumper_.DumpLine().ok());
  // Truncate string to only contain the offset.
  dest_[strlen(expected)] = '\0';
  EXPECT_STREQ(expected, dest_.data());
}

TEST_F(HexDump, FormattedHexDump_OffsetPrefix_LongData) {
  constexpr std::array<std::byte, 300> long_data = {std::byte{0xff}};

  constexpr const char* expected1 = "0000:";
  constexpr const char* expected2 = "0010:";

  default_flags_.bytes_per_line = 16;
  default_flags_.prefix_mode = FormattedHexDumper::AddressMode::kOffset;
  dumper_ = FormattedHexDumper(dest_, default_flags_);

  EXPECT_TRUE(dumper_.BeginDump(long_data).ok());
  // Dump first line.
  EXPECT_TRUE(dumper_.DumpLine().ok());
  // Truncate string to only contain the offset.
  dest_[strlen(expected1)] = '\0';
  EXPECT_STREQ(expected1, dest_.data());

  // Dump second line.
  EXPECT_TRUE(dumper_.DumpLine().ok());
  // Truncate string to only contain the offset.
  dest_[strlen(expected2)] = '\0';
  EXPECT_STREQ(expected2, dest_.data());
}

TEST_F(HexDump, FormattedHexDump_AbsolutePrefix) {
  constexpr size_t kTestBytesPerLine = 16;
  std::array<char, kHexAddrStringSize + 1> expected1;
  std::array<char, kHexAddrStringSize + 1> expected2;
  ASSERT_EQ(OkStatus(), DumpAddr(expected1, source_data.data()));
  ASSERT_EQ(OkStatus(),
            DumpAddr(expected2, source_data.data() + kTestBytesPerLine));

  default_flags_.bytes_per_line = kTestBytesPerLine;
  default_flags_.prefix_mode = FormattedHexDumper::AddressMode::kAbsolute;
  dumper_ = FormattedHexDumper(dest_, default_flags_);

  EXPECT_TRUE(dumper_.BeginDump(source_data).ok());
  // Dump first line.
  EXPECT_TRUE(dumper_.DumpLine().ok());
  // Truncate string to only contain the offset.
  EXPECT_EQ(dest_[kHexAddrStringSize], ':');
  dest_[kHexAddrStringSize] = '\0';
  EXPECT_STREQ(expected1.data(), dest_.data());

  // Dump second line.
  EXPECT_TRUE(dumper_.DumpLine().ok());
  // Truncate string to only contain the offset.
  EXPECT_EQ(dest_[kHexAddrStringSize], ':');
  dest_[kHexAddrStringSize] = '\0';
  EXPECT_STREQ(expected2.data(), dest_.data());
}

TEST_F(SmallBuffer, TinyHexDump) {
  constexpr const char* expected = "a4cc32";

  default_flags_.bytes_per_line = 3;
  default_flags_.group_every = 4;
  dumper_ = FormattedHexDumper(dest_, default_flags_);

  EXPECT_TRUE(dumper_.BeginDump(source_data).ok());
  EXPECT_TRUE(dumper_.DumpLine().ok());
  EXPECT_STREQ(expected, dest_.data());
}

TEST_F(SmallBuffer, TooManyBytesPerLine) {
  constexpr const char* expected = "";

  default_flags_.bytes_per_line = 13;
  dumper_ = FormattedHexDumper(dest_, default_flags_);

  EXPECT_EQ(dumper_.BeginDump(source_data), Status::FailedPrecondition());
  EXPECT_FALSE(dumper_.DumpLine().ok());
  EXPECT_STREQ(expected, dest_.data());
}

TEST_F(SmallBuffer, SpacesIncreaseBufferRequirement) {
  constexpr const char* expected = "";

  default_flags_.bytes_per_line = 3;
  default_flags_.group_every = 1;
  dumper_ = FormattedHexDumper(dest_, default_flags_);

  EXPECT_EQ(dumper_.BeginDump(source_data), Status::FailedPrecondition());
  EXPECT_FALSE(dumper_.DumpLine().ok());
  EXPECT_STREQ(expected, dest_.data());
}

TEST_F(SmallBuffer, PrefixIncreasesBufferRequirement) {
  constexpr const char* expected = "";

  default_flags_.bytes_per_line = 3;
  default_flags_.prefix_mode = FormattedHexDumper::AddressMode::kOffset;
  dumper_ = FormattedHexDumper(dest_, default_flags_);

  EXPECT_EQ(dumper_.BeginDump(source_data), Status::FailedPrecondition());
  EXPECT_FALSE(dumper_.DumpLine().ok());
  EXPECT_STREQ(expected, dest_.data());
}

TEST(BadBuffer, ZeroSize) {
  char buffer[1] = {static_cast<char>(0xaf)};
  FormattedHexDumper dumper(span<char>(buffer, 0));
  EXPECT_EQ(dumper.BeginDump(source_data), Status::FailedPrecondition());
  EXPECT_EQ(dumper.DumpLine(), Status::FailedPrecondition());
  EXPECT_EQ(buffer[0], static_cast<char>(0xaf));
}

TEST(BadBuffer, NullPtrDest) {
  FormattedHexDumper dumper;
  EXPECT_EQ(dumper.SetLineBuffer(span<char>()), Status::InvalidArgument());
  EXPECT_EQ(dumper.BeginDump(source_data), Status::FailedPrecondition());
  EXPECT_EQ(dumper.DumpLine(), Status::FailedPrecondition());
}

TEST(BadBuffer, NullPtrSrc) {
  char buffer[24] = {static_cast<char>(0)};
  FormattedHexDumper dumper(buffer);
  EXPECT_EQ(dumper.BeginDump(ByteSpan(static_cast<std::byte*>(nullptr), 64)),
            Status::InvalidArgument());
  // Don't actually dump nullptr in this test as it could cause a crash.
}

}  // namespace
}  // namespace pw::dump
