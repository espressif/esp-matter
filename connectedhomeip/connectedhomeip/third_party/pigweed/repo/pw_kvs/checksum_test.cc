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

#include "pw_kvs/checksum.h"

#include "gtest/gtest.h"
#include "pw_kvs/crc16_checksum.h"

namespace pw::kvs {
namespace {

using std::byte;

constexpr std::string_view kString =
    "In the beginning the Universe was created. This has made a lot of "
    "people very angry and been widely regarded as a bad move.";
constexpr uint16_t kStringCrc = 0xC184;

TEST(Checksum, UpdateAndVerify) {
  ChecksumCrc16 crc16_algo;
  ChecksumAlgorithm& algo = crc16_algo;

  algo.Update(kString.data(), kString.size());
  EXPECT_EQ(OkStatus(), algo.Verify(as_bytes(span(&kStringCrc, 1))));
}

TEST(Checksum, Verify_Failure) {
  ChecksumCrc16 algo;
  EXPECT_EQ(Status::DataLoss(), algo.Verify(as_bytes(span(kString.data(), 2))));
}

TEST(Checksum, Verify_InvalidSize) {
  ChecksumCrc16 algo;
  EXPECT_EQ(Status::InvalidArgument(), algo.Verify({}));
  EXPECT_EQ(Status::InvalidArgument(),
            algo.Verify(as_bytes(span(kString.substr(0, 1)))));
}

TEST(Checksum, Verify_LargerState_ComparesToTruncatedData) {
  byte crc[3] = {byte{0x84}, byte{0xC1}, byte{0x33}};
  ChecksumCrc16 algo;
  ASSERT_GT(sizeof(crc), algo.size_bytes());

  algo.Update(as_bytes(span(kString)));

  EXPECT_EQ(OkStatus(), algo.Verify(crc));
}

TEST(Checksum, Reset) {
  ChecksumCrc16 crc_algo;
  crc_algo.Update(as_bytes(span(kString)));
  crc_algo.Reset();

  span state = crc_algo.Finish();
  EXPECT_EQ(state[0], byte{0xFF});
  EXPECT_EQ(state[1], byte{0xFF});
}

TEST(IgnoreChecksum, NeverUpdate_VerifyWithoutData) {
  IgnoreChecksum checksum;

  EXPECT_EQ(OkStatus(), checksum.Verify({}));
}

TEST(IgnoreChecksum, NeverUpdate_VerifyWithData) {
  IgnoreChecksum checksum;

  EXPECT_EQ(OkStatus(), checksum.Verify(as_bytes(span(kString))));
}

TEST(IgnoreChecksum, AfterUpdate_Verify) {
  IgnoreChecksum checksum;

  checksum.Update(as_bytes(span(kString)));
  EXPECT_EQ(OkStatus(), checksum.Verify({}));
}

constexpr size_t kAlignment = 10;

constexpr std::string_view kData =
    "123456789_123456789_123456789_123456789_123456789_"   //  50
    "123456789_123456789_123456789_123456789_123456789_";  // 100
const span<const byte> kBytes = as_bytes(span(kData));

class PickyChecksum final : public AlignedChecksum<kAlignment, 32> {
 public:
  PickyChecksum() : AlignedChecksum(data_), data_{}, size_(0) {}

  void Reset() override {}

  void FinalizeAligned() override { EXPECT_EQ(kData.size(), size_); }

  void UpdateAligned(span<const std::byte> data) override {
    ASSERT_EQ(data.size() % kAlignment, 0u);
    EXPECT_EQ(kData.substr(0, data.size()),
              std::string_view(reinterpret_cast<const char*>(data.data()),
                               data.size()));

    std::memcpy(&data_[size_], data.data(), data.size());
    size_ += data.size();
  }

 private:
  std::byte data_[kData.size()];
  size_t size_;
};

TEST(AlignedChecksum, MaintainsAlignment) {
  PickyChecksum checksum;

  // Write values smaller than the alignment.
  checksum.Update(kBytes.subspan(0, 1));
  checksum.Update(kBytes.subspan(1, 9));

  // Write values larger than the alignment but smaller than the buffer.
  checksum.Update(kBytes.subspan(10, 11));

  // Exactly fill the remainder of the buffer.
  checksum.Update(kBytes.subspan(21, 11));

  // Fill the buffer more than once.
  checksum.Update(kBytes.subspan(32, 66));

  // Write nothing.
  checksum.Update(kBytes.subspan(98, 0));

  // Write the remaining data.
  checksum.Update(kBytes.subspan(98, 2));

  auto state = checksum.Finish();
  EXPECT_EQ(std::string_view(reinterpret_cast<const char*>(state.data()),
                             state.size()),
            kData);
  EXPECT_EQ(OkStatus(), checksum.Verify(kBytes));
}

}  // namespace
}  // namespace pw::kvs
