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

#include "pw_varint/stream.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>

#include "gtest/gtest.h"
#include "pw_stream/memory_stream.h"
#include "pw_varint/varint.h"

namespace pw::varint {
namespace {
template <size_t kStringSize>
auto MakeBuffer(const char (&data)[kStringSize]) {
  constexpr size_t kSizeBytes = kStringSize - 1;
  static_assert(kSizeBytes <= 10, "Varint arrays never need be larger than 10");

  std::array<std::byte, kSizeBytes> array;
  std::memcpy(array.data(), data, kSizeBytes);
  return array;
}
}  // namespace

TEST(VarintRead, Signed64_SingleByte) {
  int64_t value = -1234;

  {
    const auto buffer = MakeBuffer("\x00");
    stream::MemoryReader reader(buffer);
    const auto sws = Read(reader, &value);
    EXPECT_TRUE(sws.ok());
    EXPECT_EQ(sws.size(), 1u);
    EXPECT_EQ(value, 0);
  }

  {
    const auto buffer = MakeBuffer("\x01");
    stream::MemoryReader reader(buffer);
    const auto sws = Read(reader, &value);
    EXPECT_TRUE(sws.ok());
    EXPECT_EQ(sws.size(), 1u);
    EXPECT_EQ(value, -1);
  }

  {
    const auto buffer = MakeBuffer("\x02");
    stream::MemoryReader reader(buffer);
    const auto sws = Read(reader, &value);
    EXPECT_TRUE(sws.ok());
    EXPECT_EQ(sws.size(), 1u);
    EXPECT_EQ(value, 1);
  }

  {
    const auto buffer = MakeBuffer("\x03");
    stream::MemoryReader reader(buffer);
    const auto sws = Read(reader, &value);
    EXPECT_TRUE(sws.ok());
    EXPECT_EQ(sws.size(), 1u);
    EXPECT_EQ(value, -2);
  }

  {
    const auto buffer = MakeBuffer("\x04");
    stream::MemoryReader reader(buffer);
    const auto sws = Read(reader, &value);
    EXPECT_TRUE(sws.ok());
    EXPECT_EQ(sws.size(), 1u);
    EXPECT_EQ(value, 2);
  }
}

TEST(VarintRead, Signed64_MultiByte) {
  int64_t value = -1234;

  {
    const auto buffer = MakeBuffer("\x80\x01");
    stream::MemoryReader reader(buffer);
    const auto sws = Read(reader, &value);
    EXPECT_TRUE(sws.ok());
    EXPECT_EQ(sws.size(), 2u);
    EXPECT_EQ(value, 64);
  }

  {
    const auto buffer = MakeBuffer("\x81\x01");
    stream::MemoryReader reader(buffer);
    const auto sws = Read(reader, &value);
    EXPECT_TRUE(sws.ok());
    EXPECT_EQ(sws.size(), 2u);
    EXPECT_EQ(value, -65);
  }

  {
    const auto buffer = MakeBuffer("\x82\x01");
    stream::MemoryReader reader(buffer);
    const auto sws = Read(reader, &value);
    EXPECT_TRUE(sws.ok());
    EXPECT_EQ(sws.size(), 2u);
    EXPECT_EQ(value, 65);
  }

  {
    const auto buffer = MakeBuffer("\xff\xff\xff\xff\x0f");
    stream::MemoryReader reader(buffer);
    const auto sws = Read(reader, &value);
    EXPECT_TRUE(sws.ok());
    EXPECT_EQ(sws.size(), 5u);
    EXPECT_EQ(value, std::numeric_limits<int32_t>::min());
  }

  {
    const auto buffer = MakeBuffer("\xfe\xff\xff\xff\x0f");
    stream::MemoryReader reader(buffer);
    const auto sws = Read(reader, &value);
    EXPECT_TRUE(sws.ok());
    EXPECT_EQ(sws.size(), 5u);
    EXPECT_EQ(value, std::numeric_limits<int32_t>::max());
  }

  {
    const auto buffer = MakeBuffer("\xff\xff\xff\xff\xff\xff\xff\xff\xff\x01");
    stream::MemoryReader reader(buffer);
    const auto sws = Read(reader, &value);
    EXPECT_TRUE(sws.ok());
    EXPECT_EQ(sws.size(), 10u);
    EXPECT_EQ(value, std::numeric_limits<int64_t>::min());
  }

  {
    const auto buffer = MakeBuffer("\xfe\xff\xff\xff\xff\xff\xff\xff\xff\x01");
    stream::MemoryReader reader(buffer);
    const auto sws = Read(reader, &value);
    EXPECT_TRUE(sws.ok());
    EXPECT_EQ(sws.size(), 10u);
    EXPECT_EQ(value, std::numeric_limits<int64_t>::max());
  }
}

TEST(VarintRead, Unsigned64_SingleByte) {
  uint64_t value = 1234;

  {
    const auto buffer = MakeBuffer("\x00");
    stream::MemoryReader reader(buffer);
    const auto sws = Read(reader, &value);
    EXPECT_TRUE(sws.ok());
    EXPECT_EQ(sws.size(), 1u);
    EXPECT_EQ(value, 0u);
  }

  {
    const auto buffer = MakeBuffer("\x04");
    stream::MemoryReader reader(buffer);
    const auto sws = Read(reader, &value);
    EXPECT_TRUE(sws.ok());
    EXPECT_EQ(sws.size(), 1u);
    EXPECT_EQ(value, 4u);
  }

  {
    const auto buffer = MakeBuffer("\x41");
    stream::MemoryReader reader(buffer);
    const auto sws = Read(reader, &value);
    EXPECT_TRUE(sws.ok());
    EXPECT_EQ(sws.size(), 1u);
    EXPECT_EQ(value, 65u);
  }
}

TEST(VarintRead, Unsigned64_MultiByte) {
  uint64_t value = -1234;

  {
    const auto buffer = MakeBuffer("\x80\x01");
    stream::MemoryReader reader(buffer);
    const auto sws = Read(reader, &value);
    EXPECT_TRUE(sws.ok());
    EXPECT_EQ(sws.size(), 2u);
    EXPECT_EQ(value, 128u);
  }

  {
    const auto buffer = MakeBuffer("\x81\x01");
    stream::MemoryReader reader(buffer);
    const auto sws = Read(reader, &value);
    EXPECT_TRUE(sws.ok());
    EXPECT_EQ(sws.size(), 2u);
    EXPECT_EQ(value, 129u);
  }

  {
    const auto buffer = MakeBuffer("\xfe\xff\xff\xff\x0f");
    stream::MemoryReader reader(buffer);
    const auto sws = Read(reader, &value);
    EXPECT_TRUE(sws.ok());
    EXPECT_EQ(sws.size(), 5u);
    EXPECT_EQ(value, std::numeric_limits<uint32_t>::max() - 1);
  }

  {
    const auto buffer = MakeBuffer("\xff\xff\xff\xff\x0f");
    stream::MemoryReader reader(buffer);
    const auto sws = Read(reader, &value);
    EXPECT_TRUE(sws.ok());
    EXPECT_EQ(sws.size(), 5u);
    EXPECT_EQ(value, std::numeric_limits<uint32_t>::max());
  }

  {
    const auto buffer = MakeBuffer("\xfe\xff\xff\xff\xff\xff\xff\xff\xff\x01");
    stream::MemoryReader reader(buffer);
    const auto sws = Read(reader, &value);
    EXPECT_TRUE(sws.ok());
    EXPECT_EQ(sws.size(), 10u);
    EXPECT_EQ(value, std::numeric_limits<uint64_t>::max() - 1);
  }

  {
    const auto buffer = MakeBuffer("\xff\xff\xff\xff\xff\xff\xff\xff\xff\x01");
    stream::MemoryReader reader(buffer);
    const auto sws = Read(reader, &value);
    EXPECT_TRUE(sws.ok());
    EXPECT_EQ(sws.size(), 10u);
    EXPECT_EQ(value, std::numeric_limits<uint64_t>::max());
  }
}

TEST(VarintRead, Errors) {
  uint64_t value = -1234;

  {
    std::array<std::byte, 0> buffer{};
    stream::MemoryReader reader(buffer);
    const auto sws = Read(reader, &value);
    EXPECT_FALSE(sws.ok());
    EXPECT_EQ(sws.status(), Status::OutOfRange());
  }

  {
    const auto buffer = MakeBuffer("\xff\xff");
    stream::MemoryReader reader(buffer);
    const auto sws = Read(reader, &value);
    EXPECT_FALSE(sws.ok());
    EXPECT_EQ(sws.status(), Status::DataLoss());
  }

  {
    std::array<std::byte, varint::kMaxVarint64SizeBytes + 1> buffer{};
    for (auto& b : buffer) {
      b = std::byte{0xff};
    }

    stream::MemoryReader reader(buffer);
    const auto sws = Read(reader, &value);
    EXPECT_FALSE(sws.ok());
    EXPECT_EQ(sws.status(), Status::DataLoss());
  }
}

TEST(VarintRead, SizeLimit) {
  uint64_t value = -1234;

  {
    // buffer contains a valid varint, but we limit the read length to ensure
    // that the final byte is not read, turning it into an error.
    const auto buffer = MakeBuffer("\xff\xff\xff\xff\x0f");
    stream::MemoryReader reader(buffer);
    const auto sws = Read(reader, &value, 4);
    EXPECT_FALSE(sws.ok());
    EXPECT_EQ(sws.status(), Status::DataLoss());
    EXPECT_EQ(reader.Tell(), 4u);
  }

  {
    // If we tell varint::Read() to read zero bytes, it should always return
    // OutOfRange() without moving the reader.
    const auto buffer = MakeBuffer("\xff\xff\xff\xff\x0f");
    stream::MemoryReader reader(buffer);
    const auto sws = Read(reader, &value, 0);
    EXPECT_FALSE(sws.ok());
    EXPECT_EQ(sws.status(), Status::OutOfRange());
    EXPECT_EQ(reader.Tell(), 0u);
  }
}

}  // namespace pw::varint
