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

#include <cinttypes>
#include <cstdint>
#include <cstring>

#include "gtest/gtest.h"
#include "pw_tokenizer/tokenize_to_global_handler.h"
#include "pw_tokenizer/tokenize_to_global_handler_with_payload.h"
#include "pw_tokenizer_private/tokenize_test.h"

namespace pw::tokenizer {
namespace {

// Constructs an array with the hashed string followed by the provided bytes.
template <uint8_t... data, size_t kSize>
constexpr auto ExpectedData(
    const char (&format)[kSize],
    uint32_t token_mask = std::numeric_limits<uint32_t>::max()) {
  const uint32_t value = Hash(format) & token_mask;
  return std::array<uint8_t, sizeof(uint32_t) + sizeof...(data)>{
      static_cast<uint8_t>(value & 0xff),
      static_cast<uint8_t>(value >> 8 & 0xff),
      static_cast<uint8_t>(value >> 16 & 0xff),
      static_cast<uint8_t>(value >> 24 & 0xff),
      data...};
}

// Test fixture for both global handler functions. Both need a global message
// buffer. To keep the message buffers separate, template this on the derived
// class type.
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

class TokenizeToGlobalHandler : public GlobalMessage<TokenizeToGlobalHandler> {
};

TEST_F(TokenizeToGlobalHandler, Variety) {
  PW_TOKENIZE_TO_GLOBAL_HANDLER("%x%lld%1.2f%s", 0, 0ll, -0.0, "");
  const auto expected =
      ExpectedData<0, 0, 0x00, 0x00, 0x00, 0x80, 0>("%x%lld%1.2f%s");
  ASSERT_EQ(expected.size(), message_size_bytes_);
  EXPECT_EQ(std::memcmp(expected.data(), message_, expected.size()), 0);
}

TEST_F(TokenizeToGlobalHandler, Strings) {
  PW_TOKENIZE_TO_GLOBAL_HANDLER("The answer is: %s", "5432!");
  constexpr std::array<uint8_t, 10> expected =
      ExpectedData<5, '5', '4', '3', '2', '!'>("The answer is: %s");
  ASSERT_EQ(expected.size(), message_size_bytes_);
  EXPECT_EQ(std::memcmp(expected.data(), message_, expected.size()), 0);
}

TEST_F(TokenizeToGlobalHandler, Domain_Strings) {
  PW_TOKENIZE_TO_GLOBAL_HANDLER_DOMAIN(
      "TEST_DOMAIN", "The answer is: %s", "5432!");
  constexpr std::array<uint8_t, 10> expected =
      ExpectedData<5, '5', '4', '3', '2', '!'>("The answer is: %s");
  ASSERT_EQ(expected.size(), message_size_bytes_);
  EXPECT_EQ(std::memcmp(expected.data(), message_, expected.size()), 0);
}

TEST_F(TokenizeToGlobalHandler, Mask) {
  PW_TOKENIZE_TO_GLOBAL_HANDLER_MASK(
      "TEST_DOMAIN", 0x00FFF000, "The answer is: %s", "5432!");
  constexpr std::array<uint8_t, 10> expected =
      ExpectedData<5, '5', '4', '3', '2', '!'>("The answer is: %s", 0x00FFF000);
  ASSERT_EQ(expected.size(), message_size_bytes_);
  EXPECT_EQ(std::memcmp(expected.data(), message_, expected.size()), 0);
}

TEST_F(TokenizeToGlobalHandler, C_SequentialZigZag) {
  pw_tokenizer_ToGlobalHandlerTest_SequentialZigZag();

  constexpr std::array<uint8_t, 18> expected =
      ExpectedData<0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13>(
          TEST_FORMAT_SEQUENTIAL_ZIG_ZAG);
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

constexpr std::array<uint8_t, 10> kExpected =
    ExpectedData<5, '5', '4', '3', '2', '!'>("The answer is: %s");

TEST_F(TokenizeToGlobalHandlerWithPayload, Strings_ZeroPayload) {
  PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD({}, "The answer is: %s", "5432!");

  ASSERT_EQ(kExpected.size(), message_size_bytes_);
  EXPECT_EQ(std::memcmp(kExpected.data(), message_, kExpected.size()), 0);
  EXPECT_EQ(payload_, 0);
}

TEST_F(TokenizeToGlobalHandlerWithPayload, Strings_NonZeroPayload) {
  PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD(
      static_cast<pw_tokenizer_Payload>(5432), "The answer is: %s", "5432!");

  ASSERT_EQ(kExpected.size(), message_size_bytes_);
  EXPECT_EQ(std::memcmp(kExpected.data(), message_, kExpected.size()), 0);
  EXPECT_EQ(payload_, 5432);
}

TEST_F(TokenizeToGlobalHandlerWithPayload, Domain_Strings) {
  PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD_DOMAIN(
      "TEST_DOMAIN",
      static_cast<pw_tokenizer_Payload>(5432),
      "The answer is: %s",
      "5432!");
  ASSERT_EQ(kExpected.size(), message_size_bytes_);
  EXPECT_EQ(std::memcmp(kExpected.data(), message_, kExpected.size()), 0);
  EXPECT_EQ(payload_, 5432);
}

TEST_F(TokenizeToGlobalHandlerWithPayload, Mask) {
  PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD_MASK(
      "TEST_DOMAIN",
      0x12345678,
      static_cast<pw_tokenizer_Payload>(5432),
      "The answer is: %s",
      "5432!");
  constexpr std::array<uint8_t, 10> expected =
      ExpectedData<5, '5', '4', '3', '2', '!'>("The answer is: %s", 0x12345678);
  ASSERT_EQ(expected.size(), message_size_bytes_);
  EXPECT_EQ(std::memcmp(expected.data(), message_, expected.size()), 0);
  EXPECT_EQ(payload_, 5432);
}

struct Foo {
  unsigned char a;
  bool b;
};

TEST_F(TokenizeToGlobalHandlerWithPayload, PointerToStack) {
  Foo foo{254u, true};

  PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD(
      reinterpret_cast<pw_tokenizer_Payload>(&foo), "Boring!");

  constexpr auto expected = ExpectedData("Boring!");
  static_assert(expected.size() == 4);
  ASSERT_EQ(expected.size(), message_size_bytes_);
  EXPECT_EQ(std::memcmp(expected.data(), message_, expected.size()), 0);

  Foo* payload_foo = reinterpret_cast<Foo*>(payload_);
  ASSERT_EQ(&foo, payload_foo);
  EXPECT_EQ(payload_foo->a, 254u);
  EXPECT_TRUE(payload_foo->b);
}

TEST_F(TokenizeToGlobalHandlerWithPayload, C_SequentialZigZag) {
  pw_tokenizer_ToGlobalHandlerWithPayloadTest_SequentialZigZag();

  constexpr std::array<uint8_t, 18> expected =
      ExpectedData<0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13>(
          TEST_FORMAT_SEQUENTIAL_ZIG_ZAG);
  ASSERT_EQ(expected.size(), message_size_bytes_);
  EXPECT_EQ(std::memcmp(expected.data(), message_, expected.size()), 0);
  EXPECT_EQ(payload_, 600613);
}

extern "C" void pw_tokenizer_HandleEncodedMessageWithPayload(
    pw_tokenizer_Payload payload,
    const uint8_t* encoded_message,
    size_t size_bytes) {
  TokenizeToGlobalHandlerWithPayload::SetMessage(encoded_message, size_bytes);
  TokenizeToGlobalHandlerWithPayload::SetPayload(payload);
}

// Hijack an internal macro to capture the tokenizer domain.
#undef _PW_TOKENIZER_RECORD_ORIGINAL_STRING
#define _PW_TOKENIZER_RECORD_ORIGINAL_STRING(token, domain, string) \
  tokenizer_domain = domain;                                        \
  string_literal = string

TEST_F(TokenizeToGlobalHandler, Domain_Default) {
  const char* tokenizer_domain = nullptr;
  const char* string_literal = nullptr;

  PW_TOKENIZE_TO_GLOBAL_HANDLER("404");

  EXPECT_STREQ(tokenizer_domain, PW_TOKENIZER_DEFAULT_DOMAIN);
  EXPECT_STREQ(string_literal, "404");
}

TEST_F(TokenizeToGlobalHandler, Domain_Specified) {
  const char* tokenizer_domain = nullptr;
  const char* string_literal = nullptr;

  PW_TOKENIZE_TO_GLOBAL_HANDLER_DOMAIN("www.google.com", "404");

  EXPECT_STREQ(tokenizer_domain, "www.google.com");
  EXPECT_STREQ(string_literal, "404");
}

TEST_F(TokenizeToGlobalHandlerWithPayload, Domain_Default) {
  const char* tokenizer_domain = nullptr;
  const char* string_literal = nullptr;

  PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD(
      static_cast<pw_tokenizer_Payload>(123), "Wow%s", "???");

  EXPECT_STREQ(tokenizer_domain, PW_TOKENIZER_DEFAULT_DOMAIN);
  EXPECT_STREQ(string_literal, "Wow%s");
}

TEST_F(TokenizeToGlobalHandlerWithPayload, Domain_Specified) {
  const char* tokenizer_domain = nullptr;
  const char* string_literal = nullptr;

  PW_TOKENIZE_TO_GLOBAL_HANDLER_WITH_PAYLOAD_DOMAIN(
      "THEDOMAIN", static_cast<pw_tokenizer_Payload>(123), "1234567890");

  EXPECT_STREQ(tokenizer_domain, "THEDOMAIN");
  EXPECT_STREQ(string_literal, "1234567890");
}

}  // namespace
}  // namespace pw::tokenizer
