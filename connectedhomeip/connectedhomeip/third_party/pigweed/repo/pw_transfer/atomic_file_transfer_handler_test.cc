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
#include "pw_transfer/atomic_file_transfer_handler.h"

#include <cinttypes>
#include <filesystem>
#include <fstream>
#include <random>
#include <string>
#include <string_view>

#include "gtest/gtest.h"
#include "pw_random/xor_shift.h"
#include "pw_result/result.h"
#include "pw_status/status.h"
#include "pw_string/string_builder.h"
#include "pw_transfer/transfer.h"
#include "pw_transfer_private/filename_generator.h"

namespace pw::transfer {

namespace {

// Copied from go/pw-src/+/main:pw_stream/std_file_stream_test.cc;l=75
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
    StringBuffer<9> random_suffix_str;
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

class AtomicFileTransferHandlerTest : public ::testing::Test {
 public:
  TempDir temp_dir_{"atomic_file_transfer_handler_test"};
  std::string test_data_location_pass_ = temp_dir_.GetTempFileName();
  std::string transfer_temp_file_ = GetTempFilePath(test_data_location_pass_);

 protected:
  static constexpr auto test_data_location_fail = "not/a/directory/no_data.txt";
  static constexpr auto temp_file_content = "Temp File Success.";
  static constexpr auto test_data_content = "Test File Success.";

  bool WriteContentFile(std::string_view path, std::string_view value) {
    std::ofstream file(path);
    if (!file.is_open()) {
      return false;
    }
    file << value;
    return true;
  }

  Result<std::string> ReadFile(std::string_view path) {
    std::ifstream file(path);
    if (!file.is_open()) {
      return Status::NotFound();
    }
    std::string return_value;
    std::getline(file, return_value);
    return return_value;
  }

  void ClearContent(std::string_view path) {
    std::ofstream ofs(path, std::ofstream::out | std::ofstream::trunc);
  }

  void check_finalize(Status status) {
    EXPECT_EQ(status, OkStatus());
    // Temp file does not exist after finalize.
    EXPECT_TRUE(!std::filesystem::exists(transfer_temp_file_));
    // Test path does exist, file has been created.
    EXPECT_TRUE(std::filesystem::exists(test_data_location_pass_));
    // File content is the same as expected.
    const auto file_content = ReadFile(test_data_location_pass_);
    ASSERT_TRUE(file_content.ok());

    EXPECT_EQ(file_content.value(), temp_file_content);
  }

  void SetUp() override {
    // Write content file and check correct.
    ASSERT_TRUE(WriteContentFile(test_data_location_pass_, test_data_content));
    const auto file_content_data = ReadFile(test_data_location_pass_);
    ASSERT_TRUE(file_content_data.ok());
    ASSERT_EQ(file_content_data.value(), test_data_content);

    // Write temp file and check content is correct
    ASSERT_TRUE(WriteContentFile(transfer_temp_file_, temp_file_content));
    const auto file_content_tmp = ReadFile(transfer_temp_file_);
    ASSERT_TRUE(file_content_tmp.ok());
    ASSERT_EQ(file_content_tmp.value(), temp_file_content);
  }

  void TearDown() override {
    // Ensure temp file is deleted.
    ASSERT_TRUE(!std::filesystem::exists(transfer_temp_file_) ||
                std::filesystem::remove(transfer_temp_file_));
    // Ensure test file is deleted.
    ASSERT_TRUE(!std::filesystem::exists(test_data_location_pass_) ||
                std::filesystem::remove(test_data_location_pass_));
  }
};

TEST_F(AtomicFileTransferHandlerTest, PrepareReadPass) {
  AtomicFileTransferHandler test_handler{/*resource_id = */ 0,
                                         test_data_location_pass_};
  EXPECT_EQ(test_handler.PrepareRead(), OkStatus());
}

TEST_F(AtomicFileTransferHandlerTest, PrepareReadFail) {
  AtomicFileTransferHandler test_handler{/*resource_id = */ 0,
                                         test_data_location_fail};
  EXPECT_EQ(test_handler.PrepareRead(), Status::NotFound());
}

TEST_F(AtomicFileTransferHandlerTest, PrepareWritePass) {
  AtomicFileTransferHandler test_handler{/*resource_id = */ 0,
                                         test_data_location_pass_};
  // Open a file for write returns OkStatus.
  EXPECT_EQ(test_handler.PrepareWrite(), OkStatus());
}

TEST_F(AtomicFileTransferHandlerTest, PrepareWriteFail) {
  AtomicFileTransferHandler test_handler{/*resource_id = */ 0,
                                         test_data_location_fail};
  // Open a file with non existing path pass.
  // No access to underlying stream
  // so rely on the write during transfer to catch the error.
  EXPECT_EQ(test_handler.PrepareWrite(), OkStatus());
}

TEST_F(AtomicFileTransferHandlerTest, FinalizeWriteRenameExisting) {
  ASSERT_TRUE(std::filesystem::exists(transfer_temp_file_));
  ASSERT_TRUE(std::filesystem::exists(test_data_location_pass_));
  AtomicFileTransferHandler test_handler{/*resource_id = */
                                         0,
                                         test_data_location_pass_};
  // Prepare Write to open the stream. should be closed during Finalize.
  ASSERT_EQ(test_handler.PrepareWrite(), OkStatus());
  WriteContentFile(transfer_temp_file_, temp_file_content);
  auto status = test_handler.FinalizeWrite(OkStatus());
  check_finalize(status);
}

TEST_F(AtomicFileTransferHandlerTest, FinalizeWriteNoExistingFile) {
  AtomicFileTransferHandler test_handler{/*resource_id = */
                                         0,
                                         test_data_location_pass_};
  // Remove file test file and test creation.
  ASSERT_TRUE(std::filesystem::remove(test_data_location_pass_));
  ASSERT_EQ(test_handler.PrepareWrite(), OkStatus());
  WriteContentFile(transfer_temp_file_, temp_file_content);
  auto status = test_handler.FinalizeWrite(OkStatus());
  check_finalize(status);
}

TEST_F(AtomicFileTransferHandlerTest, FinalizeWriteExpectErr) {
  AtomicFileTransferHandler test_handler{/*resource_id = */
                                         0,
                                         test_data_location_pass_};
  ASSERT_EQ(test_handler.PrepareWrite(), OkStatus());
  // Simulate write fails, file is empty, No write here.
  ClearContent(transfer_temp_file_);
  ASSERT_TRUE(std::filesystem::is_empty(transfer_temp_file_));
  ASSERT_TRUE(std::filesystem::exists(test_data_location_pass_));
  EXPECT_EQ(test_handler.FinalizeWrite(Status::DataLoss()), Status::DataLoss());
}

}  // namespace

}  // namespace pw::transfer
