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

#include "pw_tokenizer/token_database.h"

#include <cstring>
#include <string>
#include <string_view>

#include "gtest/gtest.h"

namespace pw::tokenizer {
namespace {

using namespace std::literals::string_view_literals;

// Use alignas to ensure that the data is properly aligned for database entries.
// This avoids unaligned memory reads.
alignas(TokenDatabase::RawEntry) constexpr char kBasicData[] =
    "TOKENS\0\0\x03\x00\x00\x00\0\0\0\0"
    "\x01\0\0\0\0\0\0\0"
    "\x02\0\0\0\0\0\0\0"
    "\xFF\0\0\0\0\0\0\0"
    "hi!\0"
    "goodbye\0"
    ":)";

alignas(TokenDatabase::RawEntry) constexpr char kEmptyData[] =
    "TOKENS\0\0\x00\x00\x00\x00\0\0\0";  // Last byte is null terminator.

alignas(TokenDatabase::RawEntry) constexpr char kBadMagic[] =
    "TOKENs\0\0\x03\x00\x00\x00\0\0\0\0"
    "\x01\0\0\0\0\0\0\0"
    "hi!\0";

alignas(TokenDatabase::RawEntry) constexpr char kBadVersion[] =
    "TOKENS\0\1\x00\0\0\0\0\0\0\0";

alignas(TokenDatabase::RawEntry) constexpr char kBadEntryCount[] =
    "TOKENS\0\0\xff\x00\x00\x00\0\0\0\0";

// Use signed data and a size with the top bit set to test that the entry count
// is read correctly, without per-byte sign extension.
alignas(TokenDatabase::RawEntry) constexpr signed char kSignedWithTopBit[] =
    "TOKENS\0\0\x80\x00\x00\x00\0\0\0\0"
    // Entries
    "TOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdate"
    "TOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdate"
    "TOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdate"
    "TOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdate"  // 32
    "TOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdate"
    "TOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdate"
    "TOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdate"
    "TOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdate"  // 64
    "TOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdate"
    "TOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdate"
    "TOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdate"
    "TOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdate"  // 96
    "TOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdate"
    "TOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdate"
    "TOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdate"
    "TOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdateTOKNdate"  // 128
    // Strings (empty)
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"  //  32
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"  //  64
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"  //  96
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";   // 128

constexpr TokenDatabase kBasicDatabase = TokenDatabase::Create<kBasicData>();
static_assert(kBasicDatabase.size() == 3u);

TEST(TokenDatabase, EntryCount) {
  static_assert(TokenDatabase::Create<kBasicData>().size() == 3u);
  static_assert(TokenDatabase::Create(kEmptyData).size() == 0u);
  EXPECT_EQ(TokenDatabase::Create<kSignedWithTopBit>().size(), 0x80u);
}

TEST(TokenDatabase, ValidCheck) {
  char basic_data[sizeof(kBasicData)];
  std::memcpy(basic_data, kBasicData, sizeof(basic_data));
  EXPECT_TRUE(TokenDatabase::IsValid(basic_data));

  static_assert(TokenDatabase::IsValid(kBasicData));
  static_assert(TokenDatabase::IsValid(kEmptyData));
  static_assert(TokenDatabase::IsValid(kSignedWithTopBit));

  static_assert(!TokenDatabase::IsValid(kBadMagic));
  static_assert(!TokenDatabase::IsValid(kBadVersion));
  static_assert(!TokenDatabase::IsValid(kBadEntryCount));

  static_assert(!TokenDatabase::IsValid("TOKENS\0\0\0\0"));  // too short
  static_assert(!TokenDatabase::IsValid("TOKENS\0\1\0\0\0\0\0\0\0\0"));
  static_assert(!TokenDatabase::IsValid("TOKENSv0\0\0\0\0\0\0\0\0"));
  static_assert(!TokenDatabase::IsValid("tokens\0\0\0\0\0\0\0\0\0\0"));

  // No string table; this is one byte too short.
  static_assert(
      !TokenDatabase::IsValid("TOKENS\0\0\x01\x00\x00\x00\0\0\0\0WXYZdate"sv));

  // Add one byte for the string table.
  static_assert(
      TokenDatabase::IsValid("TOKENS\0\0\x01\x00\x00\x00\0\0\0\0WXYZdate\0"sv));

  static_assert(
      !TokenDatabase::IsValid("TOKENS\0\0\x02\x00\x00\x00\0\0\0\0"
                              "WXYZdate"
                              "WXYZdate"
                              "\0"sv));
  static_assert(
      TokenDatabase::IsValid("TOKENS\0\0\x02\x00\x00\x00\0\0\0\0"
                             "WXYZdate"
                             "WXYZdate"
                             "hi\0\0"sv));
  static_assert(
      TokenDatabase::IsValid("TOKENS\0\0\x02\x00\x00\x00\0\0\0\0"
                             "WXYZdate"
                             "WXYZdate"
                             "hi\0hello\0"sv));
}

TEST(TokenDatabase, Iterator) {
  auto it = kBasicDatabase.begin();
  EXPECT_EQ(it->token, 1u);
  EXPECT_STREQ(it.entry().string, "hi!");

  ++it;
  EXPECT_EQ(it->token, 2u);
  EXPECT_STREQ(it.entry().string, "goodbye");
  EXPECT_EQ(it - kBasicDatabase.begin(), 1);

  ++it;
  EXPECT_EQ(it->token, 0xFFu);
  EXPECT_STREQ(it.entry().string, ":)");
  EXPECT_EQ(it - kBasicDatabase.begin(), 2);

  ++it;
  EXPECT_EQ(it, kBasicDatabase.end());
  EXPECT_EQ(static_cast<size_t>(it - kBasicDatabase.begin()),
            kBasicDatabase.size());
}

TEST(TokenDatabase, Iterator_PreIncrement) {
  auto it = kBasicDatabase.begin();
  EXPECT_EQ((++it)->token, 2u);
  EXPECT_STREQ(it.entry().string, "goodbye");
}

TEST(TokenDatabase, Iterator_PostIncrement) {
  auto it = kBasicDatabase.begin();
  EXPECT_EQ((it++)->token, 1u);

  EXPECT_EQ(it->token, 2u);
  EXPECT_STREQ(it.entry().string, "goodbye");
}

TEST(TokenDatabase, SingleEntryLookup_FirstEntry) {
  auto match = kBasicDatabase.Find(1);
  ASSERT_EQ(match.size(), 1u);
  EXPECT_FALSE(match.empty());
  EXPECT_STREQ(match[0].string, "hi!");

  for (const auto& entry : match) {
    EXPECT_EQ(entry.token, 1u);
    EXPECT_STREQ(entry.string, "hi!");
  }
}

TEST(TokenDatabase, SingleEntryLookup_MiddleEntry) {
  auto match = kBasicDatabase.Find(2);
  ASSERT_EQ(match.size(), 1u);
  EXPECT_FALSE(match.empty());
  EXPECT_STREQ(match[0].string, "goodbye");
}

TEST(TokenDatabase, SingleEntryLookup_LastEntry) {
  auto match = kBasicDatabase.Find(0xff);
  ASSERT_EQ(match.size(), 1u);
  EXPECT_STREQ(match[0].string, ":)");
  EXPECT_FALSE(match.empty());
}

TEST(TokenDatabase, SingleEntryLookup_NonPresent) {
  EXPECT_TRUE(kBasicDatabase.Find(0).empty());
  EXPECT_TRUE(kBasicDatabase.Find(3).empty());
  EXPECT_TRUE(kBasicDatabase.Find(10239).empty());
  EXPECT_TRUE(kBasicDatabase.Find(0xFFFFFFFFu).empty());
}

TEST(TokenDatabase, SingleEntryLookup_NoMatches) {
  // Can also create the database at runtime.
  TokenDatabase tokens = TokenDatabase::Create(kBasicData);
  const auto match = tokens.Find(42);
  ASSERT_EQ(match.size(), 0u);
  EXPECT_TRUE(match.empty());

  for (const auto& entry : match) {
    FAIL();  // There were no matches, so this code should never execute.
    static_cast<void>(entry);
  }
}

alignas(TokenDatabase::RawEntry) constexpr char kCollisionsData[] =
    "TOKENS\0\0\x05\0\0\0\0\0\0\0"
    "\x01\0\0\0date"
    "\x01\0\0\0date"
    "\x01\0\0\0date"
    "\x02\0\0\0date"
    "\xFF\0\0\0date"
    "hi!\0goodbye\0:)\0\0";

constexpr TokenDatabase kCollisions = TokenDatabase::Create<kCollisionsData>();
static_assert(kCollisions.size() == 5u);

TEST(TokenDatabase, MultipleEntriesWithSameToken) {
  TokenDatabase::Entries match = kCollisions.Find(1);

  EXPECT_EQ(match.begin()->token, 1u);
  EXPECT_EQ(match.end()->token, 2u);
  ASSERT_EQ(match.size(), 3u);

  EXPECT_STREQ(match[0].string, "hi!");
  EXPECT_STREQ(match[1].string, "goodbye");
  EXPECT_STREQ(match[2].string, ":)");

  for (const auto& entry : match) {
    EXPECT_EQ(entry.token, 1u);
  }
}

TEST(TokenDatabase, Empty) {
  constexpr TokenDatabase empty_db = TokenDatabase::Create<kEmptyData>();
  static_assert(empty_db.size() == 0u);
  static_assert(empty_db.ok());

  EXPECT_TRUE(empty_db.Find(0).empty());
  EXPECT_TRUE(empty_db.Find(123).empty());

  for (const auto& entry : empty_db) {
    FAIL();  // The database is empty; this should never execute.
    static_cast<void>(entry);
  }
}

TEST(TokenDatabase, NullDatabase) {
  constexpr TokenDatabase empty_db;

  static_assert(empty_db.size() == 0u);
  static_assert(!empty_db.ok());
  EXPECT_TRUE(empty_db.Find(0).empty());
}

TEST(TokenDatabase, InvalidData) {
  constexpr TokenDatabase bad_db = TokenDatabase::Create("TOKENS\0\0");

  static_assert(!bad_db.ok());
  EXPECT_TRUE(bad_db.Find(0).empty());
}

TEST(TokenDatabase, FromString) {
  TokenDatabase bad_db = TokenDatabase::Create(std::string("wow!"));

  EXPECT_FALSE(bad_db.ok());
}

}  // namespace
}  // namespace pw::tokenizer
