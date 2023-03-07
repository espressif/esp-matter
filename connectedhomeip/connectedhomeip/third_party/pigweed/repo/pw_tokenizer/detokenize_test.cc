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

#include "pw_tokenizer/detokenize.h"

#include <string_view>

#include "gtest/gtest.h"

namespace pw::tokenizer {
namespace {

using namespace std::literals::string_view_literals;

// Use a shorter name for the error string macro.
#define ERR PW_TOKENIZER_ARG_DECODING_ERROR

// Use alignas to ensure that the data is properly aligned to be read from a
// token database entry struct. This avoids unaligned memory reads.
alignas(TokenDatabase::RawEntry) constexpr char kBasicData[] =
    "TOKENS\0\0"
    "\x04\x00\x00\x00"
    "\0\0\0\0"
    "\x01\x00\x00\x00----"
    "\x05\x00\x00\x00----"
    "\xFF\x00\x00\x00----"
    "\xFF\xEE\xEE\xDD----"
    "One\0"
    "TWO\0"
    "333\0"
    "FOUR";

class Detokenize : public ::testing::Test {
 protected:
  Detokenize() : detok_(TokenDatabase::Create<kBasicData>()) {}
  Detokenizer detok_;
};

TEST_F(Detokenize, NoFormatting) {
  EXPECT_EQ(detok_.Detokenize("\1\0\0\0"sv).BestString(), "One");
  EXPECT_EQ(detok_.Detokenize("\5\0\0\0"sv).BestString(), "TWO");
  EXPECT_EQ(detok_.Detokenize("\xff\x00\x00\x00"sv).BestString(), "333");
  EXPECT_EQ(detok_.Detokenize("\xff\xee\xee\xdd"sv).BestString(), "FOUR");
}

TEST_F(Detokenize, BestString_MissingToken_IsEmpty) {
  EXPECT_FALSE(detok_.Detokenize("").ok());
  EXPECT_TRUE(detok_.Detokenize("", 0u).BestString().empty());
}

TEST_F(Detokenize, BestString_ShorterToken_ZeroExtended) {
  EXPECT_EQ(detok_.Detokenize("\x42", 1u).token(), 0x42u);
  EXPECT_EQ(detok_.Detokenize("\1\0"sv).token(), 0x1u);
  EXPECT_EQ(detok_.Detokenize("\1\0\3"sv).token(), 0x030001u);
  EXPECT_EQ(detok_.Detokenize("\0\0\0"sv).token(), 0x0u);
}

TEST_F(Detokenize, BestString_UnknownToken_IsEmpty) {
  EXPECT_FALSE(detok_.Detokenize("\0\0\0\0"sv).ok());
  EXPECT_TRUE(detok_.Detokenize("\0\0\0\0"sv).BestString().empty());
  EXPECT_TRUE(detok_.Detokenize("\2\0\0\0"sv).BestString().empty());
  EXPECT_TRUE(detok_.Detokenize("\x10\x32\x54\x76\x99"sv).BestString().empty());
  EXPECT_TRUE(detok_.Detokenize("\x98\xba\xdc\xfe"sv).BestString().empty());
}

TEST_F(Detokenize, BestStringWithErrors_MissingToken_ErrorMessage) {
  EXPECT_FALSE(detok_.Detokenize("").ok());
  EXPECT_EQ(detok_.Detokenize("", 0u).BestStringWithErrors(),
            ERR("missing token"));
}

TEST_F(Detokenize, BestStringWithErrors_ShorterTokenMatchesStrings) {
  EXPECT_EQ(detok_.Detokenize("\1", 1u).BestStringWithErrors(), "One");
  EXPECT_EQ(detok_.Detokenize("\1\0"sv).BestStringWithErrors(), "One");
  EXPECT_EQ(detok_.Detokenize("\1\0\0"sv).BestStringWithErrors(), "One");
}

TEST_F(Detokenize, BestStringWithErrors_UnknownToken_ErrorMessage) {
  ASSERT_FALSE(detok_.Detokenize("\0\0\0\0"sv).ok());
  EXPECT_EQ(detok_.Detokenize("\0"sv).BestStringWithErrors(),
            ERR("unknown token 00000000"));
  EXPECT_EQ(detok_.Detokenize("\0\0\0"sv).BestStringWithErrors(),
            ERR("unknown token 00000000"));
  EXPECT_EQ(detok_.Detokenize("\0\0\0\0"sv).BestStringWithErrors(),
            ERR("unknown token 00000000"));
  EXPECT_EQ(detok_.Detokenize("\2\0\0\0"sv).BestStringWithErrors(),
            ERR("unknown token 00000002"));
  EXPECT_EQ(detok_.Detokenize("\x10\x32\x54\x76\x99"sv).BestStringWithErrors(),
            ERR("unknown token 76543210"));
  EXPECT_EQ(detok_.Detokenize("\x98\xba\xdc\xfe"sv).BestStringWithErrors(),
            ERR("unknown token fedcba98"));
}

alignas(TokenDatabase::RawEntry) constexpr char kDataWithArguments[] =
    "TOKENS\0\0"
    "\x09\x00\x00\x00"
    "\0\0\0\0"
    "\x00\x00\x00\x00----"
    "\x0A\x0B\x0C\x0D----"
    "\x0E\x0F\x00\x01----"
    "\xAA\xAA\xAA\xAA----"
    "\xBB\xBB\xBB\xBB----"
    "\xCC\xCC\xCC\xCC----"
    "\xDD\xDD\xDD\xDD----"
    "\xEE\xEE\xEE\xEE----"
    "\xFF\xFF\xFF\xFF----"
    "\0"
    "Use the %s, %s.\0"
    "Now there are %d of %s!\0"
    "%c!\0"    // AA
    "%hhu!\0"  // BB
    "%hu!\0"   // CC
    "%u!\0"    // DD
    "%lu!\0"   // EE
    "%llu!";   // FF

constexpr TokenDatabase kWithArgs = TokenDatabase::Create<kDataWithArguments>();

using Case = std::pair<std::string_view, std::string_view>;

template <typename... Args>
auto TestCases(Args... args) {
  return std::array<Case, sizeof...(Args)>{args...};
}

class DetokenizeWithArgs : public ::testing::Test {
 protected:
  DetokenizeWithArgs() : detok_(kWithArgs) {}

