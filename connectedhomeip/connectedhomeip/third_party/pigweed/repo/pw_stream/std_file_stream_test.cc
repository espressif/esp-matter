// Copyright 2022 The Pigweed Authors
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

#include "pw_stream/std_file_stream.h"

#include <algorithm>
#include <array>
#include <cinttypes>
#include <cstdio>
#include <filesystem>
#include <random>
#include <string>
#include <string_view>

#include "gtest/gtest.h"
#include "pw_assert/assert.h"
#include "pw_bytes/span.h"
#include "pw_containers/algorithm.h"
#include "pw_random/xor_shift.h"
#include "pw_span/span.h"
#include "pw_status/status.h"
#include "pw_status/status_with_size.h"
#include "pw_string/string_builder.h"

namespace pw::stream {
namespace {

constexpr std::string_view kSmallTestData(
    "This is a test string used to verify correctness!");

// Creates a directory with a specified prefix followed by a random 32-bit hex
// number. Random temporary file handle names can then be requested. When the
// TempDir is destroyed, the entire directory is deleted.
//
// Example created temporary files:
//     /tmp/StdFileStreamTest32B37409/997BDDA2
//     /tmp/StdFileStreamTest32B37409/C181909B
//
// WARNING: This class should ONLY be used for these tests!
//
// These tests need to open and close files by file name, which is incompatible
// with std::tmpfile() (which deletes files on close). Even though std::tmpnam()
// looks like the right tool to use, it's not thread safe and doesn't provide
// any guarantees that the provided file name is not in use. std::tmpnam() is
// also marked with a deprecation warning on some systems, warning against using
// it at all.
//
// While on some systems this approach may provide significantly better
// uniqueness since std::random_device may be backed with thread-safe random
// sources, the STL does not explicitly require std::random_device to produce
// non-deterministic random data (instead only recommending it). If
// std::random_device is pseudo-random, this temporary directory will always
// end up with the same naming pattern.
//
// If the STL required std::random_device to be thread-safe and
// cryptographically-secure, this class could be made reasonably production
// ready by increasing use of entropy and making temporary file name selection
// thread-safe (in case a TempDir is static and shared across multiple threads).
//
// Today, this class does not provide much better safety guarantees than
// std::tmpnam(), but thanks to the required directory prefix and typical
// implementations of std::random_device, should see less risk of collisions in
// practice.
class TempDir {
 public:
  TempDir(std::string_view prefix) : rng_(GetSeed()) {
    temp_dir_ = std::filesystem::temp_directory_path();
    temp_dir_ /= std::string(prefix) + GetRandomSuffix();
    PW_ASSERT(std::filesystem::create_directory(temp_dir_));
  }

  ~TempDir() { PW_ASSERT(std::filesystem::remove_all(temp_dir_)); }

  std::filesystem::path GetTempFileName() {
    return temp_dir_ / GetRandomSuffix();
  }

 private:
  std::string GetRandomSuffix() {
    pw::StringBuffer<9> random_suffix_str;
    uint32_t random_suffix_int = 0;
    rng_.GetInt(random_suffix_int);
    PW_ASSERT(random_suffix_str.Format("%08" PRIx32, random_suffix_int).ok());
    return std::string(random_suffix_str.view());
  }

  // Generate a 64-bit random from system entropy pool. This is used to seed a
  // pseudo-random number generator for individual file names.
  static uint64_t GetSeed() {
    std::random_device sys_rand;
    uint64_t seed = 0;
    for (size_t seed_bytes = 0; seed_bytes < sizeof(seed);
         seed_bytes += sizeof(std::random_device::result_type)) {
      std::random_device::result_type val = sys_rand();
      seed = seed << 8 * sizeof(std::random_device::result_type);
      seed |= val;
    }
    return seed;
  }

  random::XorShiftStarRng64 rng_;
  std::filesystem::path temp_dir_;
};

class StdFileStreamTest : public ::testing::Test {
 protected:
  StdFileStreamTest() = default;

  void SetUp() override {
    temp_file_path_ = temp_dir_.GetTempFileName().generic_string();
  }
  void TearDown() override {
    PW_ASSERT(std::filesystem::remove(TempFilename()));
  }

  const char* TempFilename() { return temp_file_path_.c_str(); }

 private:
  // Only construct one temporary directory to reduce waste of system entropy.
  static TempDir temp_dir_;

  std::string temp_file_path_;
};

TempDir StdFileStreamTest::temp_dir_{"StdFileStreamTest"};

TEST_F(StdFileStreamTest, SeekAtEnd) {
  // Write some data to the temporary file.
  const std::string_view kTestData = kSmallTestData;
  StdFileWriter writer(TempFilename());
  ASSERT_EQ(writer.Write(as_bytes(span(kTestData))), OkStatus());
  writer.Close();

  StdFileReader reader(TempFilename());
  ASSERT_EQ(reader.ConservativeReadLimit(), kTestData.size());

  std::array<char, 3> read_buffer;
  size_t read_offset = 0;
  while (read_offset < kTestData.size()) {
    Result<ConstByteSpan> result =
        reader.Read(as_writable_bytes(span(read_buffer)));
    ASSERT_EQ(result.status(), OkStatus());
    ASSERT_GT(result.value().size(), 0u);
    ASSERT_LE(result.value().size(), read_buffer.size());
    ASSERT_LE(result.value().size(), kTestData.size() - read_offset);
    ConstByteSpan expect_window =
        as_bytes(span(kTestData)).subspan(read_offset, result.value().size());
    EXPECT_TRUE(pw::containers::Equal(result.value(), expect_window));
    read_offset += result.value().size();
    ASSERT_EQ(reader.ConservativeReadLimit(), kTestData.size() - read_offset);
  }
  // After data has been read, do a final read to trigger EOF.
  Result<ConstByteSpan> result =
      reader.Read(as_writable_bytes(span(read_buffer)));
  EXPECT_EQ(result.status(), Status::OutOfRange());
  ASSERT_EQ(reader.ConservativeReadLimit(), 0u);

  EXPECT_EQ(read_offset, kTestData.size());

  // Seek backwards and read again to ensure seek at EOF works.
  ASSERT_EQ(reader.Seek(-1 * read_buffer.size(), Stream::Whence::kEnd),
            OkStatus());
  ASSERT_EQ(reader.ConservativeReadLimit(), read_buffer.size());
  result = reader.Read(as_writable_bytes(span(read_buffer)));
  EXPECT_EQ(result.status(), OkStatus());

  reader.Close();
}

}  // namespace
}  // namespace pw::stream
