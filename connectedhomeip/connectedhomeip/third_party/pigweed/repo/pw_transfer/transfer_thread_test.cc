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

#include "pw_transfer/transfer_thread.h"

#include "gtest/gtest.h"
#include "pw_assert/check.h"
#include "pw_bytes/array.h"
#include "pw_rpc/raw/client_testing.h"
#include "pw_rpc/raw/test_method_context.h"
#include "pw_rpc/test_helpers.h"
#include "pw_thread/thread.h"
#include "pw_thread_stl/options.h"
#include "pw_transfer/handler.h"
#include "pw_transfer/transfer.h"
#include "pw_transfer/transfer.raw_rpc.pb.h"
#include "pw_transfer_private/chunk_testing.h"

namespace pw::transfer::test {
namespace {

using internal::Chunk;

// TODO(frolv): Have a generic way to obtain a thread for testing on any system.
thread::Options& TransferThreadOptions() {
  static thread::stl::Options options;
  return options;
}

class TransferThreadTest : public ::testing::Test {
 public:
  TransferThreadTest()
      : ctx_(transfer_thread_, 512),
        max_parameters_(chunk_buffer_.size(),
                        chunk_buffer_.size(),
                        cfg::kDefaultExtendWindowDivisor),
        transfer_thread_(chunk_buffer_, encode_buffer_),
        system_thread_(TransferThreadOptions(), transfer_thread_) {}

  ~TransferThreadTest() override {
    transfer_thread_.Terminate();
    system_thread_.join();
  }

 protected:
  PW_RAW_TEST_METHOD_CONTEXT(TransferService, Read) ctx_;

  std::array<std::byte, 64> chunk_buffer_;
  std::array<std::byte, 64> encode_buffer_;

  rpc::RawClientTestContext<> rpc_client_context_;
  internal::TransferParameters max_parameters_;

  transfer::Thread<1, 1> transfer_thread_;

  thread::Thread system_thread_;
};

class SimpleReadTransfer final : public ReadOnlyHandler {
 public:
  SimpleReadTransfer(uint32_t session_id, ConstByteSpan data)
      : ReadOnlyHandler(session_id),
        prepare_read_called(false),
        finalize_read_called(false),
        finalize_read_status(Status::Unknown()),
        reader_(data) {}

  Status PrepareRead() final {
    PW_CHECK_OK(reader_.Seek(0));
    set_reader(reader_);
    prepare_read_called = true;
    return OkStatus();
  }

  void FinalizeRead(Status status) final {
    finalize_read_called = true;
    finalize_read_status = status;
  }

  bool prepare_read_called;
  bool finalize_read_called;
  Status finalize_read_status;

