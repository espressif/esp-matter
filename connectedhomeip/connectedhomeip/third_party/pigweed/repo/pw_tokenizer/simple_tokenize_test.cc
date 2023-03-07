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

#include <algorithm>
#include <array>

#include "gtest/gtest.h"
#include "pw_tokenizer/tokenize.h"
#include "pw_tokenizer/tokenize_to_global_handler.h"
#include "pw_tokenizer/tokenize_to_global_handler_with_payload.h"

namespace pw {
namespace tokenizer {
namespace {

template <size_t kSize>
uint32_t TestHash(const char (&str)[kSize])
    PW_NO_SANITIZE("unsigned-integer-overflow") {
  static_assert(kSize > 0u, "Must have at least a null terminator");

  static constexpr uint32_t k65599HashConstant = 65599u;

  // The length is hashed as if it were the first character.
  uint32_t hash = kSize - 1;
  uint32_t coefficient = k65599HashConstant;

  size_t length =
      std::min(static_cast<size_t>(PW_TOKENIZER_CFG_C_HASH_LENGTH), kSize - 1);

  // Hash all of the characters in the string as unsigned ints.
  // The coefficient calculation is done modulo 0x100000000, so the unsigned
  // integer overflows are intentional.
  for (size_t i = 0; i < length; ++i) {
    hash += coefficient * str[i];
    coefficient *= k65599HashConstant;
  }

  return hash;
}

TEST(TokenizeStringLiteral, EmptyString_IsZero) {
  constexpr pw_tokenizer_Token token = PW_TOKENIZE_STRING("");
  EXPECT_EQ(0u, token);
}

TEST(TokenizeStringLiteral, String_MatchesHash) {
  constexpr uint32_t token = PW_TOKENIZE_STRING("[:-)");
  EXPECT_EQ(TestHash("[:-)"), token);
}

constexpr uint32_t kGlobalToken = PW_TOKENIZE_STRING(">:-[]");

TEST(TokenizeStringLiteral, GlobalVariable_MatchesHash) {
  EXPECT_EQ(TestHash(">:-[]"), kGlobalToken);
}

class TokenizeToBuffer : public ::testing::Test {
 public:
  TokenizeToBuffer() : buffer_{} {}

 protected:
  uint8_t buffer_[64];
};

// Test fixture for callback and global handler. Both of these need a global
// message buffer. To keep the message buffers separate, template this on the
// derived class type.
template <typename Impl>
class GlobalMessage : public ::testing::Test {
 public:
  static void SetMessage(const uint8_t* message, size_t size) {
    ASSERT_LE(size, sizeof(message_));
    std::memcpy(message_, message, size);
    message_size_bytes_ = size;
  }

 protected:
  GlobalMessage() {
    std::memset(message_, 0, sizeof(message_));
    message_size_bytes_ = 0;
  }

  static uint8_t message_[256];
  static size_t message_size_bytes_;
};

template <typename Impl>
uint8_t GlobalMessage<Impl>::message_[256] = {};
template <typename Impl>
size_t GlobalMessage<Impl>::message_size_bytes_ = 0;

class TokenizeToCallback : public GlobalMessage<TokenizeToCallback> {};

template <uint8_t... kData, size_t kSize>
std::array<uint8_t, sizeof(uint32_t) + sizeof...(kData)> ExpectedData(
    const char (&format)[kSize]) {
  const uint32_t value = TestHash(format);
  return std::array<uint8_t, sizeof(uint32_t) + sizeof...(kData)>{
      static_cast<uint8_t>(value & 0xff),
      static_cast<uint8_t>(value >> 8 & 0xff),
      static_cast<uint8_t>(value >> 16 & 0xff),
      static_cast<uint8_t>(value >> 24 & 0xff),
      kData...};
}

TEST_F(TokenizeToCallback, Variety) {
  PW_TOKENIZE_TO_CALLBACK(
      SetMessage, "%s there are %x (%.2f) of them%c", "Now", 2u, 2.0f, '.');
  const auto expected =  // clang-format off
      ExpectedData<3, 'N', 'o', 'w',        // string "Now"
                   0x04,                    // unsigned 2 (zig-zag encoded)
                   0x00, 0x00, 0x00, 0x40,  // float 2.0
                   0x5C                     // char '.' (0x2E, zig-zag encoded)
                   >("%s there are %x (%.2f) of them%c");
  // clang-format on
  ASSERT_EQ(expected.size(), message_size_bytes_);
  EXPECT_EQ(std::memcmp(expected.data(), message_, expected.size()), 0);
}

class TokenizeToGlobalHandler : public GlobalMessage<TokenizeToGlobalHandler> {
};

TEST_F(TokenizeToGlobalHandler, Variety) {
  PW_TOKENIZE_TO_GLOBAL_HANDLER("%x%lld%1.2f%s", 0, 0ll, -0.0, "");
  const auto expected =
      ExpectedData<0, 0, 0x00, 0x00, 0x00, 0x80, 0>("%x%lld%1.2f%s");
  ASSERT_EQ(expected.size(), message_size_bytes_);
  EXPECT_EQ(std::memcmp(expected.data(), message_, expected.size()), 0);
}

extern "C" void pw_tokenizer_HandleEncodedMessage(
    const uint8_t* encoded_message, size_t size_bytes) {
  TokenizeToGlobalHandler::SetMessage(encoded_message, size_bytes);
}

class TokenizeToGlobalHandlerWithPayload
    : public GlobalMessage<TokenizeToGlobalHandlerWithPayload> {
 public:
  static void SetPayload(pw_tokenizer_Payload payload) {
    payload_ = static_cast<intptr_t>(payload);
  }

 protected:
  TokenizeToGlobalHandlerWithPayload() { payload_ = {}; }

  static intptr_t payload_;
};

intptr_t TokenizeToGlobalHandlerWithPayload::payload_;

TEST_F(TokenizeToGlobalHandlerWithPayload, Variety) {
  ASSERT_NE(payload_, 123);

  const auto expected =
      ExpectedData<0, 0, 0x00, 0x00, 0x00, 0x80, 0>("%x%lld%1.2f%s");

  PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD(
      static_cast<pw_tokenizer_Payload>(123),
      "%x%lld%1.2f%s",
      0,
      0ll,
      -0.0,
      "");
  ASSERT_EQ(expected.size(), message_size_bytes_);
  EXPECT_EQ(std::memcmp(expected.data(), message_, expected.size()), 0);
  EXPECT_EQ(payload_, 123);

  PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD(
      static_cast<pw_tokenizer_Payload>(-543),
      "%x%lld%1.2f%s",
      0,
      0ll,
      -0.0,
      "");
  ASSERT_EQ(expected.size(), message_size_bytes_);
  EXPECT_EQ(std::memcmp(expected.data(), message_, expected.size()), 0);
  EXPECT_EQ(payload_, -543);
}

extern "C" void pw_tokenizer_HandleEncodedMessageWithPayload(
    pw_tokenizer_Payload payload,
    const uint8_t* encoded_message,
    size_t size_bytes) {
  TokenizeToGlobalHandlerWithPayload::SetMessage(encoded_message, size_bytes);
  TokenizeToGlobalHandlerWithPayload::SetPayload(payload);
}

}  // namespace
}  // namespace tokenizer
}  // namespace pw