  Detokenizer detok_;
};

TEST_F(DetokenizeWithArgs, NoMatches) {
  EXPECT_TRUE(detok_.Detokenize("\x23\xab\xc9\x87"sv).matches().empty());
}

TEST_F(DetokenizeWithArgs, SingleMatch) {
  EXPECT_EQ(detok_.Detokenize("\x00\x00\x00\x00"sv).matches().size(), 1u);
}

TEST_F(DetokenizeWithArgs, Empty) {
  EXPECT_EQ(detok_.Detokenize("\x00\x00\x00\x00"sv).BestString(), "");
}

TEST_F(DetokenizeWithArgs, Successful) {
  // Run through test cases, but don't include cases that use %hhu or %llu since
  // these are not currently supported in arm-none-eabi-gcc.
  for (auto [data, expected] : TestCases(
           Case{"\x0A\x0B\x0C\x0D\5force\4Luke"sv, "Use the force, Luke."},
           Case{"\x0E\x0F\x00\x01\4\4them"sv, "Now there are 2 of them!"},
           Case{"\xAA\xAA\xAA\xAA\xfc\x01"sv, "~!"},
           Case{"\xCC\xCC\xCC\xCC\xfe\xff\x07"sv, "65535!"},
           Case{"\xDD\xDD\xDD\xDD\xfe\xff\x07"sv, "65535!"},
           Case{"\xDD\xDD\xDD\xDD\xfe\xff\xff\xff\x1f"sv, "4294967295!"},
           Case{"\xEE\xEE\xEE\xEE\xfe\xff\x07"sv, "65535!"},
           Case{"\xEE\xEE\xEE\xEE\xfe\xff\xff\xff\x1f"sv, "4294967295!"})) {
    EXPECT_EQ(detok_.Detokenize(data).BestString(), expected);
  }
}

TEST_F(DetokenizeWithArgs, ExtraDataError) {
  auto error = detok_.Detokenize("\x00\x00\x00\x00MORE data"sv);
  EXPECT_FALSE(error.ok());
  EXPECT_EQ("", error.BestString());
}

TEST_F(DetokenizeWithArgs, MissingArgumentError) {
  auto error = detok_.Detokenize("\x0A\x0B\x0C\x0D\5force"sv);
  EXPECT_FALSE(error.ok());
  EXPECT_EQ(error.BestString(), "Use the force, %s.");
  EXPECT_EQ(error.BestStringWithErrors(),
            "Use the force, " ERR("%s MISSING") ".");
}

TEST_F(DetokenizeWithArgs, DecodingError) {
  auto error = detok_.Detokenize("\x0E\x0F\x00\x01\xFF"sv);
  EXPECT_FALSE(error.ok());
  EXPECT_EQ(error.BestString(), "Now there are %d of %s!");
  EXPECT_EQ(error.BestStringWithErrors(),
            "Now there are " ERR("%d ERROR") " of " ERR("%s SKIPPED") "!");
}

alignas(TokenDatabase::RawEntry) constexpr char kDataWithCollisions[] =
    "TOKENS\0\0"
    "\x0F\x00\x00\x00"
    "\0\0\0\0"
    "\x00\x00\x00\x00\xff\xff\xff\xff"  // 1
    "\x00\x00\x00\x00\x01\x02\x03\x04"  // 2
    "\x00\x00\x00\x00\xff\xff\xff\xff"  // 3
    "\x00\x00\x00\x00\xff\xff\xff\xff"  // 4
    "\x00\x00\x00\x00\xff\xff\xff\xff"  // 5
    "\x00\x00\x00\x00\xff\xff\xff\xff"  // 6
    "\x00\x00\x00\x00\xff\xff\xff\xff"  // 7
    "\xAA\xAA\xAA\xAA\x00\x00\x00\x00"  // 8
    "\xAA\xAA\xAA\xAA\xff\xff\xff\xff"  // 9
    "\xBB\xBB\xBB\xBB\xff\xff\xff\xff"  // A
    "\xBB\xBB\xBB\xBB\xff\xff\xff\xff"  // B
    "\xCC\xCC\xCC\xCC\xff\xff\xff\xff"  // C
    "\xCC\xCC\xCC\xCC\xff\xff\xff\xff"  // D
    "\xDD\xDD\xDD\xDD\xff\xff\xff\xff"  // E
    "\xDD\xDD\xDD\xDD\xff\xff\xff\xff"  // F
    // String table
    "This string is present\0"   // 1
    "This string is removed\0"   // 2
    "One arg %d\0"               // 3
    "One arg %s\0"               // 4
    "Two args %s %u\0"           // 5
    "Two args %s %s %% %% %%\0"  // 6
    "Four args %d %d %d %d\0"    // 7
    "This one is removed\0"      // 8
    "This one is present\0"      // 9
    "Two ints %d %d\0"           // A
    "Three ints %d %d %d\0"      // B
    "Three strings %s %s %s\0"   // C
    "Two strings %s %s\0"        // D
    "Three %s %s %s\0"           // E
    "Five %d %d %d %d %s\0";     // F

constexpr TokenDatabase kWithCollisions =
    TokenDatabase::Create<kDataWithCollisions>();

class DetokenizeWithCollisions : public ::testing::Test {
 protected:
  DetokenizeWithCollisions() : detok_(kWithCollisions) {}