 private:
  stream::MemoryReader reader_;
};

constexpr auto kData = bytes::Initialized<32>([](size_t i) { return i; });

TEST_F(TransferThreadTest, AddTransferHandler) {
  auto reader_writer = ctx_.reader_writer();
  transfer_thread_.SetServerReadStream(reader_writer);

  SimpleReadTransfer handler(3, kData);
  transfer_thread_.AddTransferHandler(handler);

  transfer_thread_.StartServerTransfer(internal::TransferType::kTransmit,
                                       ProtocolVersion::kLegacy,
                                       3,
                                       3,
                                       {},
                                       max_parameters_,
                                       std::chrono::seconds(2),
                                       3,
                                       10);

  transfer_thread_.WaitUntilEventIsProcessed();

  EXPECT_TRUE(handler.prepare_read_called);

  transfer_thread_.RemoveTransferHandler(handler);
}

TEST_F(TransferThreadTest, RemoveTransferHandler) {
  auto reader_writer = ctx_.reader_writer();
  transfer_thread_.SetServerReadStream(reader_writer);

  SimpleReadTransfer handler(3, kData);
  transfer_thread_.AddTransferHandler(handler);
  transfer_thread_.RemoveTransferHandler(handler);

  transfer_thread_.StartServerTransfer(internal::TransferType::kTransmit,
                                       ProtocolVersion::kLegacy,
                                       3,
                                       3,
                                       {},
                                       max_parameters_,
                                       std::chrono::seconds(2),
                                       3,
                                       10);

  transfer_thread_.WaitUntilEventIsProcessed();

  EXPECT_FALSE(handler.prepare_read_called);

  ASSERT_EQ(ctx_.total_responses(), 1u);
  auto chunk = DecodeChunk(ctx_.response());
  EXPECT_EQ(chunk.session_id(), 3u);
  ASSERT_TRUE(chunk.status().has_value());
  EXPECT_EQ(chunk.status().value(), Status::NotFound());

  transfer_thread_.RemoveTransferHandler(handler);
}

TEST_F(TransferThreadTest, ProcessChunk_SendsWindow) {
  auto reader_writer = ctx_.reader_writer();
  transfer_thread_.SetServerReadStream(reader_writer);

  SimpleReadTransfer handler(3, kData);
  transfer_thread_.AddTransferHandler(handler);

  rpc::test::WaitForPackets(ctx_.output(), 2, [this] {
    transfer_thread_.StartServerTransfer(
        internal::TransferType::kTransmit,
        ProtocolVersion::kLegacy,
        3,
        3,
        EncodeChunk(
            Chunk(ProtocolVersion::kLegacy, Chunk::Type::kParametersRetransmit)
                .set_session_id(3)
                .set_window_end_offset(16)
                .set_max_chunk_size_bytes(8)
                .set_offset(0)),
        max_parameters_,
        std::chrono::seconds(2),
        3,
        10);
  });

  ASSERT_EQ(ctx_.total_responses(), 2u);
  auto chunk = DecodeChunk(ctx_.responses()[0]);
  EXPECT_EQ(chunk.session_id(), 3u);
  EXPECT_EQ(chunk.offset(), 0u);
  EXPECT_EQ(chunk.payload().size(), 8u);
  EXPECT_EQ(
      std::memcmp(chunk.payload().data(), kData.data(), chunk.payload().size()),
      0);

  chunk = DecodeChunk(ctx_.responses()[1]);
  EXPECT_EQ(chunk.session_id(), 3u);
  EXPECT_EQ(chunk.offset(), 8u);
  EXPECT_EQ(chunk.payload().size(), 8u);
  EXPECT_EQ(
      std::memcmp(
          chunk.payload().data(), kData.data() + 8, chunk.payload().size()),
      0);

  transfer_thread_.RemoveTransferHandler(handler);
}

TEST_F(TransferThreadTest, StartTransferExhausted_Server) {
  auto reader_writer = ctx_.reader_writer();
  transfer_thread_.SetServerReadStream(reader_writer);

  SimpleReadTransfer handler3(3, kData);
  SimpleReadTransfer handler4(4, kData);
  transfer_thread_.AddTransferHandler(handler3);
  transfer_thread_.AddTransferHandler(handler4);

  transfer_thread_.StartServerTransfer(
      internal::TransferType::kTransmit,
      ProtocolVersion::kLegacy,
      3,
      3,
      EncodeChunk(
          Chunk(ProtocolVersion::kLegacy, Chunk::Type::kParametersRetransmit)
              .set_session_id(3)
              .set_window_end_offset(16)
              .set_max_chunk_size_bytes(8)
              .set_offset(0)),
      max_parameters_,
      std::chrono::seconds(2),
      3,
      10);
  transfer_thread_.WaitUntilEventIsProcessed();
  transfer_thread_.WaitUntilEventIsProcessed();

  // First transfer starts correctly.
  EXPECT_TRUE(handler3.prepare_read_called);
  EXPECT_FALSE(handler4.prepare_read_called);
  ASSERT_EQ(ctx_.total_responses(), 1u);

  // Try to start a simultaneous transfer to resource 4, for which the thread
  // does not have an available context.
  transfer_thread_.StartServerTransfer(
      internal::TransferType::kTransmit,
      ProtocolVersion::kLegacy,
      4,
      4,
      EncodeChunk(
          Chunk(ProtocolVersion::kLegacy, Chunk::Type::kParametersRetransmit)
              .set_session_id(4)
              .set_window_end_offset(16)
              .set_max_chunk_size_bytes(8)
              .set_offset(0)),
      max_parameters_,
      std::chrono::seconds(2),
      3,
      10);
  transfer_thread_.WaitUntilEventIsProcessed();

  EXPECT_FALSE(handler4.prepare_read_called);

  ASSERT_EQ(ctx_.total_responses(), 2u);
  auto chunk = DecodeChunk(ctx_.response());
  EXPECT_EQ(chunk.session_id(), 4u);
  ASSERT_TRUE(chunk.status().has_value());
  EXPECT_EQ(chunk.status().value(), Status::ResourceExhausted());

  transfer_thread_.RemoveTransferHandler(handler3);
  transfer_thread_.RemoveTransferHandler(handler4);
}

TEST_F(TransferThreadTest, StartTransferExhausted_Client) {
  rpc::RawClientReaderWriter read_stream = pw_rpc::raw::Transfer::Read(
      rpc_client_context_.client(), rpc_client_context_.channel().id());
  transfer_thread_.SetClientReadStream(read_stream);

  Status status3 = Status::Unknown();
  Status status4 = Status::Unknown();

  stream::MemoryWriterBuffer<16> buffer3;
  stream::MemoryWriterBuffer<16> buffer4;

  transfer_thread_.StartClientTransfer(
      internal::TransferType::kReceive,
      ProtocolVersion::kLegacy,
      3,
      &buffer3,
      max_parameters_,
      [&status3](Status status) { status3 = status; },
      std::chrono::seconds(2),
      3,
      10);
  transfer_thread_.WaitUntilEventIsProcessed();

  EXPECT_EQ(status3, Status::Unknown());
  EXPECT_EQ(status4, Status::Unknown());

  // Try to start a simultaneous transfer to resource 4, for which the thread
  // does not have an available context.
  transfer_thread_.StartClientTransfer(
      internal::TransferType::kReceive,
      ProtocolVersion::kLegacy,
      4,
      &buffer4,
      max_parameters_,
      [&status4](Status status) { status4 = status; },
      std::chrono::seconds(2),
      3,
      10);
  transfer_thread_.WaitUntilEventIsProcessed();

  EXPECT_EQ(status3, Status::Unknown());
  EXPECT_EQ(status4, Status::ResourceExhausted());

  transfer_thread_.EndClientTransfer(3, Status::Cancelled());
  transfer_thread_.EndClientTransfer(4, Status::Cancelled());
}

TEST_F(TransferThreadTest, VersionTwo_NoHandler) {
  auto reader_writer = ctx_.reader_writer();
  transfer_thread_.SetServerReadStream(reader_writer);

  SimpleReadTransfer handler(3, kData);
  transfer_thread_.AddTransferHandler(handler);
  transfer_thread_.RemoveTransferHandler(handler);

  transfer_thread_.StartServerTransfer(internal::TransferType::kTransmit,
                                       ProtocolVersion::kVersionTwo,
                                       /*session_id=*/421,
                                       /*resource_id=*/7,
                                       {},
                                       max_parameters_,
                                       std::chrono::seconds(2),
                                       3,
                                       10);

  transfer_thread_.WaitUntilEventIsProcessed();

  EXPECT_FALSE(handler.prepare_read_called);

  ASSERT_EQ(ctx_.total_responses(), 1u);
  Result<Chunk::Identifier> id = Chunk::ExtractIdentifier(ctx_.response());
  ASSERT_TRUE(id.ok());
  EXPECT_EQ(id->value(), 7u);
  auto chunk = DecodeChunk(ctx_.response());
  EXPECT_EQ(chunk.session_id(), 7u);
  EXPECT_EQ(chunk.resource_id(), 7u);
  ASSERT_TRUE(chunk.status().has_value());
  EXPECT_EQ(chunk.status().value(), Status::NotFound());

  transfer_thread_.RemoveTransferHandler(handler);
}

}  // namespace
}  // namespace pw::transfer::test
