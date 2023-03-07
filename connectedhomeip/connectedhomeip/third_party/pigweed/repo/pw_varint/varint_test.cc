// Copyright 2019 The Pigweed Authors
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

#include "pw_varint/varint.h"

#include <cinttypes>
#include <cstdint>
#include <cstring>
#include <limits>

#include "gtest/gtest.h"

namespace pw::varint {
namespace {

extern "C" {

// Functions defined in varint_test.c which call the varint API from C.
size_t pw_varint_CallEncode(uint64_t integer, void* output, size_t output_size);
size_t pw_varint_CallZigZagEncode(int64_t integer,
                                  void* output,
                                  size_t output_size);
size_t pw_varint_CallDecode(void* input, size_t input_size, uint64_t* output);
size_t pw_varint_CallZigZagDecode(void* input,
                                  size_t input_size,
                                  int64_t* output);

}  // extern "C"

class VarintWithBuffer : public ::testing::Test {
 protected:
  VarintWithBuffer()
      : buffer_{std::byte{'a'},
                std::byte{'b'},
                std::byte{'c'},
                std::byte{'d'},
                std::byte{'e'},
                std::byte{'f'},
                std::byte{'g'},
                std::byte{'h'},
                std::byte{'i'},
                std::byte{'j'}} {}
  std::byte buffer_[10];
};

TEST_F(VarintWithBuffer, EncodeSizeUnsigned32_SmallSingleByte) {
  ASSERT_EQ(1u, Encode(UINT32_C(0), buffer_));
  EXPECT_EQ(std::byte{0}, buffer_[0]);
  ASSERT_EQ(1u, Encode(UINT32_C(1), buffer_));
  EXPECT_EQ(std::byte{1}, buffer_[0]);
  ASSERT_EQ(1u, Encode(UINT32_C(2), buffer_));
  EXPECT_EQ(std::byte{2}, buffer_[0]);
}

TEST_F(VarintWithBuffer, EncodeSizeUnsigned32_SmallSingleByte_C) {
  ASSERT_EQ(1u, pw_varint_CallEncode(UINT32_C(0), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::byte{0}, buffer_[0]);
  ASSERT_EQ(1u, pw_varint_CallEncode(UINT32_C(1), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::byte{1}, buffer_[0]);
  ASSERT_EQ(1u, pw_varint_CallEncode(UINT32_C(2), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::byte{2}, buffer_[0]);
}

TEST_F(VarintWithBuffer, EncodeSizeUnsigned32_LargeSingleByte) {
  ASSERT_EQ(1u, Encode(UINT32_C(63), buffer_));
  EXPECT_EQ(std::byte{63}, buffer_[0]);
  ASSERT_EQ(1u, Encode(UINT32_C(64), buffer_));
  EXPECT_EQ(std::byte{64}, buffer_[0]);
  ASSERT_EQ(1u, Encode(UINT32_C(126), buffer_));
  EXPECT_EQ(std::byte{126}, buffer_[0]);
  ASSERT_EQ(1u, Encode(UINT32_C(127), buffer_));
  EXPECT_EQ(std::byte{127}, buffer_[0]);
}

TEST_F(VarintWithBuffer, EncodeSizeUnsigned32_LargeSingleByte_C) {
  ASSERT_EQ(1u, pw_varint_CallEncode(UINT32_C(63), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::byte{63}, buffer_[0]);
  ASSERT_EQ(1u, pw_varint_CallEncode(UINT32_C(64), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::byte{64}, buffer_[0]);
  ASSERT_EQ(1u, pw_varint_CallEncode(UINT32_C(126), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::byte{126}, buffer_[0]);
  ASSERT_EQ(1u, pw_varint_CallEncode(UINT32_C(127), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::byte{127}, buffer_[0]);
}

TEST_F(VarintWithBuffer, EncodeSizeUnsigned32_MultiByte) {
  ASSERT_EQ(2u, Encode(UINT32_C(128), buffer_));
  EXPECT_EQ(std::memcmp("\x80\x01", buffer_, 2), 0);
  ASSERT_EQ(2u, Encode(UINT32_C(129), buffer_));
  EXPECT_EQ(std::memcmp("\x81\x01", buffer_, 2), 0);

  ASSERT_EQ(5u, Encode(std::numeric_limits<uint32_t>::max() - 1, buffer_));
  EXPECT_EQ(std::memcmp("\xfe\xff\xff\xff\x0f", buffer_, 5), 0);

  ASSERT_EQ(5u, Encode(std::numeric_limits<uint32_t>::max(), buffer_));
  EXPECT_EQ(std::memcmp("\xff\xff\xff\xff\x0f", buffer_, 5), 0);
}

TEST_F(VarintWithBuffer, EncodeSizeUnsigned32_MultiByte_C) {
  ASSERT_EQ(2u, pw_varint_CallEncode(UINT32_C(128), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::memcmp("\x80\x01", buffer_, 2), 0);
  ASSERT_EQ(2u, pw_varint_CallEncode(UINT32_C(129), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::memcmp("\x81\x01", buffer_, 2), 0);

  ASSERT_EQ(
      5u,
      pw_varint_CallEncode(
          std::numeric_limits<uint32_t>::max() - 1, buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::memcmp("\xfe\xff\xff\xff\x0f", buffer_, 5), 0);

  ASSERT_EQ(
      5u,
      pw_varint_CallEncode(
          std::numeric_limits<uint32_t>::max(), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::memcmp("\xff\xff\xff\xff\x0f", buffer_, 5), 0);
}

TEST_F(VarintWithBuffer, EncodeSizeSigned32_SmallSingleByte) {
  ASSERT_EQ(1u, Encode(INT32_C(0), buffer_));
  EXPECT_EQ(std::byte{0}, buffer_[0]);
  ASSERT_EQ(1u, Encode(INT32_C(-1), buffer_));
  EXPECT_EQ(std::byte{1}, buffer_[0]);
  ASSERT_EQ(1u, Encode(INT32_C(1), buffer_));
  EXPECT_EQ(std::byte{2}, buffer_[0]);
  ASSERT_EQ(1u, Encode(INT32_C(-2), buffer_));
  EXPECT_EQ(std::byte{3}, buffer_[0]);
  ASSERT_EQ(1u, Encode(INT32_C(2), buffer_));
  EXPECT_EQ(std::byte{4}, buffer_[0]);
}

TEST_F(VarintWithBuffer, EncodeSizeSigned32_SmallSingleByte_C) {
  ASSERT_EQ(1u,
            pw_varint_CallZigZagEncode(INT32_C(0), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::byte{0}, buffer_[0]);
  ASSERT_EQ(1u,
            pw_varint_CallZigZagEncode(INT32_C(-1), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::byte{1}, buffer_[0]);
  ASSERT_EQ(1u,
            pw_varint_CallZigZagEncode(INT32_C(1), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::byte{2}, buffer_[0]);
  ASSERT_EQ(1u,
            pw_varint_CallZigZagEncode(INT32_C(-2), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::byte{3}, buffer_[0]);
  ASSERT_EQ(1u,
            pw_varint_CallZigZagEncode(INT32_C(2), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::byte{4}, buffer_[0]);
}

TEST_F(VarintWithBuffer, EncodeSizeSigned32_LargeSingleByte) {
  ASSERT_EQ(1u, Encode(INT32_C(-63), buffer_));
  EXPECT_EQ(std::byte{125}, buffer_[0]);
  ASSERT_EQ(1u, Encode(INT32_C(63), buffer_));
  EXPECT_EQ(std::byte{126}, buffer_[0]);
  ASSERT_EQ(1u, Encode(INT32_C(-64), buffer_));
  EXPECT_EQ(std::byte{127}, buffer_[0]);
}

TEST_F(VarintWithBuffer, EncodeSizeSigned32_LargeSingleByte_C) {
  ASSERT_EQ(1u,
            pw_varint_CallZigZagEncode(INT32_C(-63), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::byte{125}, buffer_[0]);
  ASSERT_EQ(1u,
            pw_varint_CallZigZagEncode(INT32_C(63), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::byte{126}, buffer_[0]);
  ASSERT_EQ(1u,
            pw_varint_CallZigZagEncode(INT32_C(-64), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::byte{127}, buffer_[0]);
}

TEST_F(VarintWithBuffer, EncodeSizeSigned32_MultiByte) {
  ASSERT_EQ(2u, Encode(INT32_C(64), buffer_));
  EXPECT_EQ(std::memcmp("\x80\x01", buffer_, 2), 0);
  ASSERT_EQ(2u, Encode(INT32_C(-65), buffer_));
  EXPECT_EQ(std::memcmp("\x81\x01", buffer_, 2), 0);
  ASSERT_EQ(2u, Encode(INT32_C(65), buffer_));
  EXPECT_EQ(std::memcmp("\x82\x01", buffer_, 2), 0);

  ASSERT_EQ(5u, Encode(std::numeric_limits<int32_t>::min(), buffer_));
  EXPECT_EQ(std::memcmp("\xff\xff\xff\xff\x0f", buffer_, 5), 0);

  ASSERT_EQ(5u, Encode(std::numeric_limits<int32_t>::max(), buffer_));
  EXPECT_EQ(std::memcmp("\xfe\xff\xff\xff\x0f", buffer_, 5), 0);
}

TEST_F(VarintWithBuffer, EncodeSizeSigned32_MultiByte_C) {
  ASSERT_EQ(2u,
            pw_varint_CallZigZagEncode(INT32_C(64), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::memcmp("\x80\x01", buffer_, 2), 0);
  ASSERT_EQ(2u,
            pw_varint_CallZigZagEncode(INT32_C(-65), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::memcmp("\x81\x01", buffer_, 2), 0);
  ASSERT_EQ(2u,
            pw_varint_CallZigZagEncode(INT32_C(65), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::memcmp("\x82\x01", buffer_, 2), 0);

  ASSERT_EQ(5u,
            pw_varint_CallZigZagEncode(
                std::numeric_limits<int32_t>::min(), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::memcmp("\xff\xff\xff\xff\x0f", buffer_, 5), 0);

  ASSERT_EQ(5u,
            pw_varint_CallZigZagEncode(
                std::numeric_limits<int32_t>::max(), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::memcmp("\xfe\xff\xff\xff\x0f", buffer_, 5), 0);
}

TEST_F(VarintWithBuffer, EncodeSizeUnsigned64_SmallSingleByte) {
  ASSERT_EQ(1u, Encode(UINT64_C(0), buffer_));
  EXPECT_EQ(std::byte{0}, buffer_[0]);
  ASSERT_EQ(1u, Encode(UINT64_C(1), buffer_));
  EXPECT_EQ(std::byte{1}, buffer_[0]);
  ASSERT_EQ(1u, Encode(UINT64_C(2), buffer_));
  EXPECT_EQ(std::byte{2}, buffer_[0]);
}

TEST_F(VarintWithBuffer, EncodeSizeUnsigned64_SmallSingleByte_C) {
  ASSERT_EQ(1u, pw_varint_CallEncode(UINT64_C(0), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::byte{0}, buffer_[0]);
  ASSERT_EQ(1u, pw_varint_CallEncode(UINT64_C(1), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::byte{1}, buffer_[0]);
  ASSERT_EQ(1u, pw_varint_CallEncode(UINT64_C(2), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::byte{2}, buffer_[0]);
}

TEST_F(VarintWithBuffer, EncodeSizeUnsigned64_LargeSingleByte) {
  ASSERT_EQ(1u, Encode(UINT64_C(63), buffer_));
  EXPECT_EQ(std::byte{63}, buffer_[0]);
  ASSERT_EQ(1u, Encode(UINT64_C(64), buffer_));
  EXPECT_EQ(std::byte{64}, buffer_[0]);
  ASSERT_EQ(1u, Encode(UINT64_C(126), buffer_));
  EXPECT_EQ(std::byte{126}, buffer_[0]);
  ASSERT_EQ(1u, Encode(UINT64_C(127), buffer_));
  EXPECT_EQ(std::byte{127}, buffer_[0]);
}

TEST_F(VarintWithBuffer, EncodeSizeUnsigned64_LargeSingleByte_C) {
  ASSERT_EQ(1u, pw_varint_CallEncode(UINT64_C(63), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::byte{63}, buffer_[0]);
  ASSERT_EQ(1u, pw_varint_CallEncode(UINT64_C(64), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::byte{64}, buffer_[0]);
  ASSERT_EQ(1u, pw_varint_CallEncode(UINT64_C(126), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::byte{126}, buffer_[0]);
  ASSERT_EQ(1u, pw_varint_CallEncode(UINT64_C(127), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::byte{127}, buffer_[0]);
}

TEST_F(VarintWithBuffer, EncodeSizeUnsigned64_MultiByte) {
  ASSERT_EQ(2u, Encode(UINT64_C(128), buffer_));
  EXPECT_EQ(std::memcmp("\x80\x01", buffer_, 2), 0);
  ASSERT_EQ(2u, Encode(UINT64_C(129), buffer_));
  EXPECT_EQ(std::memcmp("\x81\x01", buffer_, 2), 0);

  ASSERT_EQ(5u, Encode(std::numeric_limits<uint32_t>::max() - 1, buffer_));
  EXPECT_EQ(std::memcmp("\xfe\xff\xff\xff\x0f", buffer_, 5), 0);

  ASSERT_EQ(5u, Encode(std::numeric_limits<uint32_t>::max(), buffer_));
  EXPECT_EQ(std::memcmp("\xff\xff\xff\xff\x0f", buffer_, 5), 0);

  ASSERT_EQ(10u, Encode(std::numeric_limits<uint64_t>::max() - 1, buffer_));
  EXPECT_EQ(
      std::memcmp("\xfe\xff\xff\xff\xff\xff\xff\xff\xff\x01", buffer_, 10), 0);

  ASSERT_EQ(10u, Encode(std::numeric_limits<uint64_t>::max(), buffer_));
  EXPECT_EQ(
      std::memcmp("\xff\xff\xff\xff\xff\xff\xff\xff\xff\x01", buffer_, 10), 0);
}

TEST_F(VarintWithBuffer, EncodeSizeUnsigned64_MultiByte_C) {
  ASSERT_EQ(2u, pw_varint_CallEncode(UINT64_C(128), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::memcmp("\x80\x01", buffer_, 2), 0);
  ASSERT_EQ(2u, pw_varint_CallEncode(UINT64_C(129), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::memcmp("\x81\x01", buffer_, 2), 0);

  ASSERT_EQ(
      5u,
      pw_varint_CallEncode(
          std::numeric_limits<uint32_t>::max() - 1, buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::memcmp("\xfe\xff\xff\xff\x0f", buffer_, 5), 0);

  ASSERT_EQ(
      5u,
      pw_varint_CallEncode(
          std::numeric_limits<uint32_t>::max(), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::memcmp("\xff\xff\xff\xff\x0f", buffer_, 5), 0);

  ASSERT_EQ(
      10u,
      pw_varint_CallEncode(
          std::numeric_limits<uint64_t>::max() - 1, buffer_, sizeof(buffer_)));
  EXPECT_EQ(
      std::memcmp("\xfe\xff\xff\xff\xff\xff\xff\xff\xff\x01", buffer_, 10), 0);

  ASSERT_EQ(
      10u,
      pw_varint_CallEncode(
          std::numeric_limits<uint64_t>::max(), buffer_, sizeof(buffer_)));
  EXPECT_EQ(
      std::memcmp("\xff\xff\xff\xff\xff\xff\xff\xff\xff\x01", buffer_, 10), 0);
}

TEST_F(VarintWithBuffer, EncodeSizeSigned64_SmallSingleByte) {
  ASSERT_EQ(1u, Encode(INT64_C(0), buffer_));
  EXPECT_EQ(std::byte{0}, buffer_[0]);
  ASSERT_EQ(1u, Encode(INT64_C(-1), buffer_));
  EXPECT_EQ(std::byte{1}, buffer_[0]);
  ASSERT_EQ(1u, Encode(INT64_C(1), buffer_));
  EXPECT_EQ(std::byte{2}, buffer_[0]);
  ASSERT_EQ(1u, Encode(INT64_C(-2), buffer_));
  EXPECT_EQ(std::byte{3}, buffer_[0]);
  ASSERT_EQ(1u, Encode(INT64_C(2), buffer_));
  EXPECT_EQ(std::byte{4}, buffer_[0]);
}

TEST_F(VarintWithBuffer, EncodeSizeSigned64_SmallSingleByte_C) {
  ASSERT_EQ(1u,
            pw_varint_CallZigZagEncode(INT64_C(0), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::byte{0}, buffer_[0]);
  ASSERT_EQ(1u,
            pw_varint_CallZigZagEncode(INT64_C(-1), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::byte{1}, buffer_[0]);
  ASSERT_EQ(1u,
            pw_varint_CallZigZagEncode(INT64_C(1), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::byte{2}, buffer_[0]);
  ASSERT_EQ(1u,
            pw_varint_CallZigZagEncode(INT64_C(-2), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::byte{3}, buffer_[0]);
  ASSERT_EQ(1u,
            pw_varint_CallZigZagEncode(INT64_C(2), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::byte{4}, buffer_[0]);
}

TEST_F(VarintWithBuffer, EncodeSizeSigned64_LargeSingleByte) {
  ASSERT_EQ(1u, Encode(INT64_C(-63), buffer_));
  EXPECT_EQ(std::byte{125}, buffer_[0]);
  ASSERT_EQ(1u, Encode(INT64_C(63), buffer_));
  EXPECT_EQ(std::byte{126}, buffer_[0]);
  ASSERT_EQ(1u, Encode(INT64_C(-64), buffer_));
  EXPECT_EQ(std::byte{127}, buffer_[0]);
}

TEST_F(VarintWithBuffer, EncodeSizeSigned64_LargeSingleByte_C) {
  ASSERT_EQ(1u,
            pw_varint_CallZigZagEncode(INT64_C(-63), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::byte{125}, buffer_[0]);
  ASSERT_EQ(1u,
            pw_varint_CallZigZagEncode(INT64_C(63), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::byte{126}, buffer_[0]);
  ASSERT_EQ(1u,
            pw_varint_CallZigZagEncode(INT64_C(-64), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::byte{127}, buffer_[0]);
}

TEST_F(VarintWithBuffer, EncodeSizeSigned64_MultiByte) {
  ASSERT_EQ(2u, Encode(INT64_C(64), buffer_));
  EXPECT_EQ(std::memcmp("\x80\x01", buffer_, 2), 0);
  ASSERT_EQ(2u, Encode(INT64_C(-65), buffer_));
  EXPECT_EQ(std::memcmp("\x81\x01", buffer_, 2), 0);
  ASSERT_EQ(2u, Encode(INT64_C(65), buffer_));
  EXPECT_EQ(std::memcmp("\x82\x01", buffer_, 2), 0);

  ASSERT_EQ(5u,
            Encode(static_cast<int64_t>(std::numeric_limits<int32_t>::min()),
                   buffer_));
  EXPECT_EQ(std::memcmp("\xff\xff\xff\xff\x0f", buffer_, 5), 0);

  ASSERT_EQ(5u,
            Encode(static_cast<int64_t>(std::numeric_limits<int32_t>::max()),
                   buffer_));
  EXPECT_EQ(std::memcmp("\xfe\xff\xff\xff\x0f", buffer_, 5), 0);

  ASSERT_EQ(10u, Encode(std::numeric_limits<int64_t>::min(), buffer_));
  EXPECT_EQ(
      std::memcmp("\xff\xff\xff\xff\xff\xff\xff\xff\xff\x01", buffer_, 10), 0);

  ASSERT_EQ(10u, Encode(std::numeric_limits<int64_t>::max(), buffer_));
  EXPECT_EQ(
      std::memcmp("\xfe\xff\xff\xff\xff\xff\xff\xff\xff\x01", buffer_, 10), 0);
}

TEST_F(VarintWithBuffer, EncodeSizeSigned64_MultiByte_C) {
  ASSERT_EQ(2u,
            pw_varint_CallZigZagEncode(INT64_C(64), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::memcmp("\x80\x01", buffer_, 2), 0);
  ASSERT_EQ(2u,
            pw_varint_CallZigZagEncode(INT64_C(-65), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::memcmp("\x81\x01", buffer_, 2), 0);
  ASSERT_EQ(2u,
            pw_varint_CallZigZagEncode(INT64_C(65), buffer_, sizeof(buffer_)));
  EXPECT_EQ(std::memcmp("\x82\x01", buffer_, 2), 0);

  ASSERT_EQ(5u,
            pw_varint_CallZigZagEncode(
                static_cast<int64_t>(std::numeric_limits<int32_t>::min()),
                buffer_,
                sizeof(buffer_)));
  EXPECT_EQ(std::memcmp("\xff\xff\xff\xff\x0f", buffer_, 5), 0);

  ASSERT_EQ(5u,
            pw_varint_CallZigZagEncode(
                static_cast<int64_t>(std::numeric_limits<int32_t>::max()),
                buffer_,
                sizeof(buffer_)));
  EXPECT_EQ(std::memcmp("\xfe\xff\xff\xff\x0f", buffer_, 5), 0);

  ASSERT_EQ(10u,
            pw_varint_CallZigZagEncode(
                std::numeric_limits<int64_t>::min(), buffer_, sizeof(buffer_)));
  EXPECT_EQ(
      std::memcmp("\xff\xff\xff\xff\xff\xff\xff\xff\xff\x01", buffer_, 10), 0);

  ASSERT_EQ(10u,
            pw_varint_CallZigZagEncode(
                std::numeric_limits<int64_t>::max(), buffer_, sizeof(buffer_)));
  EXPECT_EQ(
      std::memcmp("\xfe\xff\xff\xff\xff\xff\xff\xff\xff\x01", buffer_, 10), 0);
}

// How much to increment by for each iteration of the exhaustive encode/decode
// tests. Set the increment to 1 to test every number (this is slow).
constexpr int kIncrement = 100'000'009;

TEST_F(VarintWithBuffer, EncodeDecodeSigned32) {
  int32_t i = std::numeric_limits<int32_t>::min();
  while (true) {
    size_t encoded = Encode(i, buffer_);

    int64_t result;
    size_t decoded = Decode(buffer_, &result);

    EXPECT_EQ(encoded, decoded);
    ASSERT_EQ(i, result);

    if (i > std::numeric_limits<int32_t>::max() - kIncrement) {
      break;
    }

    i += kIncrement;
  }
}

TEST_F(VarintWithBuffer, EncodeDecodeSigned32_C) {
  int32_t i = std::numeric_limits<int32_t>::min();
  while (true) {
    size_t encoded = pw_varint_CallZigZagEncode(i, buffer_, sizeof(buffer_));

    int64_t result;
    size_t decoded =
        pw_varint_CallZigZagDecode(buffer_, sizeof(buffer_), &result);

    EXPECT_EQ(encoded, decoded);
    ASSERT_EQ(i, result);

    if (i > std::numeric_limits<int32_t>::max() - kIncrement) {
      break;
    }

    i += kIncrement;
  }
}

TEST_F(VarintWithBuffer, EncodeDecodeUnsigned32) {
  uint32_t i = 0;
  while (true) {
    size_t encoded = Encode(i, buffer_);

    uint64_t result;
    size_t decoded = Decode(buffer_, &result);

    EXPECT_EQ(encoded, decoded);
    ASSERT_EQ(i, result);

    if (i > std::numeric_limits<uint32_t>::max() - kIncrement) {
      break;
    }

    i += kIncrement;
  }
}

TEST_F(VarintWithBuffer, EncodeDecodeUnsigned32_C) {
  uint32_t i = 0;
  while (true) {
    size_t encoded = pw_varint_CallEncode(i, buffer_, sizeof(buffer_));

    uint64_t result;
    size_t decoded = pw_varint_CallDecode(buffer_, sizeof(buffer_), &result);

    EXPECT_EQ(encoded, decoded);
    ASSERT_EQ(i, result);

    if (i > std::numeric_limits<uint32_t>::max() - kIncrement) {
      break;
    }

    i += kIncrement;
  }
}

template <size_t kStringSize>
auto MakeBuffer(const char (&data)[kStringSize]) {
  constexpr size_t kSizeBytes = kStringSize - 1;
  static_assert(kSizeBytes <= 10, "Varint arrays never need be larger than 10");

  std::array<std::byte, kSizeBytes> array;
  std::memcpy(array.data(), data, kSizeBytes);
  return array;
}

TEST(VarintDecode, DecodeSigned64_SingleByte) {
  int64_t value = -1234;

  EXPECT_EQ(Decode(MakeBuffer("\x00"), &value), 1u);
  EXPECT_EQ(value, 0);

  EXPECT_EQ(Decode(MakeBuffer("\x01"), &value), 1u);
  EXPECT_EQ(value, -1);

  EXPECT_EQ(Decode(MakeBuffer("\x02"), &value), 1u);
  EXPECT_EQ(value, 1);

  EXPECT_EQ(Decode(MakeBuffer("\x03"), &value), 1u);
  EXPECT_EQ(value, -2);

  EXPECT_EQ(Decode(MakeBuffer("\x04"), &value), 1u);
  EXPECT_EQ(value, 2);

  EXPECT_EQ(Decode(MakeBuffer("\x04"), &value), 1u);
  EXPECT_EQ(value, 2);
}

TEST(VarintDecode, DecodeSigned64_SingleByte_C) {
  int64_t value = -1234;

  auto buffer = MakeBuffer("\x00");
  EXPECT_EQ(pw_varint_CallZigZagDecode(buffer.data(), buffer.size(), &value),
            1u);
  EXPECT_EQ(value, 0);

  buffer = MakeBuffer("\x01");
  EXPECT_EQ(pw_varint_CallZigZagDecode(buffer.data(), buffer.size(), &value),
            1u);
  EXPECT_EQ(value, -1);

  buffer = MakeBuffer("\x02");
  EXPECT_EQ(pw_varint_CallZigZagDecode(buffer.data(), buffer.size(), &value),
            1u);
  EXPECT_EQ(value, 1);

  buffer = MakeBuffer("\x03");
  EXPECT_EQ(pw_varint_CallZigZagDecode(buffer.data(), buffer.size(), &value),
            1u);
  EXPECT_EQ(value, -2);

  buffer = MakeBuffer("\x04");
  EXPECT_EQ(pw_varint_CallZigZagDecode(buffer.data(), buffer.size(), &value),
            1u);
  EXPECT_EQ(value, 2);

  buffer = MakeBuffer("\x04");
  EXPECT_EQ(pw_varint_CallZigZagDecode(buffer.data(), buffer.size(), &value),
            1u);
  EXPECT_EQ(value, 2);
}

TEST(VarintDecode, DecodeSigned64_MultiByte) {
  int64_t value = -1234;

  EXPECT_EQ(Decode(MakeBuffer("\x80\x01"), &value), 2u);
  EXPECT_EQ(value, 64);

  EXPECT_EQ(Decode(MakeBuffer("\x81\x01"), &value), 2u);
  EXPECT_EQ(value, -65);

  EXPECT_EQ(Decode(MakeBuffer("\x82\x01"), &value), 2u);
  EXPECT_EQ(value, 65);

  EXPECT_EQ(Decode(MakeBuffer("\xff\xff\xff\xff\x0f"), &value), 5u);
  EXPECT_EQ(value, std::numeric_limits<int32_t>::min());

  EXPECT_EQ(Decode(MakeBuffer("\xfe\xff\xff\xff\x0f"), &value), 5u);
  EXPECT_EQ(value, std::numeric_limits<int32_t>::max());

  EXPECT_EQ(
      Decode(MakeBuffer("\xff\xff\xff\xff\xff\xff\xff\xff\xff\x01"), &value),
      10u);
  EXPECT_EQ(value, std::numeric_limits<int64_t>::min());

  EXPECT_EQ(
      Decode(MakeBuffer("\xfe\xff\xff\xff\xff\xff\xff\xff\xff\x01"), &value),
      10u);
  EXPECT_EQ(value, std::numeric_limits<int64_t>::max());
}

TEST(VarintDecode, DecodeSigned64_MultiByte_C) {
  int64_t value = -1234;

  auto buffer2 = MakeBuffer("\x80\x01");
  EXPECT_EQ(pw_varint_CallZigZagDecode(buffer2.data(), buffer2.size(), &value),
            2u);
  EXPECT_EQ(value, 64);

  buffer2 = MakeBuffer("\x81\x01");
  EXPECT_EQ(pw_varint_CallZigZagDecode(buffer2.data(), buffer2.size(), &value),
            2u);
  EXPECT_EQ(value, -65);

  buffer2 = MakeBuffer("\x82\x01");
  EXPECT_EQ(pw_varint_CallZigZagDecode(buffer2.data(), buffer2.size(), &value),
            2u);
  EXPECT_EQ(value, 65);

  auto buffer4 = MakeBuffer("\xff\xff\xff\xff\x0f");
  EXPECT_EQ(pw_varint_CallZigZagDecode(buffer4.data(), buffer4.size(), &value),
            5u);
  EXPECT_EQ(value, std::numeric_limits<int32_t>::min());

  buffer4 = MakeBuffer("\xfe\xff\xff\xff\x0f");
  EXPECT_EQ(pw_varint_CallZigZagDecode(buffer4.data(), buffer4.size(), &value),
            5u);
  EXPECT_EQ(value, std::numeric_limits<int32_t>::max());

  auto buffer8 = MakeBuffer("\xff\xff\xff\xff\xff\xff\xff\xff\xff\x01");
  EXPECT_EQ(pw_varint_CallZigZagDecode(buffer8.data(), buffer8.size(), &value),
            10u);
  EXPECT_EQ(value, std::numeric_limits<int64_t>::min());

  buffer8 = MakeBuffer("\xfe\xff\xff\xff\xff\xff\xff\xff\xff\x01");
  EXPECT_EQ(pw_varint_CallZigZagDecode(buffer8.data(), buffer8.size(), &value),
            10u);
  EXPECT_EQ(value, std::numeric_limits<int64_t>::max());
}

TEST(Varint, ZigZagEncode_Int8) {
  EXPECT_EQ(ZigZagEncode(int8_t(0)), uint8_t(0));
  EXPECT_EQ(ZigZagEncode(int8_t(-1)), uint8_t(1));
  EXPECT_EQ(ZigZagEncode(int8_t(1)), uint8_t(2));
  EXPECT_EQ(ZigZagEncode(int8_t(-2)), uint8_t(3));
  EXPECT_EQ(ZigZagEncode(int8_t(2)), uint8_t(4));
  EXPECT_EQ(ZigZagEncode(int8_t(-33)), uint8_t(65));
  EXPECT_EQ(ZigZagEncode(int8_t(33)), uint8_t(66));
  EXPECT_EQ(ZigZagEncode(std::numeric_limits<int8_t>::min()),
            std::numeric_limits<uint8_t>::max());
  EXPECT_EQ(ZigZagEncode(std::numeric_limits<int8_t>::max()),
            std::numeric_limits<uint8_t>::max() - 1u);
}

TEST(Varint, ZigZagEncode_Int16) {
  EXPECT_EQ(ZigZagEncode(int16_t(0)), uint16_t(0));
  EXPECT_EQ(ZigZagEncode(int16_t(-1)), uint16_t(1));
  EXPECT_EQ(ZigZagEncode(int16_t(1)), uint16_t(2));
  EXPECT_EQ(ZigZagEncode(int16_t(-2)), uint16_t(3));
  EXPECT_EQ(ZigZagEncode(int16_t(2)), uint16_t(4));
  EXPECT_EQ(ZigZagEncode(int16_t(-3333)), uint16_t(6665));
  EXPECT_EQ(ZigZagEncode(int16_t(3333)), uint16_t(6666));
  EXPECT_EQ(ZigZagEncode(std::numeric_limits<int16_t>::min()),
            std::numeric_limits<uint16_t>::max());
  EXPECT_EQ(ZigZagEncode(std::numeric_limits<int16_t>::max()),
            std::numeric_limits<uint16_t>::max() - 1u);
}

TEST(Varint, ZigZagEncode_Int32) {
  EXPECT_EQ(ZigZagEncode(int32_t(0)), uint32_t(0));
  EXPECT_EQ(ZigZagEncode(int32_t(-1)), uint32_t(1));
  EXPECT_EQ(ZigZagEncode(int32_t(1)), uint32_t(2));
  EXPECT_EQ(ZigZagEncode(int32_t(-2)), uint32_t(3));
  EXPECT_EQ(ZigZagEncode(int32_t(2)), uint32_t(4));
  EXPECT_EQ(ZigZagEncode(int32_t(-128)), uint32_t(255));
  EXPECT_EQ(ZigZagEncode(int32_t(128)), uint32_t(256));
  EXPECT_EQ(ZigZagEncode(int32_t(-333333)), uint32_t(666665));
  EXPECT_EQ(ZigZagEncode(int32_t(333333)), uint32_t(666666));
  EXPECT_EQ(ZigZagEncode(std::numeric_limits<int32_t>::min()),
            std::numeric_limits<uint32_t>::max());
  EXPECT_EQ(ZigZagEncode(std::numeric_limits<int32_t>::max()),
            std::numeric_limits<uint32_t>::max() - 1u);
}

TEST(Varint, ZigZagEncode_Int64) {
  EXPECT_EQ(ZigZagEncode(int64_t(0)), uint64_t(0));
  EXPECT_EQ(ZigZagEncode(int64_t(-1)), uint64_t(1));
  EXPECT_EQ(ZigZagEncode(int64_t(1)), uint64_t(2));
  EXPECT_EQ(ZigZagEncode(int64_t(-2)), uint64_t(3));
  EXPECT_EQ(ZigZagEncode(int64_t(2)), uint64_t(4));
  EXPECT_EQ(ZigZagEncode(int64_t(-3333333333)), uint64_t(6666666665));
  EXPECT_EQ(ZigZagEncode(int64_t(3333333333)), uint64_t(6666666666));
  EXPECT_EQ(ZigZagEncode(std::numeric_limits<int64_t>::min()),
            std::numeric_limits<uint64_t>::max());
  EXPECT_EQ(ZigZagEncode(std::numeric_limits<int64_t>::max()),
            std::numeric_limits<uint64_t>::max() - 1u);
}

TEST(Varint, ZigZagDecode_Int8) {
  EXPECT_EQ(ZigZagDecode(uint8_t(0)), int8_t(0));
  EXPECT_EQ(ZigZagDecode(uint8_t(1)), int8_t(-1));
  EXPECT_EQ(ZigZagDecode(uint8_t(2)), int8_t(1));
  EXPECT_EQ(ZigZagDecode(uint8_t(3)), int8_t(-2));
  EXPECT_EQ(ZigZagDecode(uint8_t(4)), int8_t(2));
  EXPECT_EQ(ZigZagDecode(uint8_t(65)), int8_t(-33));
  EXPECT_EQ(ZigZagDecode(uint8_t(66)), int8_t(33));
  EXPECT_EQ(ZigZagDecode(std::numeric_limits<uint8_t>::max()),
            std::numeric_limits<int8_t>::min());
  EXPECT_EQ(ZigZagDecode(std::numeric_limits<uint8_t>::max() - 1u),
            std::numeric_limits<int8_t>::max());
}

TEST(Varint, ZigZagDecode_Int16) {
  EXPECT_EQ(ZigZagDecode(uint16_t(0)), int16_t(0));
  EXPECT_EQ(ZigZagDecode(uint16_t(1)), int16_t(-1));
  EXPECT_EQ(ZigZagDecode(uint16_t(2)), int16_t(1));
  EXPECT_EQ(ZigZagDecode(uint16_t(3)), int16_t(-2));
  EXPECT_EQ(ZigZagDecode(uint16_t(4)), int16_t(2));
  EXPECT_EQ(ZigZagDecode(uint16_t(6665)), int16_t(-3333));
  EXPECT_EQ(ZigZagDecode(uint16_t(6666)), int16_t(3333));
  EXPECT_EQ(ZigZagDecode(std::numeric_limits<uint16_t>::max()),
            std::numeric_limits<int16_t>::min());
  EXPECT_EQ(ZigZagDecode(std::numeric_limits<uint16_t>::max() - 1u),
            std::numeric_limits<int16_t>::max());
}

TEST(Varint, ZigZagDecode_Int32) {
  EXPECT_EQ(ZigZagDecode(uint32_t(0)), int32_t(0));
  EXPECT_EQ(ZigZagDecode(uint32_t(1)), int32_t(-1));
  EXPECT_EQ(ZigZagDecode(uint32_t(2)), int32_t(1));
  EXPECT_EQ(ZigZagDecode(uint32_t(3)), int32_t(-2));
  EXPECT_EQ(ZigZagDecode(uint32_t(4)), int32_t(2));
  EXPECT_EQ(ZigZagDecode(uint32_t(255)), int32_t(-128));
  EXPECT_EQ(ZigZagDecode(uint32_t(256)), int32_t(128));
  EXPECT_EQ(ZigZagDecode(uint32_t(666665)), int32_t(-333333));
  EXPECT_EQ(ZigZagDecode(uint32_t(666666)), int32_t(333333));
  EXPECT_EQ(ZigZagDecode(std::numeric_limits<uint32_t>::max()),
            std::numeric_limits<int32_t>::min());
  EXPECT_EQ(ZigZagDecode(std::numeric_limits<uint32_t>::max() - 1u),
            std::numeric_limits<int32_t>::max());
}

TEST(Varint, ZigZagDecode_Int64) {
  EXPECT_EQ(ZigZagDecode(uint64_t(0)), int64_t(0));
  EXPECT_EQ(ZigZagDecode(uint64_t(1)), int64_t(-1));
  EXPECT_EQ(ZigZagDecode(uint64_t(2)), int64_t(1));
  EXPECT_EQ(ZigZagDecode(uint64_t(3)), int64_t(-2));
  EXPECT_EQ(ZigZagDecode(uint64_t(4)), int64_t(2));
  EXPECT_EQ(ZigZagDecode(uint64_t(6666666665)), int64_t(-3333333333));
  EXPECT_EQ(ZigZagDecode(uint64_t(6666666666)), int64_t(3333333333));
  EXPECT_EQ(ZigZagDecode(std::numeric_limits<uint64_t>::max()),
            std::numeric_limits<int64_t>::min());
  EXPECT_EQ(ZigZagDecode(std::numeric_limits<uint64_t>::max() - 1llu),
            std::numeric_limits<int64_t>::max());
}

TEST(Varint, ZigZagEncodeDecode) {
  EXPECT_EQ(ZigZagDecode(ZigZagEncode(0)), 0);
  EXPECT_EQ(ZigZagDecode(ZigZagEncode(1)), 1);
  EXPECT_EQ(ZigZagDecode(ZigZagEncode(-1)), -1);
  EXPECT_EQ(ZigZagDecode(ZigZagEncode(8675309)), 8675309);
  EXPECT_EQ(ZigZagDecode(ZigZagEncode(std::numeric_limits<int8_t>::min())),
            std::numeric_limits<int8_t>::min());
  EXPECT_EQ(ZigZagDecode(ZigZagEncode(std::numeric_limits<int8_t>::max())),
            std::numeric_limits<int8_t>::max());
  EXPECT_EQ(ZigZagDecode(ZigZagEncode(std::numeric_limits<int16_t>::min())),
            std::numeric_limits<int16_t>::min());
  EXPECT_EQ(ZigZagDecode(ZigZagEncode(std::numeric_limits<int16_t>::max())),
            std::numeric_limits<int16_t>::max());
  EXPECT_EQ(ZigZagDecode(ZigZagEncode(std::numeric_limits<int32_t>::min())),
            std::numeric_limits<int32_t>::min());
  EXPECT_EQ(ZigZagDecode(ZigZagEncode(std::numeric_limits<int32_t>::max())),
            std::numeric_limits<int32_t>::max());
  EXPECT_EQ(ZigZagDecode(ZigZagEncode(std::numeric_limits<int64_t>::min())),
            std::numeric_limits<int64_t>::min());
  EXPECT_EQ(ZigZagDecode(ZigZagEncode(std::numeric_limits<int64_t>::max())),
            std::numeric_limits<int64_t>::max());
}

TEST_F(VarintWithBuffer, EncodeWithOptions_SingleByte) {
  ASSERT_EQ(Encode(0u, buffer_, Format::kZeroTerminatedLeastSignificant), 1u);
  EXPECT_EQ(buffer_[0], std::byte{0x00});

  ASSERT_EQ(Encode(1u, buffer_, Format::kZeroTerminatedLeastSignificant), 1u);
  EXPECT_EQ(buffer_[0], std::byte{0x02});

  ASSERT_EQ(Encode(0x7f, buffer_, Format::kZeroTerminatedLeastSignificant), 1u);
  EXPECT_EQ(buffer_[0], std::byte{0xfe});

  ASSERT_EQ(Encode(0u, buffer_, Format::kOneTerminatedLeastSignificant), 1u);
  EXPECT_EQ(buffer_[0], std::byte{0x01});

  ASSERT_EQ(Encode(2u, buffer_, Format::kOneTerminatedLeastSignificant), 1u);
  EXPECT_EQ(buffer_[0], std::byte{0x05});

  ASSERT_EQ(Encode(0x7f, buffer_, Format::kOneTerminatedLeastSignificant), 1u);
  EXPECT_EQ(buffer_[0], std::byte{0xff});

  ASSERT_EQ(Encode(0u, buffer_, Format::kZeroTerminatedMostSignificant), 1u);
  EXPECT_EQ(buffer_[0], std::byte{0x00});

  ASSERT_EQ(Encode(7u, buffer_, Format::kZeroTerminatedMostSignificant), 1u);
  EXPECT_EQ(buffer_[0], std::byte{0x07});

  ASSERT_EQ(Encode(0x7f, buffer_, Format::kZeroTerminatedMostSignificant), 1u);
  EXPECT_EQ(buffer_[0], std::byte{0x7f});

  ASSERT_EQ(Encode(0u, buffer_, Format::kOneTerminatedMostSignificant), 1u);
  EXPECT_EQ(buffer_[0], std::byte{0x80});

  ASSERT_EQ(Encode(15u, buffer_, Format::kOneTerminatedMostSignificant), 1u);
  EXPECT_EQ(buffer_[0], std::byte{0x8f});

  ASSERT_EQ(Encode(0x7f, buffer_, Format::kOneTerminatedMostSignificant), 1u);
  EXPECT_EQ(buffer_[0], std::byte{0xff});
}

TEST_F(VarintWithBuffer, EncodeWithOptions_MultiByte) {
  ASSERT_EQ(Encode(128u, buffer_, Format::kZeroTerminatedLeastSignificant), 2u);
  EXPECT_EQ(std::memcmp("\x01\x02", buffer_, 2), 0);

  ASSERT_EQ(
      Encode(0xffffffff, buffer_, Format::kZeroTerminatedLeastSignificant), 5u);
  EXPECT_EQ(std::memcmp("\xff\xff\xff\xff\x1e", buffer_, 5), 0);

  ASSERT_EQ(Encode(128u, buffer_, Format::kOneTerminatedLeastSignificant), 2u);
  EXPECT_EQ(std::memcmp("\x00\x03", buffer_, 2), 0);

  ASSERT_EQ(Encode(0xffffffff, buffer_, Format::kOneTerminatedLeastSignificant),
            5u);
  EXPECT_EQ(std::memcmp("\xfe\xfe\xfe\xfe\x1f", buffer_, 5), 0);

  ASSERT_EQ(Encode(128u, buffer_, Format::kZeroTerminatedMostSignificant), 2u);
  EXPECT_EQ(std::memcmp("\x80\x01", buffer_, 2), 0);

  ASSERT_EQ(Encode(0xffffffff, buffer_, Format::kZeroTerminatedMostSignificant),
            5u);
  EXPECT_EQ(std::memcmp("\xff\xff\xff\xff\x0f", buffer_, 5), 0);

  ASSERT_EQ(Encode(128u, buffer_, Format::kOneTerminatedMostSignificant), 2u);
  EXPECT_EQ(std::memcmp("\x00\x81", buffer_, 2), 0);

  ASSERT_EQ(Encode(0xffffffff, buffer_, Format::kOneTerminatedMostSignificant),
            5u);
  EXPECT_EQ(std::memcmp("\x7f\x7f\x7f\x7f\x8f", buffer_, 5), 0);
}

TEST(Varint, DecodeWithOptions_SingleByte) {
  uint64_t value = -1234;

  EXPECT_EQ(
      Decode(
          MakeBuffer("\x00"), &value, Format::kZeroTerminatedLeastSignificant),
      1u);
  EXPECT_EQ(value, 0u);

  EXPECT_EQ(
      Decode(
          MakeBuffer("\x04"), &value, Format::kZeroTerminatedLeastSignificant),
      1u);
  EXPECT_EQ(value, 2u);

  EXPECT_EQ(
      Decode(
          MakeBuffer("\xaa"), &value, Format::kZeroTerminatedLeastSignificant),
      1u);
  EXPECT_EQ(value, 85u);

  EXPECT_EQ(
      Decode(
          MakeBuffer("\x01"), &value, Format::kZeroTerminatedLeastSignificant),
      0u);

  EXPECT_EQ(
      Decode(
          MakeBuffer("\x01"), &value, Format::kOneTerminatedLeastSignificant),
      1u);
  EXPECT_EQ(value, 0u);

  EXPECT_EQ(
      Decode(
          MakeBuffer("\x13"), &value, Format::kOneTerminatedLeastSignificant),
      1u);
  EXPECT_EQ(value, 9u);

  EXPECT_EQ(
      Decode(
          MakeBuffer("\x00"), &value, Format::kOneTerminatedLeastSignificant),
      0u);

  EXPECT_EQ(
      Decode(
          MakeBuffer("\x00"), &value, Format::kZeroTerminatedMostSignificant),
      1u);
  EXPECT_EQ(value, 0u);

  EXPECT_EQ(
      Decode(
          MakeBuffer("\x04"), &value, Format::kZeroTerminatedMostSignificant),
      1u);
  EXPECT_EQ(value, 4u);

  EXPECT_EQ(
      Decode(
          MakeBuffer("\xff"), &value, Format::kZeroTerminatedMostSignificant),
      0u);

  EXPECT_EQ(
      Decode(MakeBuffer("\x80"), &value, Format::kOneTerminatedMostSignificant),
      1u);
  EXPECT_EQ(value, 0u);

  EXPECT_EQ(
      Decode(MakeBuffer("\x83"), &value, Format::kOneTerminatedMostSignificant),
      1u);
  EXPECT_EQ(value, 3u);

  EXPECT_EQ(
      Decode(MakeBuffer("\xaa"), &value, Format::kOneTerminatedMostSignificant),
      1u);
  EXPECT_EQ(value, 42u);

  EXPECT_EQ(
      Decode(MakeBuffer("\xff"), &value, Format::kOneTerminatedMostSignificant),
      1u);
  EXPECT_EQ(value, 127u);

  EXPECT_EQ(
      Decode(MakeBuffer("\x00"), &value, Format::kOneTerminatedMostSignificant),
      0u);
}

TEST(Varint, DecodeWithOptions_MultiByte) {
  uint64_t value = -1234;

  EXPECT_EQ(Decode(MakeBuffer("\x01\x10"),
                   &value,
                   Format::kZeroTerminatedLeastSignificant),
            2u);
  EXPECT_EQ(value, 1024u);

  EXPECT_EQ(Decode(MakeBuffer("\xff\xff\xff\xfe"),
                   &value,
                   Format::kZeroTerminatedLeastSignificant),
            4u);
  EXPECT_EQ(value, 0x0fffffffu);

  EXPECT_EQ(Decode(MakeBuffer("\x01\x01\x01\x01\x00"),
                   &value,
                   Format::kZeroTerminatedLeastSignificant),
            5u);
  EXPECT_EQ(value, 0u);

  EXPECT_EQ(Decode(MakeBuffer("\x82\x2d"),
                   &value,
                   Format::kOneTerminatedLeastSignificant),
            2u);
  EXPECT_EQ(value, 2881u);

  EXPECT_EQ(Decode(MakeBuffer("\xfe\xfe\xfe\xff"),
                   &value,
                   Format::kOneTerminatedLeastSignificant),
            4u);
  EXPECT_EQ(value, 0x0fffffffu);

  EXPECT_EQ(Decode(MakeBuffer("\x00\x00\x00\x00\x01"),
                   &value,
                   Format::kOneTerminatedLeastSignificant),
            5u);
  EXPECT_EQ(value, 0u);

  EXPECT_EQ(Decode(MakeBuffer("\x83\x6a"),
                   &value,
                   Format::kZeroTerminatedMostSignificant),
            2u);
  EXPECT_EQ(value, 0b1101010'0000011u);

  EXPECT_EQ(Decode(MakeBuffer("\xff\xff\xff\x7f"),
                   &value,
                   Format::kZeroTerminatedMostSignificant),
            4u);
  EXPECT_EQ(value, 0x0fffffffu);

  EXPECT_EQ(Decode(MakeBuffer("\x80\x80\x80\x80\x00"),
                   &value,
                   Format::kZeroTerminatedMostSignificant),
            5u);
  EXPECT_EQ(value, 0u);

  EXPECT_EQ(Decode(MakeBuffer("\x6a\x83"),
                   &value,
                   Format::kOneTerminatedMostSignificant),
            2u);
  EXPECT_EQ(value, 0b0000011'1101010u);

  EXPECT_EQ(Decode(MakeBuffer("\x7f\x7f\x7f\xff"),
                   &value,
                   Format::kOneTerminatedMostSignificant),
            4u);
  EXPECT_EQ(value, 0x0fffffffu);

  EXPECT_EQ(Decode(MakeBuffer("\x00\x00\x00\x00\x80"),
                   &value,
                   Format::kOneTerminatedMostSignificant),
            5u);
  EXPECT_EQ(value, 0u);
}

TEST(Varint, EncodedSize) {
  EXPECT_EQ(EncodedSize(uint64_t(0u)), 1u);
  EXPECT_EQ(EncodedSize(uint64_t(1u)), 1u);
  EXPECT_EQ(EncodedSize(uint64_t(127u)), 1u);
  EXPECT_EQ(EncodedSize(uint64_t(128u)), 2u);
  EXPECT_EQ(EncodedSize(uint64_t(16383u)), 2u);
  EXPECT_EQ(EncodedSize(uint64_t(16384u)), 3u);
  EXPECT_EQ(EncodedSize(uint64_t(2097151u)), 3u);
  EXPECT_EQ(EncodedSize(uint64_t(2097152u)), 4u);
  EXPECT_EQ(EncodedSize(uint64_t(268435455u)), 4u);
  EXPECT_EQ(EncodedSize(uint64_t(268435456u)), 5u);
  EXPECT_EQ(EncodedSize(uint64_t(34359738367u)), 5u);
  EXPECT_EQ(EncodedSize(uint64_t(34359738368u)), 6u);
  EXPECT_EQ(EncodedSize(uint64_t(4398046511103u)), 6u);
  EXPECT_EQ(EncodedSize(uint64_t(4398046511104u)), 7u);
  EXPECT_EQ(EncodedSize(uint64_t(562949953421311u)), 7u);
  EXPECT_EQ(EncodedSize(uint64_t(562949953421312u)), 8u);
  EXPECT_EQ(EncodedSize(uint64_t(72057594037927935u)), 8u);
  EXPECT_EQ(EncodedSize(uint64_t(72057594037927936u)), 9u);
  EXPECT_EQ(EncodedSize(uint64_t(9223372036854775807u)), 9u);
  EXPECT_EQ(EncodedSize(uint64_t(9223372036854775808u)), 10u);
  EXPECT_EQ(EncodedSize(std::numeric_limits<uint64_t>::max()), 10u);
  EXPECT_EQ(EncodedSize(std::numeric_limits<int64_t>::max()), 9u);
  EXPECT_EQ(EncodedSize(int64_t(-1)), 10u);
  EXPECT_EQ(EncodedSize(std::numeric_limits<int64_t>::min()), 10u);
}

TEST(Varint, ZigZagEncodedSize) {
  EXPECT_EQ(ZigZagEncodedSize(int64_t(0)), 1u);
  EXPECT_EQ(ZigZagEncodedSize(int64_t(-1)), 1u);
  EXPECT_EQ(ZigZagEncodedSize(int64_t(1)), 1u);
  EXPECT_EQ(ZigZagEncodedSize(int64_t(-64)), 1u);
  EXPECT_EQ(ZigZagEncodedSize(int64_t(-65)), 2u);
  EXPECT_EQ(ZigZagEncodedSize(int64_t(63)), 1u);
  EXPECT_EQ(ZigZagEncodedSize(int64_t(64)), 2u);
  EXPECT_EQ(ZigZagEncodedSize(std::numeric_limits<int8_t>::min()), 2u);
  EXPECT_EQ(ZigZagEncodedSize(std::numeric_limits<int8_t>::max()), 2u);
  EXPECT_EQ(ZigZagEncodedSize(std::numeric_limits<int16_t>::min()), 3u);
  EXPECT_EQ(ZigZagEncodedSize(std::numeric_limits<int16_t>::max()), 3u);
  EXPECT_EQ(ZigZagEncodedSize(std::numeric_limits<int32_t>::min()), 5u);
  EXPECT_EQ(ZigZagEncodedSize(std::numeric_limits<int32_t>::max()), 5u);
  EXPECT_EQ(ZigZagEncodedSize(std::numeric_limits<int64_t>::min()), 10u);
  EXPECT_EQ(ZigZagEncodedSize(std::numeric_limits<int64_t>::max()), 10u);
}

constexpr uint64_t CalculateMaxValueInBytes(size_t bytes) {
  uint64_t value = 0;
  for (size_t i = 0; i < bytes; ++i) {
    value |= uint64_t(0x7f) << (7 * i);
  }
  return value;
}

TEST(Varint, MaxValueInBytes) {
  static_assert(MaxValueInBytes(0) == 0);
  static_assert(MaxValueInBytes(1) == 0x7f);
  static_assert(MaxValueInBytes(2) == 0x3fff);
  static_assert(MaxValueInBytes(3) == 0x1fffff);
  static_assert(MaxValueInBytes(4) == 0x0fffffff);
  static_assert(MaxValueInBytes(5) == CalculateMaxValueInBytes(5));
  static_assert(MaxValueInBytes(6) == CalculateMaxValueInBytes(6));
  static_assert(MaxValueInBytes(7) == CalculateMaxValueInBytes(7));
  static_assert(MaxValueInBytes(8) == CalculateMaxValueInBytes(8));
  static_assert(MaxValueInBytes(9) == CalculateMaxValueInBytes(9));
  static_assert(MaxValueInBytes(10) == std::numeric_limits<uint64_t>::max());
  static_assert(MaxValueInBytes(11) == std::numeric_limits<uint64_t>::max());
  static_assert(MaxValueInBytes(100) == std::numeric_limits<uint64_t>::max());
}

}  // namespace
}  // namespace pw::varint