  Detokenizer detok_;
};

TEST_F(DetokenizeWithCollisions, Collision_AlwaysPreferSuccessfulDecode) {
  for (auto [data, expected] :
       TestCases(Case{"\0\0\0\0"sv, "This string is present"},
                 Case{"\0\0\0\0\x01"sv, "One arg -1"},
                 Case{"\0\0\0\0\x80"sv, "One arg [...]"},
                 Case{"\0\0\0\0\4Hey!\x04"sv, "Two args Hey! 2"})) {
    EXPECT_EQ(detok_.Detokenize(data).BestString(), expected);
  }
}

TEST_F(DetokenizeWithCollisions, Collision_PreferDecodingAllBytes) {
  for (auto [data, expected] :
       TestCases(Case{"\0\0\0\0\x80\x80\x80\x80\x00"sv, "Two args [...] 0"},
                 Case{"\0\0\0\0\x08?"sv, "One arg %s"},
                 Case{"\0\0\0\0\x01!\x01\x80"sv, "Two args ! \x80 % % %"})) {
    EXPECT_EQ(detok_.Detokenize(data).BestString(), expected);
  }
}

TEST_F(DetokenizeWithCollisions, Collision_PreferFewestDecodingErrors) {
  for (auto [data, expected] :
       TestCases(Case{"\xBB\xBB\xBB\xBB\x00"sv, "Two ints 0 %d"},
                 Case{"\xCC\xCC\xCC\xCC\2Yo\5?"sv, "Two strings Yo %s"})) {
    EXPECT_EQ(detok_.Detokenize(data).BestString(), expected);
  }
}

TEST_F(DetokenizeWithCollisions, Collision_PreferMostDecodedArgs) {
  auto result = detok_.Detokenize("\xDD\xDD\xDD\xDD\x01\x02\x01\x04\x05"sv);
  EXPECT_EQ((std::string_view)result.matches()[0].value(), "Five -1 1 -1 2 %s");
  EXPECT_EQ((std::string_view)result.matches()[1].value(), "Three \2 \4 %s"sv);
}

TEST_F(DetokenizeWithCollisions, Collision_PreferMostDecodedArgs_NoPercent) {
  // The "Two args %s %s ..." string successfully decodes this, and has more
  // "arguments", because of %%, but %% doesn't count as as a decoded argument.
  EXPECT_EQ(detok_.Detokenize("\0\0\0\0\x01\x00\x01\x02"sv).BestString(),
            "Four args -1 0 -1 1");
}

TEST_F(DetokenizeWithCollisions, Collision_PreferStillPresentString) {
  for (auto [data, expected] :
       TestCases(Case{"\x00\x00\x00\x00"sv, "This string is present"},
                 Case{"\xAA\xAA\xAA\xAA"sv, "This one is present"})) {
    EXPECT_EQ(detok_.Detokenize(data).BestString(), expected);
  }
}

TEST_F(DetokenizeWithCollisions, Collision_TracksAllMatches) {
  auto result = detok_.Detokenize("\0\0\0\0"sv);
  EXPECT_EQ(result.matches().size(), 7u);
}

}  // namespace
}  // namespace pw::tokenizer
