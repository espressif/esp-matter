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

#include "pw_tokenizer/base64.h"

#include <cstring>
#include <string_view>

#include "gtest/gtest.h"
#include "pw_span/span.h"

namespace pw::tokenizer {
namespace {

using std::byte;

class PrefixedBase64 : public ::testing::Test {
 protected:
  static constexpr char kUnset = '#';

  PrefixedBase64() {
    std::memset(binary_, kUnset, sizeof(binary_));
    std::memset(base64_, kUnset, sizeof(base64_));
  }

  byte binary_[32];
  char base64_[32];
};

const struct TestData {
  template <size_t kSize>
  TestData(const char (&binary_data)[kSize], const char* base64_data)
      : binary{as_bytes(span(binary_data, kSize - 1))}, base64(base64_data) {}

  span<const byte> binary;
  std::string_view base64;
} kTestData[] = {
    {"", "$"},
    {"\x00", "$AA=="},
    {"\x71", "$cQ=="},
    {"\xff", "$/w=="},
    {"\x63\xa9", "$Y6k="},
    {"\x69\x89\x03", "$aYkD"},
    {"\x80\xf5\xc8\xd4", "$gPXI1A=="},
    {"\x6e\xb8\x91\x3f\xac", "$briRP6w="},
    {"\x1f\x88\x91\xbb\xd7\x10", "$H4iRu9cQ"},
    {"\xac\xcf\xb2\xd5\xee\xa2\x8e", "$rM+y1e6ijg=="},
    {"\xff\x15\x25\x7e\x7b\xc9\x7b\x60", "$/xUlfnvJe2A="},
    {"\xd5\xab\xd9\xa6\xae\xaa\x33\x9f\x66", "$1avZpq6qM59m"},
    {"\x6b\xfd\x95\xc5\x4a\xc7\xc2\x39\x45\xdc", "$a/2VxUrHwjlF3A=="},
    {"\x4c\xde\xee\xb8\x68\x0d\x9c\x66\x3e\xea\x46", "$TN7uuGgNnGY+6kY="},
};

TEST_F(PrefixedBase64, Encode) {
  for (auto& [binary, base64] : kTestData) {
    EXPECT_EQ(base64.size(), PrefixedBase64Encode(binary, base64_));
    ASSERT_EQ(base64, base64_);
    EXPECT_EQ('\0', base64_[base64.size()]);
  }
}

TEST_F(PrefixedBase64, Encode_EmptyInput_WritesPrefix) {
  EXPECT_EQ(1u, PrefixedBase64Encode(span<byte>(), base64_));
  EXPECT_EQ('$', base64_[0]);
  EXPECT_EQ('\0', base64_[1]);
}

TEST_F(PrefixedBase64, Encode_EmptyOutput_WritesNothing) {
  EXPECT_EQ(0u, PrefixedBase64Encode(kTestData[5].binary, span(base64_, 0)));
  EXPECT_EQ(kUnset, base64_[0]);
}

TEST_F(PrefixedBase64, Encode_SingleByteOutput_OnlyNullTerminates) {
  EXPECT_EQ(0u, PrefixedBase64Encode(kTestData[5].binary, span(base64_, 1)));
  EXPECT_EQ('\0', base64_[0]);
  EXPECT_EQ(kUnset, base64_[1]);
}

TEST_F(PrefixedBase64, Encode_NoRoomForNullAfterMessage_OnlyNullTerminates) {
  EXPECT_EQ(0u,
            PrefixedBase64Encode(kTestData[5].binary,
                                 span(base64_, kTestData[5].base64.size())));
  EXPECT_EQ('\0', base64_[0]);
  EXPECT_EQ(kUnset, base64_[1]);
}

TEST_F(PrefixedBase64, Encode_InlineString) {
  for (auto& [binary, base64] : kTestData) {
    EXPECT_EQ(base64, PrefixedBase64Encode(binary));
  }
}

TEST_F(PrefixedBase64, Encode_InlineString_Append) {
  for (auto& [binary, base64] : kTestData) {
    pw::InlineString<32> string("Other stuff!");
    PrefixedBase64Encode(binary, string);

    pw::InlineString<32> expected("Other stuff!");
    expected.append(base64);

    EXPECT_EQ(expected, string);
  }
}

TEST_F(PrefixedBase64, Base64EncodedBufferSize_Empty_RoomForPrefixAndNull) {
  EXPECT_EQ(2u, Base64EncodedBufferSize(0));
}

TEST_F(PrefixedBase64, Base64EncodedBufferSize_PositiveSizes) {
  for (unsigned i = 1; i <= 3; ++i) {
    EXPECT_EQ(6u, Base64EncodedBufferSize(i));
  }
  for (unsigned i = 4; i <= 6; ++i) {
    EXPECT_EQ(10u, Base64EncodedBufferSize(i));
  }
}

TEST_F(PrefixedBase64, Decode) {
  for (auto& [binary, base64] : kTestData) {
    EXPECT_EQ(binary.size(), PrefixedBase64Decode(base64, binary_));
    ASSERT_EQ(0, std::memcmp(binary.data(), binary_, binary.size()));
  }
}

TEST_F(PrefixedBase64, Decode_EmptyInput_WritesNothing) {
  EXPECT_EQ(0u, PrefixedBase64Decode({}, binary_));
  EXPECT_EQ(byte{kUnset}, binary_[0]);
}

TEST_F(PrefixedBase64, Decode_OnlyPrefix_WritesNothing) {
  EXPECT_EQ(0u, PrefixedBase64Decode("$", binary_));
  EXPECT_EQ(byte{kUnset}, binary_[0]);
}

TEST_F(PrefixedBase64, Decode_EmptyOutput_WritesNothing) {
  EXPECT_EQ(0u, PrefixedBase64Decode(kTestData[5].base64, span(binary_, 0)));
  EXPECT_EQ(byte{kUnset}, binary_[0]);
}

TEST_F(PrefixedBase64, Decode_OutputTooSmall_WritesNothing) {
  auto& item = kTestData[5];
  EXPECT_EQ(
      0u,
      PrefixedBase64Decode(item.base64, span(binary_, item.binary.size() - 1)));
  EXPECT_EQ(byte{kUnset}, binary_[0]);
}

TEST(PrefixedBase64DecodeInPlace, DecodeInPlace) {
  byte buffer[32];

  for (auto& [binary, base64] : kTestData) {
    std::memcpy(buffer, base64.data(), base64.size());

    EXPECT_EQ(binary.size(),
              PrefixedBase64DecodeInPlace(span(buffer, base64.size())));
    ASSERT_EQ(0, std::memcmp(binary.data(), buffer, binary.size()));
  }
}

}  // namespace
}  // namespace pw::tokenizer
