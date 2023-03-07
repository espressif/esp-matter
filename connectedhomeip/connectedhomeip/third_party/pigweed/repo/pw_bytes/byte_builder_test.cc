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

#include "pw_bytes/byte_builder.h"

#include <array>
#include <cstddef>

#include "gtest/gtest.h"

using std::byte;

template <typename... Args>
constexpr std::array<byte, sizeof...(Args)> MakeBytes(Args... args) noexcept {
  return {static_cast<byte>(args)...};
}

namespace pw {
namespace {

TEST(ByteBuilder, EmptyBuffer_SizeAndMaxSizeAreCorrect) {
  ByteBuilder bb(ByteSpan{});

  EXPECT_TRUE(bb.empty());
  EXPECT_EQ(0u, bb.size());
  EXPECT_EQ(0u, bb.max_size());
}

TEST(ByteBuilder, NonEmptyBufferOfSize0_SizeAndMaxSizeAreCorrect) {
  std::array<byte, 3> buffer = MakeBytes(0x01, 0x02, 0x03);
  ByteBuilder bb(buffer);

  EXPECT_TRUE(bb.empty());
  EXPECT_EQ(0u, bb.size());
  EXPECT_EQ(3u, bb.max_size());
}

TEST(ByteBuilder, Constructor_InsertsEmptyBuffer) {
  std::array<byte, 3> buffer = MakeBytes(0x01, 0x02, 0x03);
  ByteBuilder bb(buffer);

  EXPECT_TRUE(bb.empty());
}

TEST(ByteBuilder, EmptyBuffer_Append) {
  ByteBuilder bb(ByteSpan{});
  EXPECT_TRUE(bb.empty());

  constexpr auto kBytesTestLiteral = MakeBytes(0x04, 0x05);

  EXPECT_FALSE(bb.append(kBytesTestLiteral.data(), 2).ok());
  EXPECT_EQ(0u, bb.size());
  EXPECT_EQ(0u, bb.max_size());
}

TEST(ByteBuilder, NonEmptyBufferOfSize0_Append) {
  std::array<byte, 3> buffer = MakeBytes(0x01, 0x02, 0x03);
  ByteBuilder bb(buffer);
  EXPECT_TRUE(bb.empty());

  constexpr auto kBytesTestLiteral = MakeBytes(0x04, 0x05);

  EXPECT_TRUE(bb.append(kBytesTestLiteral.data(), 2).ok());
  EXPECT_EQ(byte{0x04}, bb.data()[0]);
  EXPECT_EQ(byte{0x05}, bb.data()[1]);
}

TEST(ByteBuilder, NonEmptyBufferOfSize0_Append_Partial_NotResourceExhausted) {
  std::array<byte, 3> buffer = MakeBytes(0x01, 0x02, 0x03);
  ByteBuilder bb(buffer);

  EXPECT_TRUE(bb.empty());

  constexpr auto kBytesTestLiteral = MakeBytes(0x04, 0x05, 0x06, 0x07);

  EXPECT_TRUE(bb.append(kBytesTestLiteral.data(), 3).ok());
  EXPECT_EQ(byte{0x04}, bb.data()[0]);
  EXPECT_EQ(byte{0x05}, bb.data()[1]);
  EXPECT_EQ(byte{0x06}, bb.data()[2]);
}

TEST(ByteBuilder, NonEmptyBufferOfSize0_Append_Partial_ResourceExhausted) {
  std::array<byte, 3> buffer = MakeBytes(0x01, 0x02, 0x03);
  ByteBuilder bb(buffer);

  EXPECT_TRUE(bb.empty());

  constexpr auto kBytesTestLiteral = MakeBytes(0x04, 0x05, 0x06, 0x07);

  EXPECT_FALSE(bb.append(kBytesTestLiteral.data(), 4).ok());
  EXPECT_EQ(Status::ResourceExhausted(), bb.status());
  EXPECT_EQ(0u, bb.size());
}

TEST(ByteBuilder, Append_RepeatedBytes) {
  ByteBuffer<8> bb;
  EXPECT_TRUE(bb.empty());

  EXPECT_TRUE(bb.append(7, byte{0x04}).ok());

  for (size_t i = 0; i < 7; i++) {
    EXPECT_EQ(byte{0x04}, bb.data()[i]);
  }
}

TEST(ByteBuilder, Append_Bytes_Full) {
  ByteBuffer<8> bb;

  EXPECT_EQ(8u, bb.max_size() - bb.size());

  EXPECT_TRUE(bb.append(8, byte{0x04}).ok());

  for (size_t i = 0; i < 8; i++) {
    EXPECT_EQ(byte{0x04}, bb.data()[i]);
  }
}

TEST(ByteBuilder, Append_Bytes_Exhausted) {
  ByteBuffer<8> bb;

  EXPECT_EQ(Status::ResourceExhausted(), bb.append(9, byte{0x04}).status());
  EXPECT_EQ(0u, bb.size());
}

TEST(ByteBuilder, Append_Partial) {
  std::array<byte, 3> buffer = MakeBytes(0x01, 0x02, 0x03);
  ByteBuffer<12> bb;

  EXPECT_TRUE(bb.append(buffer.data(), 2).ok());
  EXPECT_EQ(2u, bb.size());
  EXPECT_EQ(byte{0x01}, bb.data()[0]);
  EXPECT_EQ(byte{0x02}, bb.data()[1]);
}

TEST(ByteBuilder, EmptyBuffer_Resize_WritesNothing) {
  std::array<byte, 3> buffer = MakeBytes(0x01, 0x02, 0x03);
  ByteBuilder bb(buffer);

  bb.resize(0);
  EXPECT_TRUE(bb.ok());
}

TEST(ByteBuilder, EmptyBuffer_Resize_Larger_Fails) {
  std::array<byte, 3> buffer = MakeBytes(0x01, 0x02, 0x03);
  ByteBuilder bb(buffer);

  bb.resize(1);
  EXPECT_EQ(Status::OutOfRange(), bb.append(9, byte{0x04}).status());
}

TEST(ByteBuilder, Resize_Smaller) {
  std::array<byte, 3> buffer = MakeBytes(0x01, 0x02, 0x03);
  ByteBuffer<8> bb;

  EXPECT_TRUE(bb.append(buffer).ok());

  bb.resize(1);
  EXPECT_TRUE(bb.ok());
  EXPECT_EQ(1u, bb.size());
  EXPECT_EQ(byte{0x01}, bb.data()[0]);
}

TEST(ByteBuilder, Resize_Clear) {
  std::array<byte, 3> buffer = MakeBytes(0x01, 0x02, 0x03);
  ByteBuffer<8> bb;

  EXPECT_TRUE(bb.append(buffer).ok());

  bb.resize(0);
  EXPECT_TRUE(bb.ok());
  EXPECT_EQ(0u, bb.size());
  EXPECT_TRUE(bb.empty());
}

TEST(ByteBuilder, Resize_Larger_Fails) {
  std::array<byte, 3> buffer = MakeBytes(0x01, 0x02, 0x03);
  ByteBuffer<8> bb;

  EXPECT_TRUE(bb.append(buffer).ok());

  EXPECT_EQ(3u, bb.size());
  bb.resize(5);
  EXPECT_EQ(3u, bb.size());
  EXPECT_EQ(bb.status(), Status::OutOfRange());
}

TEST(ByteBuilder, Status_StartsOk) {
  ByteBuffer<16> bb;
  EXPECT_EQ(OkStatus(), bb.status());
}

TEST(ByteBuilder, Status_StatusUpdate) {
  std::array<byte, 3> buffer = MakeBytes(0x01, 0x02, 0x03);
  ByteBuffer<2> bb;

  EXPECT_FALSE(bb.append(buffer).ok());
  EXPECT_EQ(Status::ResourceExhausted(), bb.status());

  bb.resize(4);
  EXPECT_EQ(Status::OutOfRange(), bb.status());

  EXPECT_FALSE(bb.append(buffer.data(), 0).ok());
  EXPECT_EQ(Status::OutOfRange(), bb.status());
}

TEST(ByteBuilder, Status_ClearStatus_SetsStatusToOk) {
  std::array<byte, 3> buffer = MakeBytes(0x01, 0x02, 0x03);
  ByteBuffer<2> bb;

  EXPECT_FALSE(bb.append(buffer).ok());
  EXPECT_EQ(Status::ResourceExhausted(), bb.status());

  bb.clear_status();
  EXPECT_EQ(OkStatus(), bb.status());
}

TEST(ByteBuilder, PushBack) {
  ByteBuffer<12> bb;
  bb.push_back(byte{0x01});
  EXPECT_EQ(OkStatus(), bb.status());
  EXPECT_EQ(1u, bb.size());
  EXPECT_EQ(byte{0x01}, bb.data()[0]);
}

TEST(ByteBuilder, PushBack_Full) {
  ByteBuffer<1> bb;
  bb.push_back(byte{0x01});
  EXPECT_EQ(OkStatus(), bb.status());
  EXPECT_EQ(1u, bb.size());
}

TEST(ByteBuilder, PushBack_Full_ResourceExhausted) {
  ByteBuffer<1> bb;
  bb.push_back(byte{0x01});
  bb.push_back(byte{0x01});

  EXPECT_EQ(Status::ResourceExhausted(), bb.status());
  EXPECT_EQ(1u, bb.size());
}

TEST(ByteBuilder, PopBack) {
  std::array<byte, 3> buffer = MakeBytes(0x01, 0x02, 0x03);
  ByteBuffer<3> bb;

  bb.append(buffer.data(), 3);

  bb.pop_back();
  EXPECT_EQ(OkStatus(), bb.status());
  EXPECT_EQ(2u, bb.size());
  EXPECT_EQ(byte{0x01}, bb.data()[0]);
  EXPECT_EQ(byte{0x02}, bb.data()[1]);
}

TEST(ByteBuilder, PopBack_Empty) {
  std::array<byte, 3> buffer = MakeBytes(0x01, 0x02, 0x03);
  ByteBuffer<3> bb;
  bb.append(buffer.data(), 3);

  bb.pop_back();
  bb.pop_back();
  bb.pop_back();
  EXPECT_EQ(OkStatus(), bb.status());
  EXPECT_EQ(0u, bb.size());
  EXPECT_TRUE(bb.empty());
}

TEST(ByteBuffer, Assign) {
  std::array<byte, 3> buffer = MakeBytes(0x01, 0x02, 0x03);
  ByteBuffer<10> one;
  ByteBuffer<10> two;

  one.append(buffer.data(), 3);
  EXPECT_EQ(byte{0x01}, one.data()[0]);
  EXPECT_EQ(byte{0x02}, one.data()[1]);
  EXPECT_EQ(byte{0x03}, one.data()[2]);

  two = one;
  EXPECT_EQ(byte{0x01}, two.data()[0]);
  EXPECT_EQ(byte{0x02}, two.data()[1]);
  EXPECT_EQ(byte{0x03}, two.data()[2]);

  constexpr auto kBytesTestLiteral = MakeBytes(0x04, 0x05, 0x06, 0x07);
  one.append(kBytesTestLiteral.data(), 2);
  two.append(kBytesTestLiteral.data(), 4);
  EXPECT_EQ(5u, one.size());
  EXPECT_EQ(7u, two.size());
  EXPECT_EQ(byte{0x04}, one.data()[3]);
  EXPECT_EQ(byte{0x05}, one.data()[4]);
  EXPECT_EQ(byte{0x04}, two.data()[3]);
  EXPECT_EQ(byte{0x05}, two.data()[4]);
  EXPECT_EQ(byte{0x06}, two.data()[5]);
  EXPECT_EQ(byte{0x07}, two.data()[6]);

  two.push_back(byte{0x01});
  two.push_back(byte{0x01});
  two.push_back(byte{0x01});
  two.push_back(byte{0x01});
  ASSERT_EQ(Status::ResourceExhausted(), two.status());

  one = two;
  EXPECT_EQ(byte{0x01}, two.data()[7]);
  EXPECT_EQ(byte{0x01}, two.data()[8]);
  EXPECT_EQ(byte{0x01}, two.data()[9]);
  EXPECT_EQ(Status::ResourceExhausted(), one.status());
}

TEST(ByteBuffer, CopyConstructFromSameSize) {
  ByteBuffer<10> one;
  std::array<byte, 3> buffer = MakeBytes(0x01, 0x02, 0x03);

  one.append(buffer.data(), 3);
  EXPECT_EQ(byte{0x01}, one.data()[0]);
  EXPECT_EQ(byte{0x02}, one.data()[1]);
  EXPECT_EQ(byte{0x03}, one.data()[2]);

  ByteBuffer<10> two(one);
  EXPECT_EQ(byte{0x01}, two.data()[0]);
  EXPECT_EQ(byte{0x02}, two.data()[1]);
  EXPECT_EQ(byte{0x03}, two.data()[2]);
}

TEST(ByteBuffer, CopyConstructFromSmaller) {
  std::array<byte, 3> buffer = MakeBytes(0x01, 0x02, 0x03);
  ByteBuffer<2> one;
  one.append(buffer.data(), 2);
  ByteBuffer<3> two(one);

  EXPECT_EQ(byte{0x01}, two.data()[0]);
  EXPECT_EQ(byte{0x02}, two.data()[1]);
  EXPECT_EQ(OkStatus(), two.status());
}

TEST(ByteBuilder, ResizeError_NoDataAddedAfter) {
  std::array<byte, 3> buffer = MakeBytes(0x01, 0x02, 0x03);
  ByteBuffer<8> bb;

  EXPECT_TRUE(bb.append(buffer).ok());

  EXPECT_EQ(3u, bb.size());
  bb.resize(5);
  EXPECT_EQ(3u, bb.size());
  EXPECT_EQ(bb.status(), Status::OutOfRange());

  bb.PutInt8(0xFE);
  EXPECT_EQ(3u, bb.size());
  EXPECT_EQ(bb.status(), Status::OutOfRange());
}

TEST(ByteBuilder, AddingNoBytesToZeroSizedByteBuffer) {
  std::array<byte, 3> buffer = MakeBytes(0x01, 0x02, 0x03);
  ByteBuffer<0> bb;

  EXPECT_TRUE(bb.append(buffer.data(), 0).ok());
  EXPECT_EQ(0u, bb.size());
}

TEST(ByteBuffer, Putting8ByteInts_Full) {
  ByteBuffer<2> bb;
  bb.PutInt8(0xFE);
  bb.PutUint8(0x02);

  EXPECT_EQ(byte{0xFE}, bb.data()[0]);
  EXPECT_EQ(byte{0x02}, bb.data()[1]);
  EXPECT_EQ(OkStatus(), bb.status());
}

TEST(ByteBuffer, Putting8ByteInts_Exhausted) {
  ByteBuffer<2> bb;
  bb.PutInt8(0xFE);
  bb.PutUint8(0x02);
  bb.PutUint8(0x05);

  EXPECT_EQ(byte{0xFE}, bb.data()[0]);
  EXPECT_EQ(byte{0x02}, bb.data()[1]);
  EXPECT_EQ(Status::ResourceExhausted(), bb.status());
}

TEST(ByteBuffer, Putting16ByteInts_Full_kLittleEndian) {
  ByteBuffer<4> bb;
  bb.PutInt16(0xFFF7);
  bb.PutUint16(0x0008);

  EXPECT_EQ(byte{0xF7}, bb.data()[0]);
  EXPECT_EQ(byte{0xFF}, bb.data()[1]);
  EXPECT_EQ(byte{0x08}, bb.data()[2]);
  EXPECT_EQ(byte{0x00}, bb.data()[3]);

  EXPECT_EQ(OkStatus(), bb.status());
}

TEST(ByteBuffer, Putting16ByteInts_Exhausted_kBigEndian) {
  ByteBuffer<5> bb;
  bb.PutInt16(0xFFF7, endian::big);
  bb.PutUint16(0x0008, endian::big);

  EXPECT_EQ(byte{0xFF}, bb.data()[0]);
  EXPECT_EQ(byte{0xF7}, bb.data()[1]);
  EXPECT_EQ(byte{0x00}, bb.data()[2]);
  EXPECT_EQ(byte{0x08}, bb.data()[3]);

  bb.PutInt16(0xFAFA, endian::big);
  EXPECT_EQ(4u, bb.size());
  EXPECT_EQ(Status::ResourceExhausted(), bb.status());
}

TEST(ByteBuffer, Putting32ByteInts_Full_kLittleEndian) {
  ByteBuffer<8> bb;
  bb.PutInt32(0xFFFFFFF1);
  bb.PutUint32(0x00000014);

  EXPECT_EQ(byte{0xF1}, bb.data()[0]);
  EXPECT_EQ(byte{0xFF}, bb.data()[1]);
  EXPECT_EQ(byte{0xFF}, bb.data()[2]);
  EXPECT_EQ(byte{0xFF}, bb.data()[3]);
  EXPECT_EQ(byte{0x14}, bb.data()[4]);
  EXPECT_EQ(byte{0x00}, bb.data()[5]);
  EXPECT_EQ(byte{0x00}, bb.data()[6]);
  EXPECT_EQ(byte{0x00}, bb.data()[7]);

  EXPECT_EQ(OkStatus(), bb.status());
}

TEST(ByteBuffer, Putting32ByteInts_Exhausted_kBigEndian) {
  ByteBuffer<10> bb;
  bb.PutInt32(0xF92927B2, endian::big);
  bb.PutUint32(0x0C90739E, endian::big);

  EXPECT_EQ(byte{0xF9}, bb.data()[0]);
  EXPECT_EQ(byte{0x29}, bb.data()[1]);
  EXPECT_EQ(byte{0x27}, bb.data()[2]);
  EXPECT_EQ(byte{0xB2}, bb.data()[3]);
  EXPECT_EQ(byte{0x0C}, bb.data()[4]);
  EXPECT_EQ(byte{0x90}, bb.data()[5]);
  EXPECT_EQ(byte{0x73}, bb.data()[6]);
  EXPECT_EQ(byte{0x9E}, bb.data()[7]);

  bb.PutInt32(-114743374, endian::big);
  EXPECT_EQ(8u, bb.size());
  EXPECT_EQ(Status::ResourceExhausted(), bb.status());
}

TEST(ByteBuffer, Putting64ByteInts_Full_kLittleEndian) {
  ByteBuffer<16> bb;
  bb.PutInt64(0x000001E8A7A0D569);
  bb.PutUint64(0xFFFFFE17585F2A97);

  EXPECT_EQ(byte{0x69}, bb.data()[0]);
  EXPECT_EQ(byte{0xD5}, bb.data()[1]);
  EXPECT_EQ(byte{0xA0}, bb.data()[2]);
  EXPECT_EQ(byte{0xA7}, bb.data()[3]);
  EXPECT_EQ(byte{0xE8}, bb.data()[4]);
  EXPECT_EQ(byte{0x01}, bb.data()[5]);
  EXPECT_EQ(byte{0x00}, bb.data()[6]);
  EXPECT_EQ(byte{0x00}, bb.data()[7]);
  EXPECT_EQ(byte{0x97}, bb.data()[8]);
  EXPECT_EQ(byte{0x2A}, bb.data()[9]);
  EXPECT_EQ(byte{0x5F}, bb.data()[10]);
  EXPECT_EQ(byte{0x58}, bb.data()[11]);
  EXPECT_EQ(byte{0x17}, bb.data()[12]);
  EXPECT_EQ(byte{0xFE}, bb.data()[13]);
  EXPECT_EQ(byte{0xFF}, bb.data()[14]);
  EXPECT_EQ(byte{0xFF}, bb.data()[15]);

  EXPECT_EQ(OkStatus(), bb.status());
}

TEST(ByteBuffer, Putting64ByteInts_Exhausted_kBigEndian) {
  ByteBuffer<20> bb;
  bb.PutUint64(0x000001E8A7A0D569, endian::big);
  bb.PutInt64(0xFFFFFE17585F2A97, endian::big);

  EXPECT_EQ(byte{0x00}, bb.data()[0]);
  EXPECT_EQ(byte{0x00}, bb.data()[1]);
  EXPECT_EQ(byte{0x01}, bb.data()[2]);
  EXPECT_EQ(byte{0xE8}, bb.data()[3]);
  EXPECT_EQ(byte{0xA7}, bb.data()[4]);
  EXPECT_EQ(byte{0xA0}, bb.data()[5]);
  EXPECT_EQ(byte{0xD5}, bb.data()[6]);
  EXPECT_EQ(byte{0x69}, bb.data()[7]);
  EXPECT_EQ(byte{0xFF}, bb.data()[8]);
  EXPECT_EQ(byte{0xFF}, bb.data()[9]);
  EXPECT_EQ(byte{0xFE}, bb.data()[10]);
  EXPECT_EQ(byte{0x17}, bb.data()[11]);
  EXPECT_EQ(byte{0x58}, bb.data()[12]);
  EXPECT_EQ(byte{0x5F}, bb.data()[13]);
  EXPECT_EQ(byte{0x2A}, bb.data()[14]);
  EXPECT_EQ(byte{0x97}, bb.data()[15]);

  bb.PutInt64(-6099875637501324530, endian::big);
  EXPECT_EQ(16u, bb.size());
  EXPECT_EQ(Status::ResourceExhausted(), bb.status());
}

TEST(ByteBuffer, PuttingInts_MixedTypes_MixedEndian) {
  ByteBuffer<16> bb;
  bb.PutUint8(0x03);
  bb.PutInt16(0xFD6D, endian::big);
  bb.PutUint32(0x482B3D9E);
  bb.PutInt64(0x9A1C3641843DF317, endian::big);
  bb.PutInt8(0xFB);

  EXPECT_EQ(byte{0x03}, bb.data()[0]);
  EXPECT_EQ(byte{0xFD}, bb.data()[1]);
  EXPECT_EQ(byte{0x6D}, bb.data()[2]);
  EXPECT_EQ(byte{0x9E}, bb.data()[3]);
  EXPECT_EQ(byte{0x3D}, bb.data()[4]);
  EXPECT_EQ(byte{0x2B}, bb.data()[5]);
  EXPECT_EQ(byte{0x48}, bb.data()[6]);
  EXPECT_EQ(byte{0x9A}, bb.data()[7]);
  EXPECT_EQ(byte{0x1C}, bb.data()[8]);
  EXPECT_EQ(byte{0x36}, bb.data()[9]);
  EXPECT_EQ(byte{0x41}, bb.data()[10]);
  EXPECT_EQ(byte{0x84}, bb.data()[11]);
  EXPECT_EQ(byte{0x3D}, bb.data()[12]);
  EXPECT_EQ(byte{0xF3}, bb.data()[13]);
  EXPECT_EQ(byte{0x17}, bb.data()[14]);
  EXPECT_EQ(byte{0xFB}, bb.data()[15]);

  EXPECT_EQ(OkStatus(), bb.status());
}

TEST(ByteBuffer, Iterator) {
  std::array<byte, 3> buffer = MakeBytes(0x01, 0x02, 0x03);
  ByteBuffer<8> bb;
  EXPECT_TRUE(bb.append(buffer).ok());

  auto it = bb.begin();
  EXPECT_EQ(*it, byte{0x01});

  ++it;
  EXPECT_EQ(*it, byte{0x02});
  EXPECT_EQ(it - bb.begin(), 1);

  ++it;
  EXPECT_EQ(*it, byte{0x03});
  EXPECT_EQ(it - bb.begin(), 2);

  ++it;
  EXPECT_EQ(it, bb.end());
  EXPECT_EQ(static_cast<size_t>(it - bb.begin()), bb.size());
}

TEST(ByteBuffer, Iterator_PreIncrement) {
  std::array<byte, 3> buffer = MakeBytes(0x01, 0x02, 0x03);
  ByteBuffer<8> bb;
  EXPECT_TRUE(bb.append(buffer).ok());

  auto it = bb.begin();
  EXPECT_EQ(*(++it), byte{0x02});
}

TEST(ByteBuffer, Iterator_PostIncrement) {
  std::array<byte, 3> buffer = MakeBytes(0x01, 0x02, 0x03);
  ByteBuffer<8> bb;
  EXPECT_TRUE(bb.append(buffer).ok());

  auto it = bb.begin();
  EXPECT_EQ(*(it++), byte{0x01});
  EXPECT_EQ(*it, byte{0x02});
  EXPECT_EQ(*(it++), byte{0x02});
  EXPECT_EQ(*it, byte{0x03});
}

TEST(ByteBuffer, Iterator_PreDecrement) {
  std::array<byte, 3> buffer = MakeBytes(0x01, 0x02, 0x03);
  ByteBuffer<8> bb;
  EXPECT_TRUE(bb.append(buffer).ok());
  auto it = bb.begin();

  EXPECT_EQ(*it, byte{0x01});
  ++it;
  EXPECT_EQ(*it, byte{0x02});
  ++it;
  EXPECT_EQ(*it, byte{0x03});
  ++it;
  EXPECT_EQ(it, bb.end());

  --it;
  EXPECT_EQ(*it, byte{0x03});
  --it;
  EXPECT_EQ(*it, byte{0x02});
  --it;
  EXPECT_EQ(*it, byte{0x01});
  EXPECT_EQ(it, bb.begin());
}

TEST(ByteBuffer, Iterator_PostDecrement) {
  std::array<byte, 3> buffer = MakeBytes(0x01, 0x02, 0x03);
  ByteBuffer<8> bb;
  EXPECT_TRUE(bb.append(buffer).ok());
  auto it = bb.begin();

  it += 2;
  EXPECT_EQ(*it, byte{0x03});

  EXPECT_EQ(*(it--), byte{0x03});
  EXPECT_EQ(*it, byte{0x02});
  EXPECT_EQ(*(it--), byte{0x02});
  EXPECT_EQ(*it, byte{0x01});
}

TEST(ByteBuffer, Iterator_PlusEquals) {
  std::array<byte, 3> buffer = MakeBytes(0x01, 0x02, 0x03);
  ByteBuffer<8> bb;
  EXPECT_TRUE(bb.append(buffer).ok());
  auto it = bb.begin();

  it += 2;
  EXPECT_EQ(*it, byte{0x03});

  it += -1;
  EXPECT_EQ(*it, byte{0x02});

  it += 1;
  EXPECT_EQ(*it, byte{0x03});

  it += -2;
  EXPECT_EQ(*it, byte{0x01});
}

TEST(ByteBuffer, Iterator_MinusEquals) {
  std::array<byte, 3> buffer = MakeBytes(0x01, 0x02, 0x03);
  ByteBuffer<8> bb;
  EXPECT_TRUE(bb.append(buffer).ok());
  auto it = bb.begin();

  it -= -2;
  EXPECT_EQ(*it, byte{0x03});

  it -= +1;
  EXPECT_EQ(*it, byte{0x02});

  it -= -1;
  EXPECT_EQ(*it, byte{0x03});

  it -= +2;
  EXPECT_EQ(*it, byte{0x01});
}

TEST(ByteBuffer, Iterator_Plus) {
  std::array<byte, 3> buffer = MakeBytes(0x01, 0x02, 0x03);
  ByteBuffer<8> bb;
  EXPECT_TRUE(bb.append(buffer).ok());
  auto it = bb.begin();

  auto test = it + 2;
  EXPECT_EQ(*test, byte{0x03});

  test = test + -1;
  EXPECT_EQ(*test, byte{0x02});

  test = test + 1;
  EXPECT_EQ(*test, byte{0x03});

  test = test + (-2);
  EXPECT_EQ(*test, byte{0x01});
}

TEST(ByteBuffer, Iterator_Minus) {
  std::array<byte, 3> buffer = MakeBytes(0x01, 0x02, 0x03);
  ByteBuffer<8> bb;
  EXPECT_TRUE(bb.append(buffer).ok());
  auto it = bb.begin();

  auto test = it - (-2);
  EXPECT_EQ(*test, byte{0x03});

  test = test - 1;
  EXPECT_EQ(*test, byte{0x02});

  test = test - (-1);
  EXPECT_EQ(*test, byte{0x03});

  test = test - 2;
  EXPECT_EQ(*test, byte{0x01});
}

TEST(ByteBuffer, Iterator_LessThan) {
  std::array<byte, 3> buffer = MakeBytes(0x01, 0x02, 0x03);
  ByteBuffer<8> bb;
  EXPECT_TRUE(bb.append(buffer).ok());

  auto it_1 = bb.begin();
  auto it_2 = it_1 + 2;
  EXPECT_EQ(*it_1, byte{0x01});
  EXPECT_EQ(*it_2, byte{0x03});
  EXPECT_TRUE(it_1 < it_2);

  it_1++;
  it_2--;
  EXPECT_EQ(*it_1, byte{0x02});
  EXPECT_EQ(*it_2, byte{0x02});
  EXPECT_FALSE(it_1 < it_2);

  it_1++;
  it_2--;
  EXPECT_EQ(*it_1, byte{0x03});
  EXPECT_EQ(*it_2, byte{0x01});
  EXPECT_FALSE(it_1 < it_2);
  EXPECT_TRUE(it_2 < it_1);
}

TEST(ByteBuffer, Iterator_GreaterThan) {
  std::array<byte, 3> buffer = MakeBytes(0x01, 0x02, 0x03);
  ByteBuffer<8> bb;
  EXPECT_TRUE(bb.append(buffer).ok());

  auto it_1 = bb.begin();
  auto it_2 = it_1 + 2;
  EXPECT_EQ(*it_1, byte{0x01});
  EXPECT_EQ(*it_2, byte{0x03});
  EXPECT_FALSE(it_1 > it_2);

  it_1++;
  it_2--;
  EXPECT_EQ(*it_1, byte{0x02});
  EXPECT_EQ(*it_2, byte{0x02});
  EXPECT_FALSE(it_1 > it_2);

  it_1++;
  it_2--;
  EXPECT_EQ(*it_1, byte{0x03});
  EXPECT_EQ(*it_2, byte{0x01});
  EXPECT_TRUE(it_1 > it_2);
  EXPECT_FALSE(it_2 > it_1);
}

TEST(ByteBuffer, Iterator_LessThanEqual_GreaterThanEqual) {
  std::array<byte, 3> buffer = MakeBytes(0x01, 0x02, 0x03);
  ByteBuffer<8> bb;
  EXPECT_TRUE(bb.append(buffer).ok());

  auto it_1 = bb.begin();
  auto it_2 = it_1 + 2;
  EXPECT_EQ(*it_1, byte{0x01});
  EXPECT_EQ(*it_2, byte{0x03});
  EXPECT_FALSE(it_1 >= it_2);
  EXPECT_TRUE(it_1 <= it_2);

  it_1++;
  it_2--;
  EXPECT_EQ(*it_1, byte{0x02});
  EXPECT_EQ(*it_2, byte{0x02});
  EXPECT_TRUE(it_1 >= it_2);
  EXPECT_TRUE(it_1 <= it_2);

  it_1++;
  it_2--;
  EXPECT_EQ(*it_1, byte{0x03});
  EXPECT_EQ(*it_2, byte{0x01});
  EXPECT_FALSE(it_1 <= it_2);
  EXPECT_TRUE(it_1 >= it_2);
}

TEST(ByteBuffer, Iterator_Indexing) {
  std::array<byte, 3> buffer = MakeBytes(0x01, 0x02, 0x03);
  ByteBuffer<8> bb;
  EXPECT_TRUE(bb.append(buffer).ok());

  auto it = bb.begin();
  EXPECT_EQ(it[0], byte{0x01});
  EXPECT_EQ(it[1], byte{0x02});
  EXPECT_EQ(it[2], byte{0x03});
}

TEST(ByteBuffer, Iterator_PeekValues_1Byte) {
  ByteBuffer<3> bb;
  bb.PutInt8(0xF2);
  bb.PutUint8(0xE5);
  bb.PutInt8(0x5F);

  auto it = bb.begin();
  EXPECT_EQ(it.PeekInt8(), int8_t(0xF2));
  it = it + 1;
  EXPECT_EQ(it.PeekUint8(), uint8_t(0xE5));
  it = it + 1;
  EXPECT_EQ(it.PeekInt8(), int8_t(0x5F));
}

TEST(ByteBuffer, Iterator_PeekValues_2Bytes) {
  ByteBuffer<4> bb;
  bb.PutInt16(0xA7F1);
  bb.PutUint16(0xF929, endian::big);

  auto it = bb.begin();
  EXPECT_EQ(it.PeekInt16(), int16_t(0xA7F1));
  it = it + 2;
  EXPECT_EQ(it.PeekUint16(endian::big), uint16_t(0xF929));
}

TEST(ByteBuffer, Iterator_PeekValues_4Bytes) {
  ByteBuffer<8> bb;
  bb.PutInt32(0xFFFFFFF1);
  bb.PutUint32(0xF92927B2, endian::big);

  auto it = bb.begin();
  EXPECT_EQ(it.PeekInt32(), int32_t(0xFFFFFFF1));
  it = it + 4;
  EXPECT_EQ(it.PeekUint32(endian::big), uint32_t(0xF92927B2));
}

TEST(ByteBuffer, Iterator_PeekValues_8Bytes) {
  ByteBuffer<16> bb;
  bb.PutUint64(0x000001E8A7A0D569);
  bb.PutInt64(0xFFFFFE17585F2A97, endian::big);

  auto it = bb.begin();
  EXPECT_EQ(it.PeekUint64(), uint64_t(0x000001E8A7A0D569));
  it = it + 8;
  EXPECT_EQ(it.PeekInt64(endian::big), int64_t(0xFFFFFE17585F2A97));
}

TEST(ByteBuffer, Iterator_ReadValues_1Byte) {
  ByteBuffer<3> bb;
  bb.PutInt8(0xF2);
  bb.PutUint8(0xE5);
  bb.PutInt8(0x5F);

  auto it = bb.begin();
  EXPECT_EQ(it.ReadInt8(), int8_t(0xF2));
  EXPECT_EQ(it.ReadUint8(), uint8_t(0xE5));
  EXPECT_EQ(it.ReadInt8(), int8_t(0x5F));
}

TEST(ByteBuffer, Iterator_ReadValues_2Bytes) {
  ByteBuffer<4> bb;
  bb.PutInt16(0xA7F1);
  bb.PutUint16(0xF929, endian::big);

  auto it = bb.begin();
  EXPECT_EQ(it.ReadInt16(), int16_t(0xA7F1));
  EXPECT_EQ(it.ReadUint16(endian::big), uint16_t(0xF929));
}

TEST(ByteBuffer, Iterator_ReadValues_4Bytes) {
  ByteBuffer<8> bb;
  bb.PutInt32(0xFFFFFFF1);
  bb.PutUint32(0xF92927B2, endian::big);

  auto it = bb.begin();
  EXPECT_EQ(it.ReadInt32(), int32_t(0xFFFFFFF1));
  EXPECT_EQ(it.ReadUint32(endian::big), uint32_t(0xF92927B2));
}

TEST(ByteBuffer, Iterator_ReadValues_8Bytes) {
  ByteBuffer<16> bb;
  bb.PutUint64(0x000001E8A7A0D569);
  bb.PutInt64(0xFFFFFE17585F2A97, endian::big);

  auto it = bb.begin();
  EXPECT_EQ(it.ReadUint64(), uint64_t(0x000001E8A7A0D569));
  EXPECT_EQ(it.ReadInt64(endian::big), int64_t(0xFFFFFE17585F2A97));
}

TEST(ByteBuffer, ConvertsToSpan) {
  ByteBuffer<16> bb;
  bb.push_back(std::byte{210});

  span<const std::byte> byte_span(bb);
  EXPECT_EQ(byte_span.data(), bb.data());
  EXPECT_EQ(byte_span.size(), bb.size());
  EXPECT_EQ(byte_span[0], std::byte{210});
}

}  // namespace
}  // namespace pw
