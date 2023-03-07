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

#include "pw_transfer/client.h"

#include <cstring>

#include "gtest/gtest.h"
#include "pw_assert/check.h"
#include "pw_bytes/array.h"
#include "pw_rpc/raw/client_testing.h"
#include "pw_rpc/test_helpers.h"
#include "pw_thread/sleep.h"
#include "pw_thread/thread.h"
#include "pw_thread_stl/options.h"
#include "pw_transfer_private/chunk_testing.h"

namespace pw::transfer::test {
namespace {

using internal::Chunk;
using pw_rpc::raw::Transfer;

using namespace std::chrono_literals;

thread::Options& TransferThreadOptions() {
  static thread::stl::Options options;
  return options;
}

class ReadTransfer : public ::testing::Test {
 protected:
  ReadTransfer(size_t max_bytes_to_receive = 0)
      : transfer_thread_(chunk_buffer_, encode_buffer_),
        client_(context_.client(),
                context_.channel().id(),
                transfer_thread_,
                max_bytes_to_receive),
        system_thread_(TransferThreadOptions(), transfer_thread_) {}

  ~ReadTransfer() override {
    transfer_thread_.Terminate();
    system_thread_.join();
  }

  rpc::RawClientTestContext<> context_;

  Thread<1, 1> transfer_thread_;
  Client client_;

  std::array<std::byte, 64> chunk_buffer_;
  std::array<std::byte, 64> encode_buffer_;

  thread::Thread system_thread_;
};

constexpr auto kData32 = bytes::Initialized<32>([](size_t i) { return i; });
constexpr auto kData64 = bytes::Initialized<64>([](size_t i) { return i; });

TEST_F(ReadTransfer, SingleChunk) {
  stream::MemoryWriterBuffer<64> writer;
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Read(3, writer, [&transfer_status](Status status) {
              transfer_status = status;
            }));

  transfer_thread_.WaitUntilEventIsProcessed();

  // First transfer parameters chunk is sent.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Read>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk c0 = DecodeChunk(payloads[0]);
  EXPECT_EQ(c0.session_id(), 3u);
  EXPECT_EQ(c0.offset(), 0u);
  EXPECT_EQ(c0.window_end_offset(), 64u);
  EXPECT_EQ(c0.type(), Chunk::Type::kStart);

  context_.server().SendServerStream<Transfer::Read>(
      EncodeChunk(Chunk(ProtocolVersion::kLegacy, Chunk::Type::kData)
                      .set_session_id(3)
                      .set_offset(0)
                      .set_payload(kData32)
                      .set_remaining_bytes(0)));
  transfer_thread_.WaitUntilEventIsProcessed();

  ASSERT_EQ(payloads.size(), 2u);

  Chunk c1 = DecodeChunk(payloads.back());
  EXPECT_EQ(c1.session_id(), 3u);
  ASSERT_TRUE(c1.status().has_value());
  EXPECT_EQ(c1.status().value(), OkStatus());

  EXPECT_EQ(transfer_status, OkStatus());
  EXPECT_EQ(std::memcmp(writer.data(), kData32.data(), writer.bytes_written()),
            0);
}

TEST_F(ReadTransfer, MultiChunk) {
  stream::MemoryWriterBuffer<64> writer;
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Read(4, writer, [&transfer_status](Status status) {
              transfer_status = status;
            }));

  transfer_thread_.WaitUntilEventIsProcessed();

  // First transfer parameters chunk is sent.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Read>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk c0 = DecodeChunk(payloads[0]);
  EXPECT_EQ(c0.session_id(), 4u);
  EXPECT_EQ(c0.offset(), 0u);
  EXPECT_EQ(c0.window_end_offset(), 64u);
  EXPECT_EQ(c0.type(), Chunk::Type::kStart);

  constexpr ConstByteSpan data(kData32);
  context_.server().SendServerStream<Transfer::Read>(
      EncodeChunk(Chunk(ProtocolVersion::kLegacy, Chunk::Type::kData)
                      .set_session_id(4)
                      .set_offset(0)
                      .set_payload(data.first(16))));
  transfer_thread_.WaitUntilEventIsProcessed();

  ASSERT_EQ(payloads.size(), 1u);

  context_.server().SendServerStream<Transfer::Read>(
      EncodeChunk(Chunk(ProtocolVersion::kLegacy, Chunk::Type::kData)
                      .set_session_id(4)
                      .set_offset(16)
                      .set_payload(data.subspan(16))
                      .set_remaining_bytes(0)));
  transfer_thread_.WaitUntilEventIsProcessed();

  ASSERT_EQ(payloads.size(), 2u);

  Chunk c1 = DecodeChunk(payloads[1]);
  EXPECT_EQ(c1.session_id(), 4u);
  ASSERT_TRUE(c1.status().has_value());
  EXPECT_EQ(c1.status().value(), OkStatus());

  EXPECT_EQ(transfer_status, OkStatus());
  EXPECT_EQ(std::memcmp(writer.data(), kData32.data(), writer.bytes_written()),
            0);
}

TEST_F(ReadTransfer, MultipleTransfers) {
  stream::MemoryWriterBuffer<64> writer;
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Read(3, writer, [&transfer_status](Status status) {
              transfer_status = status;
            }));
  transfer_thread_.WaitUntilEventIsProcessed();

  context_.server().SendServerStream<Transfer::Read>(
      EncodeChunk(Chunk(ProtocolVersion::kLegacy, Chunk::Type::kData)
                      .set_session_id(3)
                      .set_offset(0)
                      .set_payload(kData32)
                      .set_remaining_bytes(0)));
  transfer_thread_.WaitUntilEventIsProcessed();

  ASSERT_EQ(transfer_status, OkStatus());
  transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Read(3, writer, [&transfer_status](Status status) {
              transfer_status = status;
            }));
  transfer_thread_.WaitUntilEventIsProcessed();

  context_.server().SendServerStream<Transfer::Read>(
      EncodeChunk(Chunk(ProtocolVersion::kLegacy, Chunk::Type::kData)
                      .set_session_id(3)
                      .set_offset(0)
                      .set_payload(kData32)
                      .set_remaining_bytes(0)));
  transfer_thread_.WaitUntilEventIsProcessed();

  EXPECT_EQ(transfer_status, OkStatus());
}

class ReadTransferMaxBytes32 : public ReadTransfer {
 protected:
  ReadTransferMaxBytes32() : ReadTransfer(/*max_bytes_to_receive=*/32) {}
};

TEST_F(ReadTransferMaxBytes32, SetsPendingBytesFromConstructorArg) {
  stream::MemoryWriterBuffer<64> writer;
  EXPECT_EQ(OkStatus(), client_.Read(5, writer, [](Status) {}));
  transfer_thread_.WaitUntilEventIsProcessed();

  // First transfer parameters chunk is sent.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Read>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);

  Chunk c0 = DecodeChunk(payloads[0]);
  EXPECT_EQ(c0.session_id(), 5u);
  EXPECT_EQ(c0.offset(), 0u);
  EXPECT_EQ(c0.window_end_offset(), 32u);
  EXPECT_EQ(c0.type(), Chunk::Type::kStart);
}

TEST_F(ReadTransferMaxBytes32, SetsPendingBytesFromWriterLimit) {
  stream::MemoryWriterBuffer<16> small_writer;
  EXPECT_EQ(OkStatus(), client_.Read(5, small_writer, [](Status) {}));
  transfer_thread_.WaitUntilEventIsProcessed();

  // First transfer parameters chunk is sent.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Read>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);

  Chunk c0 = DecodeChunk(payloads[0]);
  EXPECT_EQ(c0.session_id(), 5u);
  EXPECT_EQ(c0.offset(), 0u);
  EXPECT_EQ(c0.window_end_offset(), 16u);
  EXPECT_EQ(c0.type(), Chunk::Type::kStart);
}

TEST_F(ReadTransferMaxBytes32, MultiParameters) {
  stream::MemoryWriterBuffer<64> writer;
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Read(6, writer, [&transfer_status](Status status) {
              transfer_status = status;
            }));
  transfer_thread_.WaitUntilEventIsProcessed();

  // First transfer parameters chunk is sent.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Read>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk c0 = DecodeChunk(payloads[0]);
  EXPECT_EQ(c0.session_id(), 6u);
  EXPECT_EQ(c0.offset(), 0u);
  ASSERT_EQ(c0.window_end_offset(), 32u);

  constexpr ConstByteSpan data(kData64);
  context_.server().SendServerStream<Transfer::Read>(
      EncodeChunk(Chunk(ProtocolVersion::kLegacy, Chunk::Type::kData)
                      .set_session_id(6)
                      .set_offset(0)
                      .set_payload(data.first(32))));
  transfer_thread_.WaitUntilEventIsProcessed();

  ASSERT_EQ(payloads.size(), 2u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  // Second parameters chunk.
  Chunk c1 = DecodeChunk(payloads[1]);
  EXPECT_EQ(c1.session_id(), 6u);
  EXPECT_EQ(c1.offset(), 32u);
  ASSERT_EQ(c1.window_end_offset(), 64u);

  context_.server().SendServerStream<Transfer::Read>(
      EncodeChunk(Chunk(ProtocolVersion::kLegacy, Chunk::Type::kData)
                      .set_session_id(6)
                      .set_offset(32)
                      .set_payload(data.subspan(32))
                      .set_remaining_bytes(0)));
  transfer_thread_.WaitUntilEventIsProcessed();

  ASSERT_EQ(payloads.size(), 3u);

  Chunk c2 = DecodeChunk(payloads[2]);
  EXPECT_EQ(c2.session_id(), 6u);
  ASSERT_TRUE(c2.status().has_value());
  EXPECT_EQ(c2.status().value(), OkStatus());

  EXPECT_EQ(transfer_status, OkStatus());
  EXPECT_EQ(std::memcmp(writer.data(), data.data(), writer.bytes_written()), 0);
}

TEST_F(ReadTransfer, UnexpectedOffset) {
  stream::MemoryWriterBuffer<64> writer;
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Read(7, writer, [&transfer_status](Status status) {
              transfer_status = status;
            }));
  transfer_thread_.WaitUntilEventIsProcessed();

  // First transfer parameters chunk is sent.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Read>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk c0 = DecodeChunk(payloads[0]);
  EXPECT_EQ(c0.session_id(), 7u);
  EXPECT_EQ(c0.offset(), 0u);
  EXPECT_EQ(c0.window_end_offset(), 64u);

  constexpr ConstByteSpan data(kData32);
  context_.server().SendServerStream<Transfer::Read>(
      EncodeChunk(Chunk(ProtocolVersion::kLegacy, Chunk::Type::kData)
                      .set_session_id(7)
                      .set_offset(0)
                      .set_payload(data.first(16))));
  transfer_thread_.WaitUntilEventIsProcessed();

  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  // Send a chunk with an incorrect offset. The client should resend parameters.
  context_.server().SendServerStream<Transfer::Read>(
      EncodeChunk(Chunk(ProtocolVersion::kLegacy, Chunk::Type::kData)
                      .set_session_id(7)
                      .set_offset(8)  // wrong!
                      .set_payload(data.subspan(16))
                      .set_remaining_bytes(0)));
  transfer_thread_.WaitUntilEventIsProcessed();

  ASSERT_EQ(payloads.size(), 2u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk c1 = DecodeChunk(payloads[1]);
  EXPECT_EQ(c1.session_id(), 7u);
  EXPECT_EQ(c1.offset(), 16u);
  EXPECT_EQ(c1.window_end_offset(), 64u);

  // Send the correct chunk, completing the transfer.
  context_.server().SendServerStream<Transfer::Read>(
      EncodeChunk(Chunk(ProtocolVersion::kLegacy, Chunk::Type::kData)
                      .set_session_id(7)
                      .set_offset(16)
                      .set_payload(data.subspan(16))
                      .set_remaining_bytes(0)));
  transfer_thread_.WaitUntilEventIsProcessed();

  ASSERT_EQ(payloads.size(), 3u);

  Chunk c2 = DecodeChunk(payloads[2]);
  EXPECT_EQ(c2.session_id(), 7u);
  ASSERT_TRUE(c2.status().has_value());
  EXPECT_EQ(c2.status().value(), OkStatus());

  EXPECT_EQ(transfer_status, OkStatus());
  EXPECT_EQ(std::memcmp(writer.data(), kData32.data(), writer.bytes_written()),
            0);
}

TEST_F(ReadTransferMaxBytes32, TooMuchData) {
  stream::MemoryWriterBuffer<32> writer;
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Read(8, writer, [&transfer_status](Status status) {
              transfer_status = status;
            }));
  transfer_thread_.WaitUntilEventIsProcessed();

  // First transfer parameters chunk is sent.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Read>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk c0 = DecodeChunk(payloads[0]);
  EXPECT_EQ(c0.session_id(), 8u);
  EXPECT_EQ(c0.offset(), 0u);
  ASSERT_EQ(c0.window_end_offset(), 32u);

  constexpr ConstByteSpan data(kData64);

  // pending_bytes == 32
  context_.server().SendServerStream<Transfer::Read>(
      EncodeChunk(Chunk(ProtocolVersion::kLegacy, Chunk::Type::kData)
                      .set_session_id(8)
                      .set_offset(0)
                      .set_payload(data.first(16))));

  // pending_bytes == 16
  context_.server().SendServerStream<Transfer::Read>(
      EncodeChunk(Chunk(ProtocolVersion::kLegacy, Chunk::Type::kData)
                      .set_session_id(8)
                      .set_offset(16)
                      .set_payload(data.subspan(16, 8))));

  // pending_bytes == 8, send 16 instead.
  context_.server().SendServerStream<Transfer::Read>(
      EncodeChunk(Chunk(ProtocolVersion::kLegacy, Chunk::Type::kData)
                      .set_session_id(8)
                      .set_offset(24)
                      .set_payload(data.subspan(24, 16))));
  transfer_thread_.WaitUntilEventIsProcessed();

  ASSERT_EQ(payloads.size(), 4u);

  Chunk c1 = DecodeChunk(payloads[3]);
  EXPECT_EQ(c1.session_id(), 8u);
  ASSERT_TRUE(c1.status().has_value());
  EXPECT_EQ(c1.status().value(), Status::Internal());

  EXPECT_EQ(transfer_status, Status::Internal());
}

TEST_F(ReadTransfer, ServerError) {
  stream::MemoryWriterBuffer<64> writer;
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Read(9, writer, [&transfer_status](Status status) {
              transfer_status = status;
            }));
  transfer_thread_.WaitUntilEventIsProcessed();

  // First transfer parameters chunk is sent.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Read>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk c0 = DecodeChunk(payloads[0]);
  EXPECT_EQ(c0.session_id(), 9u);
  EXPECT_EQ(c0.offset(), 0u);
  ASSERT_EQ(c0.window_end_offset(), 64u);

  // Server sends an error. Client should not respond and terminate the
  // transfer.
  context_.server().SendServerStream<Transfer::Read>(EncodeChunk(
      Chunk::Final(ProtocolVersion::kLegacy, 9, Status::NotFound())));
  transfer_thread_.WaitUntilEventIsProcessed();

  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::NotFound());
}

TEST_F(ReadTransfer, OnlySendsParametersOnceAfterDrop) {
  stream::MemoryWriterBuffer<64> writer;
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Read(10, writer, [&transfer_status](Status status) {
              transfer_status = status;
            }));
  transfer_thread_.WaitUntilEventIsProcessed();

  // First transfer parameters chunk is sent.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Read>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk c0 = DecodeChunk(payloads[0]);
  EXPECT_EQ(c0.session_id(), 10u);
  EXPECT_EQ(c0.offset(), 0u);
  ASSERT_EQ(c0.window_end_offset(), 64u);

  constexpr ConstByteSpan data(kData32);

  // Send the first 8 bytes of the transfer.
  context_.server().SendServerStream<Transfer::Read>(
      EncodeChunk(Chunk(ProtocolVersion::kLegacy, Chunk::Type::kData)
                      .set_session_id(10)
                      .set_offset(0)
                      .set_payload(data.first(8))));

  // Skip offset 8, send the rest starting from 16.
  for (uint32_t offset = 16; offset < data.size(); offset += 8) {
    context_.server().SendServerStream<Transfer::Read>(
        EncodeChunk(Chunk(ProtocolVersion::kLegacy, Chunk::Type::kData)
                        .set_session_id(10)
                        .set_offset(offset)
                        .set_payload(data.subspan(offset, 8))));
  }
  transfer_thread_.WaitUntilEventIsProcessed();

  // Only one parameters update should be sent, with the offset of the initial
  // dropped packet.
  ASSERT_EQ(payloads.size(), 2u);

  Chunk c1 = DecodeChunk(payloads[1]);
  EXPECT_EQ(c1.session_id(), 10u);
  EXPECT_EQ(c1.offset(), 8u);
  ASSERT_EQ(c1.window_end_offset(), 64u);

  // Send the remaining data to complete the transfer.
  context_.server().SendServerStream<Transfer::Read>(
      EncodeChunk(Chunk(ProtocolVersion::kLegacy, Chunk::Type::kData)
                      .set_session_id(10)
                      .set_offset(8)
                      .set_payload(data.subspan(8))
                      .set_remaining_bytes(0)));
  transfer_thread_.WaitUntilEventIsProcessed();

  ASSERT_EQ(payloads.size(), 3u);

  Chunk c2 = DecodeChunk(payloads[2]);
  EXPECT_EQ(c2.session_id(), 10u);
  ASSERT_TRUE(c2.status().has_value());
  EXPECT_EQ(c2.status().value(), OkStatus());

  EXPECT_EQ(transfer_status, OkStatus());
}

TEST_F(ReadTransfer, ResendsParametersIfSentRepeatedChunkDuringRecovery) {
  stream::MemoryWriterBuffer<64> writer;
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Read(11, writer, [&transfer_status](Status status) {
              transfer_status = status;
            }));
  transfer_thread_.WaitUntilEventIsProcessed();

  // First transfer parameters chunk is sent.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Read>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk c0 = DecodeChunk(payloads[0]);
  EXPECT_EQ(c0.session_id(), 11u);
  EXPECT_EQ(c0.offset(), 0u);
  ASSERT_EQ(c0.window_end_offset(), 64u);

  constexpr ConstByteSpan data(kData32);

  // Send the first 8 bytes of the transfer.
  context_.server().SendServerStream<Transfer::Read>(
      EncodeChunk(Chunk(ProtocolVersion::kLegacy, Chunk::Type::kData)
                      .set_session_id(11)
                      .set_offset(0)
                      .set_payload(data.first(8))));

  // Skip offset 8, send the rest starting from 16.
  for (uint32_t offset = 16; offset < data.size(); offset += 8) {
    context_.server().SendServerStream<Transfer::Read>(
        EncodeChunk(Chunk(ProtocolVersion::kLegacy, Chunk::Type::kData)
                        .set_session_id(11)
                        .set_offset(offset)
                        .set_payload(data.subspan(offset, 8))));
  }
  transfer_thread_.WaitUntilEventIsProcessed();

  // Only one parameters update should be sent, with the offset of the initial
  // dropped packet.
  ASSERT_EQ(payloads.size(), 2u);

  const Chunk last_chunk = Chunk(ProtocolVersion::kLegacy, Chunk::Type::kData)
                               .set_session_id(11)
                               .set_offset(24)
                               .set_payload(data.subspan(24));

  // Re-send the final chunk of the block.
  context_.server().SendServerStream<Transfer::Read>(EncodeChunk(last_chunk));
  transfer_thread_.WaitUntilEventIsProcessed();

  // The original drop parameters should be re-sent.
  ASSERT_EQ(payloads.size(), 3u);
  Chunk c2 = DecodeChunk(payloads[2]);
  EXPECT_EQ(c2.session_id(), 11u);
  EXPECT_EQ(c2.offset(), 8u);
  ASSERT_EQ(c2.window_end_offset(), 64u);

  // Do it again.
  context_.server().SendServerStream<Transfer::Read>(EncodeChunk(last_chunk));
  transfer_thread_.WaitUntilEventIsProcessed();

  ASSERT_EQ(payloads.size(), 4u);
  Chunk c3 = DecodeChunk(payloads[3]);
  EXPECT_EQ(c3.session_id(), 11u);
  EXPECT_EQ(c3.offset(), 8u);
  ASSERT_EQ(c3.window_end_offset(), 64u);

  // Finish the transfer normally.
  context_.server().SendServerStream<Transfer::Read>(
      EncodeChunk(Chunk(ProtocolVersion::kLegacy, Chunk::Type::kData)
                      .set_session_id(11)
                      .set_offset(8)
                      .set_payload(data.subspan(8))
                      .set_remaining_bytes(0)));
  transfer_thread_.WaitUntilEventIsProcessed();

  ASSERT_EQ(payloads.size(), 5u);

  Chunk c4 = DecodeChunk(payloads[4]);
  EXPECT_EQ(c4.session_id(), 11u);
  ASSERT_TRUE(c4.status().has_value());
  EXPECT_EQ(c4.status().value(), OkStatus());

  EXPECT_EQ(transfer_status, OkStatus());
}

constexpr chrono::SystemClock::duration kTestTimeout =
    std::chrono::milliseconds(50);
constexpr uint8_t kTestRetries = 3;

TEST_F(ReadTransfer, Timeout_ResendsCurrentParameters) {
  stream::MemoryWriterBuffer<64> writer;
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Read(
                12,
                writer,
                [&transfer_status](Status status) { transfer_status = status; },
                kTestTimeout));
  transfer_thread_.WaitUntilEventIsProcessed();

  // First transfer parameters chunk is sent.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Read>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk c0 = DecodeChunk(payloads.back());
  EXPECT_EQ(c0.session_id(), 12u);
  EXPECT_EQ(c0.offset(), 0u);
  EXPECT_EQ(c0.window_end_offset(), 64u);
  EXPECT_EQ(c0.type(), Chunk::Type::kStart);

  // Wait for the timeout to expire without doing anything. The client should
  // resend its initial parameters chunk.
  transfer_thread_.SimulateClientTimeout(12);
  ASSERT_EQ(payloads.size(), 2u);

  Chunk c = DecodeChunk(payloads.back());
  EXPECT_EQ(c.session_id(), 12u);
  EXPECT_EQ(c.offset(), 0u);
  EXPECT_EQ(c.window_end_offset(), 64u);
  EXPECT_EQ(c0.type(), Chunk::Type::kStart);

  // Transfer has not yet completed.
  EXPECT_EQ(transfer_status, Status::Unknown());

  // Finish the transfer following the timeout.
  context_.server().SendServerStream<Transfer::Read>(
      EncodeChunk(Chunk(ProtocolVersion::kLegacy, Chunk::Type::kData)
                      .set_session_id(12)
                      .set_offset(0)
                      .set_payload(kData32)
                      .set_remaining_bytes(0)));
  transfer_thread_.WaitUntilEventIsProcessed();

  ASSERT_EQ(payloads.size(), 3u);

  Chunk c4 = DecodeChunk(payloads.back());
  EXPECT_EQ(c4.session_id(), 12u);
  ASSERT_TRUE(c4.status().has_value());
  EXPECT_EQ(c4.status().value(), OkStatus());

  EXPECT_EQ(transfer_status, OkStatus());
}

TEST_F(ReadTransfer, Timeout_ResendsUpdatedParameters) {
  stream::MemoryWriterBuffer<64> writer;
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Read(
                13,
                writer,
                [&transfer_status](Status status) { transfer_status = status; },
                kTestTimeout));
  transfer_thread_.WaitUntilEventIsProcessed();

  // First transfer parameters chunk is sent.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Read>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk c0 = DecodeChunk(payloads.back());
  EXPECT_EQ(c0.session_id(), 13u);
  EXPECT_EQ(c0.offset(), 0u);
  EXPECT_EQ(c0.window_end_offset(), 64u);
  EXPECT_EQ(c0.type(), Chunk::Type::kStart);

  constexpr ConstByteSpan data(kData32);

  // Send some data, but not everything.
  context_.server().SendServerStream<Transfer::Read>(
      EncodeChunk(Chunk(ProtocolVersion::kLegacy, Chunk::Type::kData)
                      .set_session_id(13)
                      .set_offset(0)
                      .set_payload(data.first(16))));
  transfer_thread_.WaitUntilEventIsProcessed();

  ASSERT_EQ(payloads.size(), 1u);

  // Wait for the timeout to expire without sending more data. The client should
  // send an updated parameters chunk, accounting for the data already received.
  transfer_thread_.SimulateClientTimeout(13);
  ASSERT_EQ(payloads.size(), 2u);

  Chunk c = DecodeChunk(payloads.back());
  EXPECT_EQ(c.session_id(), 13u);
  EXPECT_EQ(c.offset(), 16u);
  EXPECT_EQ(c.window_end_offset(), 64u);
  EXPECT_EQ(c.type(), Chunk::Type::kParametersRetransmit);

  // Transfer has not yet completed.
  EXPECT_EQ(transfer_status, Status::Unknown());

  // Send the rest of the data, finishing the transfer.
  context_.server().SendServerStream<Transfer::Read>(
      EncodeChunk(Chunk(ProtocolVersion::kLegacy, Chunk::Type::kData)
                      .set_session_id(13)
                      .set_offset(16)
                      .set_payload(data.subspan(16))
                      .set_remaining_bytes(0)));
  transfer_thread_.WaitUntilEventIsProcessed();

  ASSERT_EQ(payloads.size(), 3u);

  Chunk c4 = DecodeChunk(payloads.back());
  EXPECT_EQ(c4.session_id(), 13u);
  ASSERT_TRUE(c4.status().has_value());
  EXPECT_EQ(c4.status().value(), OkStatus());

  EXPECT_EQ(transfer_status, OkStatus());
}

TEST_F(ReadTransfer, Timeout_EndsTransferAfterMaxRetries) {
  stream::MemoryWriterBuffer<64> writer;
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Read(
                14,
                writer,
                [&transfer_status](Status status) { transfer_status = status; },
                kTestTimeout));
  transfer_thread_.WaitUntilEventIsProcessed();

  // First transfer parameters chunk is sent.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Read>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk c0 = DecodeChunk(payloads.back());
  EXPECT_EQ(c0.session_id(), 14u);
  EXPECT_EQ(c0.offset(), 0u);
  EXPECT_EQ(c0.window_end_offset(), 64u);
  EXPECT_EQ(c0.type(), Chunk::Type::kStart);

  for (unsigned retry = 1; retry <= kTestRetries; ++retry) {
    // Wait for the timeout to expire without doing anything. The client should
    // resend its parameters chunk.
    transfer_thread_.SimulateClientTimeout(14);
    ASSERT_EQ(payloads.size(), retry + 1);

    Chunk c = DecodeChunk(payloads.back());
    EXPECT_EQ(c.session_id(), 14u);
    EXPECT_EQ(c.offset(), 0u);
    EXPECT_EQ(c.window_end_offset(), 64u);

    // Transfer has not yet completed.
    EXPECT_EQ(transfer_status, Status::Unknown());
  }

  // Sleep one more time after the final retry. The client should cancel the
  // transfer at this point. As no packets were received from the server, no
  // final status chunk should be sent.
  transfer_thread_.SimulateClientTimeout(14);
  ASSERT_EQ(payloads.size(), 4u);

  EXPECT_EQ(transfer_status, Status::DeadlineExceeded());

  // After finishing the transfer, nothing else should be sent. Verify this by
  // waiting for a bit.
  this_thread::sleep_for(kTestTimeout * 4);
  ASSERT_EQ(payloads.size(), 4u);
}

TEST_F(ReadTransfer, Timeout_ReceivingDataResetsRetryCount) {
  stream::MemoryWriterBuffer<64> writer;
  Status transfer_status = Status::Unknown();

  constexpr ConstByteSpan data(kData32);

  ASSERT_EQ(OkStatus(),
            client_.Read(
                14,
                writer,
                [&transfer_status](Status status) { transfer_status = status; },
                kTestTimeout));
  transfer_thread_.WaitUntilEventIsProcessed();

  // First transfer parameters chunk is sent.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Read>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk c0 = DecodeChunk(payloads.back());
  EXPECT_EQ(c0.session_id(), 14u);
  EXPECT_EQ(c0.offset(), 0u);
  EXPECT_EQ(c0.window_end_offset(), 64u);

  // Simulate one less timeout than the maximum amount of retries.
  for (unsigned retry = 1; retry <= kTestRetries - 1; ++retry) {
    transfer_thread_.SimulateClientTimeout(14);
    ASSERT_EQ(payloads.size(), retry + 1);

    Chunk c = DecodeChunk(payloads.back());
    EXPECT_EQ(c.session_id(), 14u);
    EXPECT_EQ(c.offset(), 0u);
    EXPECT_EQ(c.window_end_offset(), 64u);

    // Transfer has not yet completed.
    EXPECT_EQ(transfer_status, Status::Unknown());
  }

  // Send some data.
  context_.server().SendServerStream<Transfer::Read>(
      EncodeChunk(Chunk(ProtocolVersion::kLegacy, Chunk::Type::kData)
                      .set_session_id(14)
                      .set_offset(0)
                      .set_payload(data.first(16))));
  transfer_thread_.WaitUntilEventIsProcessed();
  ASSERT_EQ(payloads.size(), 3u);

  // Time out a couple more times. The context's retry count should have been
  // reset, so it should go through the standard retry flow instead of
  // terminating the transfer.
  transfer_thread_.SimulateClientTimeout(14);
  ASSERT_EQ(payloads.size(), 4u);

  Chunk c = DecodeChunk(payloads.back());
  EXPECT_FALSE(c.status().has_value());
  EXPECT_EQ(c.session_id(), 14u);
  EXPECT_EQ(c.offset(), 16u);
  EXPECT_EQ(c.window_end_offset(), 64u);

  transfer_thread_.SimulateClientTimeout(14);
  ASSERT_EQ(payloads.size(), 5u);

  c = DecodeChunk(payloads.back());
  EXPECT_FALSE(c.status().has_value());
  EXPECT_EQ(c.session_id(), 14u);
  EXPECT_EQ(c.offset(), 16u);
  EXPECT_EQ(c.window_end_offset(), 64u);

  // Ensure we don't leave a dangling reference to transfer_status.
  client_.CancelTransfer(14);
  transfer_thread_.WaitUntilEventIsProcessed();
}

TEST_F(ReadTransfer, InitialPacketFails_OnCompletedCalledWithDataLoss) {
  stream::MemoryWriterBuffer<64> writer;
  Status transfer_status = Status::Unknown();

  context_.output().set_send_status(Status::Unauthenticated());

  ASSERT_EQ(OkStatus(),
            client_.Read(
                14,
                writer,
                [&transfer_status](Status status) {
                  ASSERT_EQ(transfer_status,
                            Status::Unknown());  // Must only call once
                  transfer_status = status;
                },
                kTestTimeout));
  transfer_thread_.WaitUntilEventIsProcessed();

  EXPECT_EQ(transfer_status, Status::Internal());
}

class WriteTransfer : public ::testing::Test {
 protected:
  WriteTransfer()
      : transfer_thread_(chunk_buffer_, encode_buffer_),
        client_(context_.client(), context_.channel().id(), transfer_thread_),
        system_thread_(TransferThreadOptions(), transfer_thread_) {}

  ~WriteTransfer() override {
    transfer_thread_.Terminate();
    system_thread_.join();
  }

  rpc::RawClientTestContext<> context_;

  Thread<1, 1> transfer_thread_;
  Client client_;

  std::array<std::byte, 64> chunk_buffer_;
  std::array<std::byte, 64> encode_buffer_;

  thread::Thread system_thread_;
};

TEST_F(WriteTransfer, SingleChunk) {
  stream::MemoryReader reader(kData32);
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Write(3, reader, [&transfer_status](Status status) {
              transfer_status = status;
            }));
  transfer_thread_.WaitUntilEventIsProcessed();

  // The client begins by sending the ID of the resource to transfer.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Write>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk c0 = DecodeChunk(payloads[0]);
  EXPECT_EQ(c0.session_id(), 3u);
  EXPECT_EQ(c0.resource_id(), 3u);
  EXPECT_EQ(c0.type(), Chunk::Type::kStart);

  // Send transfer parameters. Client should send a data chunk and the final
  // chunk.
  rpc::test::WaitForPackets(context_.output(), 2, [this] {
    context_.server().SendServerStream<Transfer::Write>(EncodeChunk(
        Chunk(ProtocolVersion::kLegacy, Chunk::Type::kParametersRetransmit)
            .set_session_id(3)
            .set_offset(0)
            .set_window_end_offset(64)
            .set_max_chunk_size_bytes(32)));
  });

  ASSERT_EQ(payloads.size(), 3u);

  Chunk c1 = DecodeChunk(payloads[1]);
  EXPECT_EQ(c1.session_id(), 3u);
  EXPECT_EQ(c1.offset(), 0u);
  EXPECT_TRUE(c1.has_payload());
  EXPECT_EQ(
      std::memcmp(c1.payload().data(), kData32.data(), c1.payload().size()), 0);

  Chunk c2 = DecodeChunk(payloads[2]);
  EXPECT_EQ(c2.session_id(), 3u);
  ASSERT_TRUE(c2.remaining_bytes().has_value());
  EXPECT_EQ(c2.remaining_bytes().value(), 0u);

  EXPECT_EQ(transfer_status, Status::Unknown());

  // Send the final status chunk to complete the transfer.
  context_.server().SendServerStream<Transfer::Write>(
      EncodeChunk(Chunk::Final(ProtocolVersion::kLegacy, 3, OkStatus())));
  transfer_thread_.WaitUntilEventIsProcessed();

  EXPECT_EQ(payloads.size(), 3u);
  EXPECT_EQ(transfer_status, OkStatus());
}

TEST_F(WriteTransfer, MultiChunk) {
  stream::MemoryReader reader(kData32);
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Write(4, reader, [&transfer_status](Status status) {
              transfer_status = status;
            }));
  transfer_thread_.WaitUntilEventIsProcessed();

  // The client begins by sending the ID of the resource to transfer.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Write>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk c0 = DecodeChunk(payloads[0]);
  EXPECT_EQ(c0.session_id(), 4u);
  EXPECT_EQ(c0.resource_id(), 4u);
  EXPECT_EQ(c0.type(), Chunk::Type::kStart);

  // Send transfer parameters with a chunk size smaller than the data.

  // Client should send two data chunks and the final chunk.
  rpc::test::WaitForPackets(context_.output(), 3, [this] {
    context_.server().SendServerStream<Transfer::Write>(EncodeChunk(
        Chunk(ProtocolVersion::kLegacy, Chunk::Type::kParametersRetransmit)
            .set_session_id(4)
            .set_offset(0)
            .set_window_end_offset(64)
            .set_max_chunk_size_bytes(16)));
  });

  ASSERT_EQ(payloads.size(), 4u);

  Chunk c1 = DecodeChunk(payloads[1]);
  EXPECT_EQ(c1.session_id(), 4u);
  EXPECT_EQ(c1.offset(), 0u);
  EXPECT_TRUE(c1.has_payload());
  EXPECT_EQ(
      std::memcmp(c1.payload().data(), kData32.data(), c1.payload().size()), 0);

  Chunk c2 = DecodeChunk(payloads[2]);
  EXPECT_EQ(c2.session_id(), 4u);
  EXPECT_EQ(c2.offset(), 16u);
  EXPECT_TRUE(c2.has_payload());
  EXPECT_EQ(std::memcmp(c2.payload().data(),
                        kData32.data() + c2.offset(),
                        c2.payload().size()),
            0);

  Chunk c3 = DecodeChunk(payloads[3]);
  EXPECT_EQ(c3.session_id(), 4u);
  ASSERT_TRUE(c3.remaining_bytes().has_value());
  EXPECT_EQ(c3.remaining_bytes().value(), 0u);

  EXPECT_EQ(transfer_status, Status::Unknown());

  // Send the final status chunk to complete the transfer.
  context_.server().SendServerStream<Transfer::Write>(
      EncodeChunk(Chunk::Final(ProtocolVersion::kLegacy, 4, OkStatus())));
  transfer_thread_.WaitUntilEventIsProcessed();

  EXPECT_EQ(payloads.size(), 4u);
  EXPECT_EQ(transfer_status, OkStatus());
}

TEST_F(WriteTransfer, OutOfOrder_SeekSupported) {
  stream::MemoryReader reader(kData32);
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Write(5, reader, [&transfer_status](Status status) {
              transfer_status = status;
            }));
  transfer_thread_.WaitUntilEventIsProcessed();

  // The client begins by sending the ID of the resource to transfer.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Write>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk c0 = DecodeChunk(payloads[0]);
  EXPECT_EQ(c0.session_id(), 5u);
  EXPECT_EQ(c0.resource_id(), 5u);
  EXPECT_EQ(c0.type(), Chunk::Type::kStart);

  // Send transfer parameters with a nonzero offset, requesting a seek.
  // Client should send a data chunk and the final chunk.
  rpc::test::WaitForPackets(context_.output(), 2, [this] {
    context_.server().SendServerStream<Transfer::Write>(EncodeChunk(
        Chunk(ProtocolVersion::kLegacy, Chunk::Type::kParametersRetransmit)
            .set_session_id(5)
            .set_offset(16)
            .set_window_end_offset(64)
            .set_max_chunk_size_bytes(32)));
  });

  ASSERT_EQ(payloads.size(), 3u);

  Chunk c1 = DecodeChunk(payloads[1]);
  EXPECT_EQ(c1.session_id(), 5u);
  EXPECT_EQ(c1.offset(), 16u);
  EXPECT_TRUE(c1.has_payload());
  EXPECT_EQ(std::memcmp(c1.payload().data(),
                        kData32.data() + c1.offset(),
                        c1.payload().size()),
            0);

  Chunk c2 = DecodeChunk(payloads[2]);
  EXPECT_EQ(c2.session_id(), 5u);
  ASSERT_TRUE(c2.remaining_bytes().has_value());
  EXPECT_EQ(c2.remaining_bytes().value(), 0u);

  EXPECT_EQ(transfer_status, Status::Unknown());

  // Send the final status chunk to complete the transfer.
  context_.server().SendServerStream<Transfer::Write>(
      EncodeChunk(Chunk::Final(ProtocolVersion::kLegacy, 5, OkStatus())));
  transfer_thread_.WaitUntilEventIsProcessed();

  EXPECT_EQ(payloads.size(), 3u);
  EXPECT_EQ(transfer_status, OkStatus());
}

class FakeNonSeekableReader final : public stream::NonSeekableReader {
 public:
  FakeNonSeekableReader(ConstByteSpan data) : data_(data), position_(0) {}

 private:
  StatusWithSize DoRead(ByteSpan out) final {
    if (position_ == data_.size()) {
      return StatusWithSize::OutOfRange();
    }

    size_t to_copy = std::min(out.size(), data_.size() - position_);
    std::memcpy(out.data(), data_.data() + position_, to_copy);
    position_ += to_copy;

    return StatusWithSize(to_copy);
  }

  ConstByteSpan data_;
  size_t position_;
};

TEST_F(WriteTransfer, OutOfOrder_SeekNotSupported) {
  FakeNonSeekableReader reader(kData32);
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Write(6, reader, [&transfer_status](Status status) {
              transfer_status = status;
            }));
  transfer_thread_.WaitUntilEventIsProcessed();

  // The client begins by sending the ID of the resource to transfer.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Write>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk c0 = DecodeChunk(payloads[0]);
  EXPECT_EQ(c0.session_id(), 6u);
  EXPECT_EQ(c0.resource_id(), 6u);
  EXPECT_EQ(c0.type(), Chunk::Type::kStart);

  // Send transfer parameters with a nonzero offset, requesting a seek.
  context_.server().SendServerStream<Transfer::Write>(EncodeChunk(
      Chunk(ProtocolVersion::kLegacy, Chunk::Type::kParametersRetransmit)
          .set_session_id(6)
          .set_offset(16)
          .set_window_end_offset(64)
          .set_max_chunk_size_bytes(32)));
  transfer_thread_.WaitUntilEventIsProcessed();

  // Client should send a status chunk and end the transfer.
  ASSERT_EQ(payloads.size(), 2u);

  Chunk c1 = DecodeChunk(payloads[1]);
  EXPECT_EQ(c1.session_id(), 6u);
  EXPECT_EQ(c1.type(), Chunk::Type::kCompletion);
  ASSERT_TRUE(c1.status().has_value());
  EXPECT_EQ(c1.status().value(), Status::Unimplemented());

  EXPECT_EQ(transfer_status, Status::Unimplemented());
}

TEST_F(WriteTransfer, ServerError) {
  stream::MemoryReader reader(kData32);
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Write(7, reader, [&transfer_status](Status status) {
              transfer_status = status;
            }));
  transfer_thread_.WaitUntilEventIsProcessed();

  // The client begins by sending the ID of the resource to transfer.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Write>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk c0 = DecodeChunk(payloads[0]);
  EXPECT_EQ(c0.session_id(), 7u);
  EXPECT_EQ(c0.resource_id(), 7u);
  EXPECT_EQ(c0.type(), Chunk::Type::kStart);

  // Send an error from the server.
  context_.server().SendServerStream<Transfer::Write>(EncodeChunk(
      Chunk::Final(ProtocolVersion::kLegacy, 7, Status::NotFound())));
  transfer_thread_.WaitUntilEventIsProcessed();

  // Client should not respond and terminate the transfer.
  EXPECT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::NotFound());
}

TEST_F(WriteTransfer, AbortIfZeroBytesAreRequested) {
  stream::MemoryReader reader(kData32);
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Write(9, reader, [&transfer_status](Status status) {
              transfer_status = status;
            }));
  transfer_thread_.WaitUntilEventIsProcessed();

  // The client begins by sending the ID of the resource to transfer.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Write>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk c0 = DecodeChunk(payloads[0]);
  EXPECT_EQ(c0.session_id(), 9u);
  EXPECT_EQ(c0.resource_id(), 9u);
  EXPECT_EQ(c0.type(), Chunk::Type::kStart);

  // Send an invalid transfer parameters chunk with 0 pending bytes.
  context_.server().SendServerStream<Transfer::Write>(EncodeChunk(
      Chunk(ProtocolVersion::kLegacy, Chunk::Type::kParametersRetransmit)
          .set_session_id(9)
          .set_offset(0)
          .set_window_end_offset(0)
          .set_max_chunk_size_bytes(32)));
  transfer_thread_.WaitUntilEventIsProcessed();

  // Client should send a status chunk and end the transfer.
  ASSERT_EQ(payloads.size(), 2u);

  Chunk c1 = DecodeChunk(payloads[1]);
  EXPECT_EQ(c1.session_id(), 9u);
  ASSERT_TRUE(c1.status().has_value());
  EXPECT_EQ(c1.status().value(), Status::ResourceExhausted());

  EXPECT_EQ(transfer_status, Status::ResourceExhausted());
}

TEST_F(WriteTransfer, Timeout_RetriesWithInitialChunk) {
  stream::MemoryReader reader(kData32);
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Write(
                10,
                reader,
                [&transfer_status](Status status) { transfer_status = status; },
                kTestTimeout));
  transfer_thread_.WaitUntilEventIsProcessed();

  // The client begins by sending the ID of the resource to transfer.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Write>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk c0 = DecodeChunk(payloads.back());
  EXPECT_EQ(c0.session_id(), 10u);
  EXPECT_EQ(c0.resource_id(), 10u);
  EXPECT_EQ(c0.type(), Chunk::Type::kStart);

  // Wait for the timeout to expire without doing anything. The client should
  // resend the initial transmit chunk.
  transfer_thread_.SimulateClientTimeout(10);
  ASSERT_EQ(payloads.size(), 2u);

  Chunk c = DecodeChunk(payloads.back());
  EXPECT_EQ(c.session_id(), 10u);
  EXPECT_EQ(c.resource_id(), 10u);
  EXPECT_EQ(c.type(), Chunk::Type::kStart);

  // Transfer has not yet completed.
  EXPECT_EQ(transfer_status, Status::Unknown());

  // Ensure we don't leave a dangling reference to transfer_status.
  client_.CancelTransfer(10);
  transfer_thread_.WaitUntilEventIsProcessed();
}

TEST_F(WriteTransfer, Timeout_RetriesWithMostRecentChunk) {
  stream::MemoryReader reader(kData32);
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Write(
                11,
                reader,
                [&transfer_status](Status status) { transfer_status = status; },
                kTestTimeout));
  transfer_thread_.WaitUntilEventIsProcessed();

  // The client begins by sending the ID of the resource to transfer.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Write>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk c0 = DecodeChunk(payloads.back());
  EXPECT_EQ(c0.session_id(), 11u);
  EXPECT_EQ(c0.resource_id(), 11u);
  EXPECT_EQ(c0.type(), Chunk::Type::kStart);

  // Send the first parameters chunk.
  rpc::test::WaitForPackets(context_.output(), 2, [this] {
    context_.server().SendServerStream<Transfer::Write>(EncodeChunk(
        Chunk(ProtocolVersion::kLegacy, Chunk::Type::kParametersRetransmit)
            .set_session_id(11)
            .set_offset(0)
            .set_window_end_offset(16)
            .set_max_chunk_size_bytes(8)));
  });
  ASSERT_EQ(payloads.size(), 3u);

  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk c1 = DecodeChunk(payloads[1]);
  EXPECT_EQ(c1.session_id(), 11u);
  EXPECT_EQ(c1.offset(), 0u);
  EXPECT_EQ(c1.payload().size(), 8u);
  EXPECT_EQ(
      std::memcmp(c1.payload().data(), kData32.data(), c1.payload().size()), 0);

  Chunk c2 = DecodeChunk(payloads[2]);
  EXPECT_EQ(c2.session_id(), 11u);
  EXPECT_EQ(c2.offset(), 8u);
  EXPECT_EQ(c2.payload().size(), 8u);
  EXPECT_EQ(std::memcmp(c2.payload().data(),
                        kData32.data() + c2.offset(),
                        c2.payload().size()),
            0);

  // Wait for the timeout to expire without doing anything. The client should
  // resend the most recently sent chunk.
  transfer_thread_.SimulateClientTimeout(11);
  ASSERT_EQ(payloads.size(), 4u);

  Chunk c3 = DecodeChunk(payloads[3]);
  EXPECT_EQ(c3.session_id(), c2.session_id());
  EXPECT_EQ(c3.offset(), c2.offset());
  EXPECT_EQ(c3.payload().size(), c2.payload().size());
  EXPECT_EQ(std::memcmp(
                c3.payload().data(), c2.payload().data(), c3.payload().size()),
            0);

  // Transfer has not yet completed.
  EXPECT_EQ(transfer_status, Status::Unknown());

  // Ensure we don't leave a dangling reference to transfer_status.
  client_.CancelTransfer(11);
  transfer_thread_.WaitUntilEventIsProcessed();
}

TEST_F(WriteTransfer, Timeout_RetriesWithSingleChunkTransfer) {
  stream::MemoryReader reader(kData32);
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Write(
                12,
                reader,
                [&transfer_status](Status status) { transfer_status = status; },
                kTestTimeout));
  transfer_thread_.WaitUntilEventIsProcessed();

  // The client begins by sending the ID of the resource to transfer.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Write>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk c0 = DecodeChunk(payloads.back());
  EXPECT_EQ(c0.session_id(), 12u);
  EXPECT_EQ(c0.resource_id(), 12u);
  EXPECT_EQ(c0.type(), Chunk::Type::kStart);

  // Send the first parameters chunk, requesting all the data. The client should
  // respond with one data chunk and a remaining_bytes = 0 chunk.
  rpc::test::WaitForPackets(context_.output(), 2, [this] {
    context_.server().SendServerStream<Transfer::Write>(EncodeChunk(
        Chunk(ProtocolVersion::kLegacy, Chunk::Type::kParametersRetransmit)
            .set_session_id(12)
            .set_offset(0)
            .set_window_end_offset(64)
            .set_max_chunk_size_bytes(64)));
  });
  ASSERT_EQ(payloads.size(), 3u);

  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk c1 = DecodeChunk(payloads[1]);
  EXPECT_EQ(c1.session_id(), 12u);
  EXPECT_EQ(c1.offset(), 0u);
  EXPECT_EQ(c1.payload().size(), 32u);
  EXPECT_EQ(
      std::memcmp(c1.payload().data(), kData32.data(), c1.payload().size()), 0);

  Chunk c2 = DecodeChunk(payloads[2]);
  EXPECT_EQ(c2.session_id(), 12u);
  ASSERT_TRUE(c2.remaining_bytes().has_value());
  EXPECT_EQ(c2.remaining_bytes().value(), 0u);

  // Wait for the timeout to expire without doing anything. The client should
  // resend the data chunk.
  transfer_thread_.SimulateClientTimeout(12);
  ASSERT_EQ(payloads.size(), 4u);

  Chunk c3 = DecodeChunk(payloads[3]);
  EXPECT_EQ(c3.session_id(), c1.session_id());
  EXPECT_EQ(c3.offset(), c1.offset());
  EXPECT_EQ(c3.payload().size(), c1.payload().size());
  EXPECT_EQ(std::memcmp(
                c3.payload().data(), c1.payload().data(), c3.payload().size()),
            0);

  // The remaining_bytes = 0 chunk should be resent on the next parameters.
  context_.server().SendServerStream<Transfer::Write>(EncodeChunk(
      Chunk(ProtocolVersion::kLegacy, Chunk::Type::kParametersRetransmit)
          .set_session_id(12)
          .set_offset(32)
          .set_window_end_offset(64)));
  transfer_thread_.WaitUntilEventIsProcessed();

  ASSERT_EQ(payloads.size(), 5u);

  Chunk c4 = DecodeChunk(payloads[4]);
  EXPECT_EQ(c4.session_id(), 12u);
  ASSERT_TRUE(c4.remaining_bytes().has_value());
  EXPECT_EQ(c4.remaining_bytes().value(), 0u);

  context_.server().SendServerStream<Transfer::Write>(
      EncodeChunk(Chunk::Final(ProtocolVersion::kLegacy, 12, OkStatus())));
  transfer_thread_.WaitUntilEventIsProcessed();

  EXPECT_EQ(transfer_status, OkStatus());
}

TEST_F(WriteTransfer, Timeout_EndsTransferAfterMaxRetries) {
  stream::MemoryReader reader(kData32);
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Write(
                13,
                reader,
                [&transfer_status](Status status) { transfer_status = status; },
                kTestTimeout));
  transfer_thread_.WaitUntilEventIsProcessed();

  // The client begins by sending the ID of the resource to transfer.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Write>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk c0 = DecodeChunk(payloads.back());
  EXPECT_EQ(c0.session_id(), 13u);
  EXPECT_EQ(c0.resource_id(), 13u);
  EXPECT_EQ(c0.type(), Chunk::Type::kStart);

  for (unsigned retry = 1; retry <= kTestRetries; ++retry) {
    // Wait for the timeout to expire without doing anything. The client should
    // resend the initial transmit chunk.
    transfer_thread_.SimulateClientTimeout(13);
    ASSERT_EQ(payloads.size(), retry + 1);

    Chunk c = DecodeChunk(payloads.back());
    EXPECT_EQ(c.session_id(), 13u);
    EXPECT_EQ(c.resource_id(), 13u);
    EXPECT_EQ(c.type(), Chunk::Type::kStart);

    // Transfer has not yet completed.
    EXPECT_EQ(transfer_status, Status::Unknown());
  }

  // Sleep one more time after the final retry. The client should cancel the
  // transfer at this point. As no packets were received from the server, no
  // final status chunk should be sent.
  transfer_thread_.SimulateClientTimeout(13);
  ASSERT_EQ(payloads.size(), 4u);

  EXPECT_EQ(transfer_status, Status::DeadlineExceeded());

  // After finishing the transfer, nothing else should be sent. Verify this by
  // waiting for a bit.
  this_thread::sleep_for(kTestTimeout * 4);
  ASSERT_EQ(payloads.size(), 4u);

  // Ensure we don't leave a dangling reference to transfer_status.
  client_.CancelTransfer(13);
  transfer_thread_.WaitUntilEventIsProcessed();
}

TEST_F(WriteTransfer, Timeout_NonSeekableReaderEndsTransfer) {
  FakeNonSeekableReader reader(kData32);
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Write(
                14,
                reader,
                [&transfer_status](Status status) { transfer_status = status; },
                kTestTimeout));
  transfer_thread_.WaitUntilEventIsProcessed();

  // The client begins by sending the ID of the resource to transfer.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Write>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk c0 = DecodeChunk(payloads.back());
  EXPECT_EQ(c0.session_id(), 14u);
  EXPECT_EQ(c0.resource_id(), 14u);
  EXPECT_EQ(c0.type(), Chunk::Type::kStart);

  // Send the first parameters chunk.
  rpc::test::WaitForPackets(context_.output(), 2, [this] {
    context_.server().SendServerStream<Transfer::Write>(EncodeChunk(
        Chunk(ProtocolVersion::kLegacy, Chunk::Type::kParametersRetransmit)
            .set_session_id(14)
            .set_offset(0)
            .set_window_end_offset(16)
            .set_max_chunk_size_bytes(8)));
  });
  ASSERT_EQ(payloads.size(), 3u);

  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk c1 = DecodeChunk(payloads[1]);
  EXPECT_EQ(c1.session_id(), 14u);
  EXPECT_EQ(c1.offset(), 0u);
  EXPECT_TRUE(c1.has_payload());
  EXPECT_EQ(c1.payload().size(), 8u);
  EXPECT_EQ(
      std::memcmp(c1.payload().data(), kData32.data(), c1.payload().size()), 0);

  Chunk c2 = DecodeChunk(payloads[2]);
  EXPECT_EQ(c2.session_id(), 14u);
  EXPECT_EQ(c2.offset(), 8u);
  EXPECT_TRUE(c2.has_payload());
  EXPECT_EQ(c2.payload().size(), 8u);
  EXPECT_EQ(std::memcmp(c2.payload().data(),
                        kData32.data() + c2.offset(),
                        c2.payload().size()),
            0);

  // Wait for the timeout to expire without doing anything. The client should
  // fail to seek back and end the transfer.
  transfer_thread_.SimulateClientTimeout(14);
  ASSERT_EQ(payloads.size(), 4u);

  Chunk c3 = DecodeChunk(payloads[3]);
  EXPECT_EQ(c3.protocol_version(), ProtocolVersion::kLegacy);
  EXPECT_EQ(c3.session_id(), 14u);
  ASSERT_TRUE(c3.status().has_value());
  EXPECT_EQ(c3.status().value(), Status::DeadlineExceeded());

  EXPECT_EQ(transfer_status, Status::DeadlineExceeded());

  // Ensure we don't leave a dangling reference to transfer_status.
  client_.CancelTransfer(14);
  transfer_thread_.WaitUntilEventIsProcessed();
}

TEST_F(WriteTransfer, ManualCancel) {
  stream::MemoryReader reader(kData32);
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Write(
                15,
                reader,
                [&transfer_status](Status status) { transfer_status = status; },
                kTestTimeout));
  transfer_thread_.WaitUntilEventIsProcessed();

  // The client begins by sending the ID of the resource to transfer.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Write>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk chunk = DecodeChunk(payloads.back());
  EXPECT_EQ(chunk.session_id(), 15u);
  EXPECT_EQ(chunk.resource_id(), 15u);
  EXPECT_EQ(chunk.type(), Chunk::Type::kStart);

  // Get a response from the server, then cancel the transfer.
  context_.server().SendServerStream<Transfer::Write>(EncodeChunk(
      Chunk(ProtocolVersion::kLegacy, Chunk::Type::kParametersRetransmit)
          .set_session_id(15)
          .set_offset(0)
          .set_window_end_offset(64)
          .set_max_chunk_size_bytes(32)));
  transfer_thread_.WaitUntilEventIsProcessed();
  ASSERT_EQ(payloads.size(), 2u);

  client_.CancelTransfer(15);
  transfer_thread_.WaitUntilEventIsProcessed();

  // Client should send a cancellation chunk to the server.
  ASSERT_EQ(payloads.size(), 3u);
  chunk = DecodeChunk(payloads.back());
  EXPECT_EQ(chunk.session_id(), 15u);
  ASSERT_EQ(chunk.type(), Chunk::Type::kCompletion);
  EXPECT_EQ(chunk.status().value(), Status::Cancelled());

  EXPECT_EQ(transfer_status, Status::Cancelled());
}

TEST_F(WriteTransfer, ManualCancel_NoContact) {
  stream::MemoryReader reader(kData32);
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Write(
                15,
                reader,
                [&transfer_status](Status status) { transfer_status = status; },
                kTestTimeout));
  transfer_thread_.WaitUntilEventIsProcessed();

  // The client begins by sending the ID of the resource to transfer.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Write>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk chunk = DecodeChunk(payloads.back());
  EXPECT_EQ(chunk.session_id(), 15u);
  EXPECT_EQ(chunk.resource_id(), 15u);
  EXPECT_EQ(chunk.type(), Chunk::Type::kStart);

  // Cancel transfer without a server response. No final chunk should be sent.
  client_.CancelTransfer(15);
  transfer_thread_.WaitUntilEventIsProcessed();

  ASSERT_EQ(payloads.size(), 1u);

  EXPECT_EQ(transfer_status, Status::Cancelled());
}

TEST_F(ReadTransfer, Version2_SingleChunk) {
  stream::MemoryWriterBuffer<64> writer;
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Read(
                3,
                writer,
                [&transfer_status](Status status) { transfer_status = status; },
                cfg::kDefaultChunkTimeout,
                ProtocolVersion::kVersionTwo));

  transfer_thread_.WaitUntilEventIsProcessed();

  // Initial chunk of the transfer is sent. This chunk should contain all the
  // fields from both legacy and version 2 protocols for backwards
  // compatibility.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Read>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk chunk = DecodeChunk(payloads[0]);
  EXPECT_EQ(chunk.type(), Chunk::Type::kStart);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  EXPECT_EQ(chunk.session_id(), 3u);
  EXPECT_EQ(chunk.resource_id(), 3u);
  EXPECT_EQ(chunk.offset(), 0u);
  EXPECT_EQ(chunk.window_end_offset(), 64u);
  EXPECT_EQ(chunk.max_chunk_size_bytes(), 37u);

  // The server responds with a START_ACK, continuing the version 2 handshake
  // and assigning a session_id to the transfer.
  context_.server().SendServerStream<Transfer::Read>(
      EncodeChunk(Chunk(ProtocolVersion::kVersionTwo, Chunk::Type::kStartAck)
                      .set_session_id(29)
                      .set_resource_id(3)));
  transfer_thread_.WaitUntilEventIsProcessed();

  ASSERT_EQ(payloads.size(), 2u);

  // Client should accept the session_id with a START_ACK_CONFIRMATION,
  // additionally containing the initial parameters for the read transfer.
  chunk = DecodeChunk(payloads.back());
  EXPECT_EQ(chunk.type(), Chunk::Type::kStartAckConfirmation);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  EXPECT_EQ(chunk.session_id(), 29u);
  EXPECT_FALSE(chunk.resource_id().has_value());
  EXPECT_EQ(chunk.offset(), 0u);
  EXPECT_EQ(chunk.window_end_offset(), 64u);
  EXPECT_EQ(chunk.max_chunk_size_bytes(), 37u);

  // Send all the transfer data. Client should accept it and complete the
  // transfer.
  context_.server().SendServerStream<Transfer::Read>(
      EncodeChunk(Chunk(ProtocolVersion::kVersionTwo, Chunk::Type::kData)
                      .set_session_id(29)
                      .set_offset(0)
                      .set_payload(kData32)
                      .set_remaining_bytes(0)));
  transfer_thread_.WaitUntilEventIsProcessed();

  ASSERT_EQ(payloads.size(), 3u);

  chunk = DecodeChunk(payloads.back());
  EXPECT_EQ(chunk.session_id(), 29u);
  EXPECT_EQ(chunk.type(), Chunk::Type::kCompletion);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  ASSERT_TRUE(chunk.status().has_value());
  EXPECT_EQ(chunk.status().value(), OkStatus());

  EXPECT_EQ(transfer_status, OkStatus());
  EXPECT_EQ(std::memcmp(writer.data(), kData32.data(), writer.bytes_written()),
            0);

  context_.server().SendServerStream<Transfer::Read>(EncodeChunk(
      Chunk(ProtocolVersion::kVersionTwo, Chunk::Type::kCompletionAck)
          .set_session_id(29)));
}

TEST_F(ReadTransfer, Version2_ServerRunsLegacy) {
  stream::MemoryWriterBuffer<64> writer;
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Read(
                3,
                writer,
                [&transfer_status](Status status) { transfer_status = status; },
                cfg::kDefaultChunkTimeout,
                ProtocolVersion::kVersionTwo));

  transfer_thread_.WaitUntilEventIsProcessed();

  // Initial chunk of the transfer is sent. This chunk should contain all the
  // fields from both legacy and version 2 protocols for backwards
  // compatibility.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Read>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk chunk = DecodeChunk(payloads[0]);
  EXPECT_EQ(chunk.type(), Chunk::Type::kStart);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  EXPECT_EQ(chunk.session_id(), 3u);
  EXPECT_EQ(chunk.resource_id(), 3u);
  EXPECT_EQ(chunk.offset(), 0u);
  EXPECT_EQ(chunk.window_end_offset(), 64u);
  EXPECT_EQ(chunk.max_chunk_size_bytes(), 37u);

  // Instead of a START_ACK to continue the handshake, the server responds with
  // an immediate data chunk, indicating that it is running the legacy protocol
  // version. Client should revert to legacy, using the resource_id of 3 as the
  // session_id, and complete the transfer.
  context_.server().SendServerStream<Transfer::Read>(
      EncodeChunk(Chunk(ProtocolVersion::kLegacy, Chunk::Type::kData)
                      .set_session_id(3)
                      .set_offset(0)
                      .set_payload(kData32)
                      .set_remaining_bytes(0)));
  transfer_thread_.WaitUntilEventIsProcessed();

  ASSERT_EQ(payloads.size(), 2u);

  chunk = DecodeChunk(payloads.back());
  EXPECT_EQ(chunk.session_id(), 3u);
  EXPECT_EQ(chunk.type(), Chunk::Type::kCompletion);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kLegacy);
  ASSERT_TRUE(chunk.status().has_value());
  EXPECT_EQ(chunk.status().value(), OkStatus());

  EXPECT_EQ(transfer_status, OkStatus());
  EXPECT_EQ(std::memcmp(writer.data(), kData32.data(), writer.bytes_written()),
            0);
}

TEST_F(ReadTransfer, Version2_TimeoutDuringHandshake) {
  stream::MemoryWriterBuffer<64> writer;
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Read(
                3,
                writer,
                [&transfer_status](Status status) { transfer_status = status; },
                cfg::kDefaultChunkTimeout,
                ProtocolVersion::kVersionTwo));

  transfer_thread_.WaitUntilEventIsProcessed();

  // Initial chunk of the transfer is sent. This chunk should contain all the
  // fields from both legacy and version 2 protocols for backwards
  // compatibility.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Read>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk chunk = DecodeChunk(payloads.back());
  EXPECT_EQ(chunk.type(), Chunk::Type::kStart);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  EXPECT_EQ(chunk.session_id(), 3u);
  EXPECT_EQ(chunk.resource_id(), 3u);
  EXPECT_EQ(chunk.offset(), 0u);
  EXPECT_EQ(chunk.window_end_offset(), 64u);
  EXPECT_EQ(chunk.max_chunk_size_bytes(), 37u);

  // Wait for the timeout to expire without doing anything. The client should
  // resend the initial chunk.
  transfer_thread_.SimulateClientTimeout(3);
  ASSERT_EQ(payloads.size(), 2u);

  chunk = DecodeChunk(payloads.back());
  EXPECT_EQ(chunk.type(), Chunk::Type::kStart);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  EXPECT_EQ(chunk.session_id(), 3u);
  EXPECT_EQ(chunk.resource_id(), 3u);
  EXPECT_EQ(chunk.offset(), 0u);
  EXPECT_EQ(chunk.window_end_offset(), 64u);
  EXPECT_EQ(chunk.max_chunk_size_bytes(), 37u);

  // This time, the server responds, continuing the handshake and transfer.
  context_.server().SendServerStream<Transfer::Read>(
      EncodeChunk(Chunk(ProtocolVersion::kVersionTwo, Chunk::Type::kStartAck)
                      .set_session_id(31)
                      .set_resource_id(3)));
  transfer_thread_.WaitUntilEventIsProcessed();

  ASSERT_EQ(payloads.size(), 3u);

  chunk = DecodeChunk(payloads.back());
  EXPECT_EQ(chunk.type(), Chunk::Type::kStartAckConfirmation);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  EXPECT_EQ(chunk.session_id(), 31u);
  EXPECT_FALSE(chunk.resource_id().has_value());
  EXPECT_EQ(chunk.offset(), 0u);
  EXPECT_EQ(chunk.window_end_offset(), 64u);
  EXPECT_EQ(chunk.max_chunk_size_bytes(), 37u);

  context_.server().SendServerStream<Transfer::Read>(
      EncodeChunk(Chunk(ProtocolVersion::kVersionTwo, Chunk::Type::kData)
                      .set_session_id(31)
                      .set_offset(0)
                      .set_payload(kData32)
                      .set_remaining_bytes(0)));
  transfer_thread_.WaitUntilEventIsProcessed();

  ASSERT_EQ(payloads.size(), 4u);

  chunk = DecodeChunk(payloads.back());
  EXPECT_EQ(chunk.session_id(), 31u);
  EXPECT_EQ(chunk.type(), Chunk::Type::kCompletion);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  ASSERT_TRUE(chunk.status().has_value());
  EXPECT_EQ(chunk.status().value(), OkStatus());

  EXPECT_EQ(transfer_status, OkStatus());
  EXPECT_EQ(std::memcmp(writer.data(), kData32.data(), writer.bytes_written()),
            0);

  context_.server().SendServerStream<Transfer::Read>(EncodeChunk(
      Chunk(ProtocolVersion::kVersionTwo, Chunk::Type::kCompletionAck)
          .set_session_id(31)));
}

TEST_F(ReadTransfer, Version2_TimeoutAfterHandshake) {
  stream::MemoryWriterBuffer<64> writer;
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Read(
                3,
                writer,
                [&transfer_status](Status status) { transfer_status = status; },
                cfg::kDefaultChunkTimeout,
                ProtocolVersion::kVersionTwo));

  transfer_thread_.WaitUntilEventIsProcessed();

  // Initial chunk of the transfer is sent. This chunk should contain all the
  // fields from both legacy and version 2 protocols for backwards
  // compatibility.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Read>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk chunk = DecodeChunk(payloads.back());
  EXPECT_EQ(chunk.type(), Chunk::Type::kStart);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  EXPECT_EQ(chunk.session_id(), 3u);
  EXPECT_EQ(chunk.resource_id(), 3u);
  EXPECT_EQ(chunk.offset(), 0u);
  EXPECT_EQ(chunk.window_end_offset(), 64u);
  EXPECT_EQ(chunk.max_chunk_size_bytes(), 37u);

  // The server responds with a START_ACK, continuing the version 2 handshake
  // and assigning a session_id to the transfer.
  context_.server().SendServerStream<Transfer::Read>(
      EncodeChunk(Chunk(ProtocolVersion::kVersionTwo, Chunk::Type::kStartAck)
                      .set_session_id(33)
                      .set_resource_id(3)));
  transfer_thread_.WaitUntilEventIsProcessed();

  ASSERT_EQ(payloads.size(), 2u);

  // Client should accept the session_id with a START_ACK_CONFIRMATION,
  // additionally containing the initial parameters for the read transfer.
  chunk = DecodeChunk(payloads.back());
  EXPECT_EQ(chunk.type(), Chunk::Type::kStartAckConfirmation);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  EXPECT_EQ(chunk.session_id(), 33u);
  EXPECT_FALSE(chunk.resource_id().has_value());
  EXPECT_EQ(chunk.offset(), 0u);
  EXPECT_EQ(chunk.window_end_offset(), 64u);
  EXPECT_EQ(chunk.max_chunk_size_bytes(), 37u);

  // Wait for the timeout to expire without doing anything. The client should
  // resend the confirmation chunk.
  transfer_thread_.SimulateClientTimeout(33);
  ASSERT_EQ(payloads.size(), 3u);

  chunk = DecodeChunk(payloads.back());
  EXPECT_EQ(chunk.type(), Chunk::Type::kStartAckConfirmation);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  EXPECT_EQ(chunk.session_id(), 33u);
  EXPECT_FALSE(chunk.resource_id().has_value());
  EXPECT_EQ(chunk.offset(), 0u);
  EXPECT_EQ(chunk.window_end_offset(), 64u);
  EXPECT_EQ(chunk.max_chunk_size_bytes(), 37u);

  // The server responds and the transfer should continue normally.
  context_.server().SendServerStream<Transfer::Read>(
      EncodeChunk(Chunk(ProtocolVersion::kVersionTwo, Chunk::Type::kData)
                      .set_session_id(33)
                      .set_offset(0)
                      .set_payload(kData32)
                      .set_remaining_bytes(0)));
  transfer_thread_.WaitUntilEventIsProcessed();

  ASSERT_EQ(payloads.size(), 4u);

  chunk = DecodeChunk(payloads.back());
  EXPECT_EQ(chunk.session_id(), 33u);
  EXPECT_EQ(chunk.type(), Chunk::Type::kCompletion);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  ASSERT_TRUE(chunk.status().has_value());
  EXPECT_EQ(chunk.status().value(), OkStatus());

  EXPECT_EQ(transfer_status, OkStatus());
  EXPECT_EQ(std::memcmp(writer.data(), kData32.data(), writer.bytes_written()),
            0);

  context_.server().SendServerStream<Transfer::Read>(EncodeChunk(
      Chunk(ProtocolVersion::kVersionTwo, Chunk::Type::kCompletionAck)
          .set_session_id(33)));
}

TEST_F(ReadTransfer, Version2_ServerErrorDuringHandshake) {
  stream::MemoryWriterBuffer<64> writer;
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Read(
                3,
                writer,
                [&transfer_status](Status status) { transfer_status = status; },
                cfg::kDefaultChunkTimeout,
                ProtocolVersion::kVersionTwo));

  transfer_thread_.WaitUntilEventIsProcessed();

  // Initial chunk of the transfer is sent. This chunk should contain all the
  // fields from both legacy and version 2 protocols for backwards
  // compatibility.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Read>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk chunk = DecodeChunk(payloads.back());
  EXPECT_EQ(chunk.type(), Chunk::Type::kStart);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  EXPECT_EQ(chunk.session_id(), 3u);
  EXPECT_EQ(chunk.resource_id(), 3u);
  EXPECT_EQ(chunk.offset(), 0u);
  EXPECT_EQ(chunk.window_end_offset(), 64u);
  EXPECT_EQ(chunk.max_chunk_size_bytes(), 37u);

  // The server responds to the start request with an error.
  context_.server().SendServerStream<Transfer::Read>(EncodeChunk(Chunk::Final(
      ProtocolVersion::kVersionTwo, 3, Status::Unauthenticated())));
  transfer_thread_.WaitUntilEventIsProcessed();

  EXPECT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unauthenticated());
}

TEST_F(ReadTransfer, Version2_TimeoutWaitingForCompletionAckRetries) {
  stream::MemoryWriterBuffer<64> writer;
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Read(
                3,
                writer,
                [&transfer_status](Status status) { transfer_status = status; },
                cfg::kDefaultChunkTimeout,
                ProtocolVersion::kVersionTwo));

  transfer_thread_.WaitUntilEventIsProcessed();

  // Initial chunk of the transfer is sent. This chunk should contain all the
  // fields from both legacy and version 2 protocols for backwards
  // compatibility.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Read>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk chunk = DecodeChunk(payloads[0]);
  EXPECT_EQ(chunk.type(), Chunk::Type::kStart);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  EXPECT_EQ(chunk.session_id(), 3u);
  EXPECT_EQ(chunk.resource_id(), 3u);
  EXPECT_EQ(chunk.offset(), 0u);
  EXPECT_EQ(chunk.window_end_offset(), 64u);
  EXPECT_EQ(chunk.max_chunk_size_bytes(), 37u);

  // The server responds with a START_ACK, continuing the version 2 handshake
  // and assigning a session_id to the transfer.
  context_.server().SendServerStream<Transfer::Read>(
      EncodeChunk(Chunk(ProtocolVersion::kVersionTwo, Chunk::Type::kStartAck)
                      .set_session_id(29)
                      .set_resource_id(3)));
  transfer_thread_.WaitUntilEventIsProcessed();

  ASSERT_EQ(payloads.size(), 2u);

  // Client should accept the session_id with a START_ACK_CONFIRMATION,
  // additionally containing the initial parameters for the read transfer.
  chunk = DecodeChunk(payloads.back());
  EXPECT_EQ(chunk.type(), Chunk::Type::kStartAckConfirmation);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  EXPECT_EQ(chunk.session_id(), 29u);
  EXPECT_FALSE(chunk.resource_id().has_value());
  EXPECT_EQ(chunk.offset(), 0u);
  EXPECT_EQ(chunk.window_end_offset(), 64u);
  EXPECT_EQ(chunk.max_chunk_size_bytes(), 37u);

  // Send all the transfer data. Client should accept it and complete the
  // transfer.
  context_.server().SendServerStream<Transfer::Read>(
      EncodeChunk(Chunk(ProtocolVersion::kVersionTwo, Chunk::Type::kData)
                      .set_session_id(29)
                      .set_offset(0)
                      .set_payload(kData32)
                      .set_remaining_bytes(0)));
  transfer_thread_.WaitUntilEventIsProcessed();

  ASSERT_EQ(payloads.size(), 3u);

  chunk = DecodeChunk(payloads.back());
  EXPECT_EQ(chunk.session_id(), 29u);
  EXPECT_EQ(chunk.type(), Chunk::Type::kCompletion);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  ASSERT_TRUE(chunk.status().has_value());
  EXPECT_EQ(chunk.status().value(), OkStatus());

  EXPECT_EQ(transfer_status, OkStatus());
  EXPECT_EQ(std::memcmp(writer.data(), kData32.data(), writer.bytes_written()),
            0);

  // Time out instead of sending a completion ACK. THe transfer should resend
  // its completion chunk.
  transfer_thread_.SimulateClientTimeout(29);
  ASSERT_EQ(payloads.size(), 4u);

  // Reset transfer_status to check whether the handler is called again.
  transfer_status = Status::Unknown();

  chunk = DecodeChunk(payloads.back());
  EXPECT_EQ(chunk.session_id(), 29u);
  EXPECT_EQ(chunk.type(), Chunk::Type::kCompletion);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  ASSERT_TRUE(chunk.status().has_value());
  EXPECT_EQ(chunk.status().value(), OkStatus());

  // Transfer handler should not be called a second time in response to the
  // re-sent completion chunk.
  EXPECT_EQ(transfer_status, Status::Unknown());

  // Send a completion ACK to end the transfer.
  context_.server().SendServerStream<Transfer::Read>(EncodeChunk(
      Chunk(ProtocolVersion::kVersionTwo, Chunk::Type::kCompletionAck)
          .set_session_id(29)));
  transfer_thread_.WaitUntilEventIsProcessed();

  // No further chunks should be sent following the ACK.
  transfer_thread_.SimulateClientTimeout(29);
  ASSERT_EQ(payloads.size(), 4u);
}

TEST_F(ReadTransfer,
       Version2_TimeoutWaitingForCompletionAckEndsTransferAfterRetries) {
  stream::MemoryWriterBuffer<64> writer;
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Read(
                3,
                writer,
                [&transfer_status](Status status) { transfer_status = status; },
                cfg::kDefaultChunkTimeout,
                ProtocolVersion::kVersionTwo));

  transfer_thread_.WaitUntilEventIsProcessed();

  // Initial chunk of the transfer is sent. This chunk should contain all the
  // fields from both legacy and version 2 protocols for backwards
  // compatibility.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Read>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk chunk = DecodeChunk(payloads[0]);
  EXPECT_EQ(chunk.type(), Chunk::Type::kStart);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  EXPECT_EQ(chunk.session_id(), 3u);
  EXPECT_EQ(chunk.resource_id(), 3u);
  EXPECT_EQ(chunk.offset(), 0u);
  EXPECT_EQ(chunk.window_end_offset(), 64u);
  EXPECT_EQ(chunk.max_chunk_size_bytes(), 37u);

  // The server responds with a START_ACK, continuing the version 2 handshake
  // and assigning a session_id to the transfer.
  context_.server().SendServerStream<Transfer::Read>(
      EncodeChunk(Chunk(ProtocolVersion::kVersionTwo, Chunk::Type::kStartAck)
                      .set_session_id(29)
                      .set_resource_id(3)));
  transfer_thread_.WaitUntilEventIsProcessed();

  ASSERT_EQ(payloads.size(), 2u);

  // Client should accept the session_id with a START_ACK_CONFIRMATION,
  // additionally containing the initial parameters for the read transfer.
  chunk = DecodeChunk(payloads.back());
  EXPECT_EQ(chunk.type(), Chunk::Type::kStartAckConfirmation);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  EXPECT_EQ(chunk.session_id(), 29u);
  EXPECT_FALSE(chunk.resource_id().has_value());
  EXPECT_EQ(chunk.offset(), 0u);
  EXPECT_EQ(chunk.window_end_offset(), 64u);
  EXPECT_EQ(chunk.max_chunk_size_bytes(), 37u);

  // Send all the transfer data. Client should accept it and complete the
  // transfer.
  context_.server().SendServerStream<Transfer::Read>(
      EncodeChunk(Chunk(ProtocolVersion::kVersionTwo, Chunk::Type::kData)
                      .set_session_id(29)
                      .set_offset(0)
                      .set_payload(kData32)
                      .set_remaining_bytes(0)));
  transfer_thread_.WaitUntilEventIsProcessed();

  ASSERT_EQ(payloads.size(), 3u);

  chunk = DecodeChunk(payloads.back());
  EXPECT_EQ(chunk.session_id(), 29u);
  EXPECT_EQ(chunk.type(), Chunk::Type::kCompletion);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  ASSERT_TRUE(chunk.status().has_value());
  EXPECT_EQ(chunk.status().value(), OkStatus());

  EXPECT_EQ(transfer_status, OkStatus());
  EXPECT_EQ(std::memcmp(writer.data(), kData32.data(), writer.bytes_written()),
            0);

  // Time out instead of sending a completion ACK. THe transfer should resend
  // its completion chunk at first, then terminate after the maximum number of
  // retries.
  transfer_thread_.SimulateClientTimeout(29);
  ASSERT_EQ(payloads.size(), 4u);  // Retry 1.

  transfer_thread_.SimulateClientTimeout(29);
  ASSERT_EQ(payloads.size(), 5u);  // Retry 2.

  transfer_thread_.SimulateClientTimeout(29);
  ASSERT_EQ(payloads.size(), 6u);  // Retry 3.

  transfer_thread_.SimulateClientTimeout(29);
  ASSERT_EQ(payloads.size(), 6u);  // No more retries; transfer has ended.
}

TEST_F(WriteTransfer, Version2_SingleChunk) {
  stream::MemoryReader reader(kData32);
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Write(
                3,
                reader,
                [&transfer_status](Status status) { transfer_status = status; },
                cfg::kDefaultChunkTimeout,
                ProtocolVersion::kVersionTwo));
  transfer_thread_.WaitUntilEventIsProcessed();

  // The client begins by sending the ID of the resource to transfer.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Write>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk chunk = DecodeChunk(payloads.back());
  EXPECT_EQ(chunk.type(), Chunk::Type::kStart);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  EXPECT_EQ(chunk.session_id(), 3u);
  EXPECT_EQ(chunk.resource_id(), 3u);

  // The server responds with a START_ACK, continuing the version 2 handshake
  // and assigning a session_id to the transfer.
  context_.server().SendServerStream<Transfer::Write>(
      EncodeChunk(Chunk(ProtocolVersion::kVersionTwo, Chunk::Type::kStartAck)
                      .set_session_id(29)
                      .set_resource_id(3)));
  transfer_thread_.WaitUntilEventIsProcessed();

  ASSERT_EQ(payloads.size(), 2u);

  // Client should accept the session_id with a START_ACK_CONFIRMATION.
  chunk = DecodeChunk(payloads.back());
  EXPECT_EQ(chunk.type(), Chunk::Type::kStartAckConfirmation);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  EXPECT_EQ(chunk.session_id(), 29u);
  EXPECT_FALSE(chunk.resource_id().has_value());

  // The server can then begin the data transfer by sending its transfer
  // parameters. Client should respond with a data chunk and the final chunk.
  rpc::test::WaitForPackets(context_.output(), 2, [this] {
    context_.server().SendServerStream<Transfer::Write>(EncodeChunk(
        Chunk(ProtocolVersion::kVersionTwo, Chunk::Type::kParametersRetransmit)
            .set_session_id(29)
            .set_offset(0)
            .set_window_end_offset(64)
            .set_max_chunk_size_bytes(32)));
  });

  ASSERT_EQ(payloads.size(), 4u);

  chunk = DecodeChunk(payloads[2]);
  EXPECT_EQ(chunk.type(), Chunk::Type::kData);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  EXPECT_EQ(chunk.session_id(), 29u);
  EXPECT_EQ(chunk.offset(), 0u);
  EXPECT_TRUE(chunk.has_payload());
  EXPECT_EQ(std::memcmp(
                chunk.payload().data(), kData32.data(), chunk.payload().size()),
            0);

  chunk = DecodeChunk(payloads[3]);
  EXPECT_EQ(chunk.type(), Chunk::Type::kData);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  EXPECT_EQ(chunk.session_id(), 29u);
  ASSERT_TRUE(chunk.remaining_bytes().has_value());
  EXPECT_EQ(chunk.remaining_bytes().value(), 0u);

  EXPECT_EQ(transfer_status, Status::Unknown());

  // Send the final status chunk to complete the transfer.
  context_.server().SendServerStream<Transfer::Write>(
      EncodeChunk(Chunk::Final(ProtocolVersion::kVersionTwo, 29, OkStatus())));
  transfer_thread_.WaitUntilEventIsProcessed();

  // Client should acknowledge the completion of the transfer.
  EXPECT_EQ(payloads.size(), 5u);

  chunk = DecodeChunk(payloads[4]);
  EXPECT_EQ(chunk.type(), Chunk::Type::kCompletionAck);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  EXPECT_EQ(chunk.session_id(), 29u);

  EXPECT_EQ(transfer_status, OkStatus());
}

TEST_F(WriteTransfer, Version2_ServerRunsLegacy) {
  stream::MemoryReader reader(kData32);
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Write(
                3,
                reader,
                [&transfer_status](Status status) { transfer_status = status; },
                cfg::kDefaultChunkTimeout,
                ProtocolVersion::kVersionTwo));
  transfer_thread_.WaitUntilEventIsProcessed();

  // The client begins by sending the ID of the resource to transfer.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Write>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk chunk = DecodeChunk(payloads.back());
  EXPECT_EQ(chunk.type(), Chunk::Type::kStart);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  EXPECT_EQ(chunk.session_id(), 3u);
  EXPECT_EQ(chunk.resource_id(), 3u);

  // Instead of continuing the handshake with a START_ACK, the server
  // immediately sends parameters, indicating that it only supports the legacy
  // protocol. Client should switch over to legacy and continue the transfer.
  rpc::test::WaitForPackets(context_.output(), 2, [this] {
    context_.server().SendServerStream<Transfer::Write>(EncodeChunk(
        Chunk(ProtocolVersion::kLegacy, Chunk::Type::kParametersRetransmit)
            .set_session_id(3)
            .set_offset(0)
            .set_window_end_offset(64)
            .set_max_chunk_size_bytes(32)));
  });

  ASSERT_EQ(payloads.size(), 3u);

  chunk = DecodeChunk(payloads[1]);
  EXPECT_EQ(chunk.type(), Chunk::Type::kData);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kLegacy);
  EXPECT_EQ(chunk.session_id(), 3u);
  EXPECT_EQ(chunk.offset(), 0u);
  EXPECT_TRUE(chunk.has_payload());
  EXPECT_EQ(std::memcmp(
                chunk.payload().data(), kData32.data(), chunk.payload().size()),
            0);

  chunk = DecodeChunk(payloads[2]);
  EXPECT_EQ(chunk.type(), Chunk::Type::kData);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kLegacy);
  EXPECT_EQ(chunk.session_id(), 3u);
  ASSERT_TRUE(chunk.remaining_bytes().has_value());
  EXPECT_EQ(chunk.remaining_bytes().value(), 0u);

  EXPECT_EQ(transfer_status, Status::Unknown());

  // Send the final status chunk to complete the transfer.
  context_.server().SendServerStream<Transfer::Write>(
      EncodeChunk(Chunk::Final(ProtocolVersion::kLegacy, 3, OkStatus())));
  transfer_thread_.WaitUntilEventIsProcessed();

  EXPECT_EQ(payloads.size(), 3u);
  EXPECT_EQ(transfer_status, OkStatus());
}

TEST_F(WriteTransfer, Version2_RetryDuringHandshake) {
  stream::MemoryReader reader(kData32);
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Write(
                3,
                reader,
                [&transfer_status](Status status) { transfer_status = status; },
                cfg::kDefaultChunkTimeout,
                ProtocolVersion::kVersionTwo));
  transfer_thread_.WaitUntilEventIsProcessed();

  // The client begins by sending the ID of the resource to transfer.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Write>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk chunk = DecodeChunk(payloads.back());
  EXPECT_EQ(chunk.type(), Chunk::Type::kStart);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  EXPECT_EQ(chunk.session_id(), 3u);
  EXPECT_EQ(chunk.resource_id(), 3u);

  // Time out waiting for a server response. The client should resend the
  // initial packet.
  transfer_thread_.SimulateClientTimeout(3);
  ASSERT_EQ(payloads.size(), 2u);

  chunk = DecodeChunk(payloads.back());
  EXPECT_EQ(chunk.type(), Chunk::Type::kStart);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  EXPECT_EQ(chunk.session_id(), 3u);
  EXPECT_EQ(chunk.resource_id(), 3u);

  // This time, respond with the correct continuation packet. The transfer
  // should resume and complete normally.
  context_.server().SendServerStream<Transfer::Write>(
      EncodeChunk(Chunk(ProtocolVersion::kVersionTwo, Chunk::Type::kStartAck)
                      .set_session_id(31)
                      .set_resource_id(3)));
  transfer_thread_.WaitUntilEventIsProcessed();

  ASSERT_EQ(payloads.size(), 3u);

  chunk = DecodeChunk(payloads.back());
  EXPECT_EQ(chunk.type(), Chunk::Type::kStartAckConfirmation);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  EXPECT_EQ(chunk.session_id(), 31u);
  EXPECT_FALSE(chunk.resource_id().has_value());

  rpc::test::WaitForPackets(context_.output(), 2, [this] {
    context_.server().SendServerStream<Transfer::Write>(EncodeChunk(
        Chunk(ProtocolVersion::kVersionTwo, Chunk::Type::kParametersRetransmit)
            .set_session_id(31)
            .set_offset(0)
            .set_window_end_offset(64)
            .set_max_chunk_size_bytes(32)));
  });

  ASSERT_EQ(payloads.size(), 5u);

  chunk = DecodeChunk(payloads[3]);
  EXPECT_EQ(chunk.type(), Chunk::Type::kData);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  EXPECT_EQ(chunk.session_id(), 31u);
  EXPECT_EQ(chunk.offset(), 0u);
  EXPECT_TRUE(chunk.has_payload());
  EXPECT_EQ(std::memcmp(
                chunk.payload().data(), kData32.data(), chunk.payload().size()),
            0);

  chunk = DecodeChunk(payloads[4]);
  EXPECT_EQ(chunk.type(), Chunk::Type::kData);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  EXPECT_EQ(chunk.session_id(), 31u);
  ASSERT_TRUE(chunk.remaining_bytes().has_value());
  EXPECT_EQ(chunk.remaining_bytes().value(), 0u);

  EXPECT_EQ(transfer_status, Status::Unknown());

  context_.server().SendServerStream<Transfer::Write>(
      EncodeChunk(Chunk::Final(ProtocolVersion::kVersionTwo, 31, OkStatus())));
  transfer_thread_.WaitUntilEventIsProcessed();

  // Client should acknowledge the completion of the transfer.
  EXPECT_EQ(payloads.size(), 6u);

  chunk = DecodeChunk(payloads[5]);
  EXPECT_EQ(chunk.type(), Chunk::Type::kCompletionAck);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  EXPECT_EQ(chunk.session_id(), 31u);

  EXPECT_EQ(transfer_status, OkStatus());
}

TEST_F(WriteTransfer, Version2_RetryAfterHandshake) {
  stream::MemoryReader reader(kData32);
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Write(
                3,
                reader,
                [&transfer_status](Status status) { transfer_status = status; },
                cfg::kDefaultChunkTimeout,
                ProtocolVersion::kVersionTwo));
  transfer_thread_.WaitUntilEventIsProcessed();

  // The client begins by sending the ID of the resource to transfer.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Write>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk chunk = DecodeChunk(payloads.back());
  EXPECT_EQ(chunk.type(), Chunk::Type::kStart);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  EXPECT_EQ(chunk.session_id(), 3u);
  EXPECT_EQ(chunk.resource_id(), 3u);

  // The server responds with a START_ACK, continuing the version 2 handshake
  // and assigning a session_id to the transfer.
  context_.server().SendServerStream<Transfer::Write>(
      EncodeChunk(Chunk(ProtocolVersion::kVersionTwo, Chunk::Type::kStartAck)
                      .set_session_id(33)
                      .set_resource_id(3)));
  transfer_thread_.WaitUntilEventIsProcessed();

  ASSERT_EQ(payloads.size(), 2u);

  // Client should accept the session_id with a START_ACK_CONFIRMATION.
  chunk = DecodeChunk(payloads.back());
  EXPECT_EQ(chunk.type(), Chunk::Type::kStartAckConfirmation);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  EXPECT_EQ(chunk.session_id(), 33u);
  EXPECT_FALSE(chunk.resource_id().has_value());

  // Time out waiting for a server response. The client should resend the
  // initial packet.
  transfer_thread_.SimulateClientTimeout(33);
  ASSERT_EQ(payloads.size(), 3u);

  chunk = DecodeChunk(payloads.back());
  EXPECT_EQ(chunk.type(), Chunk::Type::kStartAckConfirmation);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  EXPECT_EQ(chunk.session_id(), 33u);
  EXPECT_FALSE(chunk.resource_id().has_value());

  // This time, respond with the first transfer parameters chunk. The transfer
  // should resume and complete normally.
  rpc::test::WaitForPackets(context_.output(), 2, [this] {
    context_.server().SendServerStream<Transfer::Write>(EncodeChunk(
        Chunk(ProtocolVersion::kVersionTwo, Chunk::Type::kParametersRetransmit)
            .set_session_id(33)
            .set_offset(0)
            .set_window_end_offset(64)
            .set_max_chunk_size_bytes(32)));
  });

  ASSERT_EQ(payloads.size(), 5u);

  chunk = DecodeChunk(payloads[3]);
  EXPECT_EQ(chunk.type(), Chunk::Type::kData);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  EXPECT_EQ(chunk.session_id(), 33u);
  EXPECT_EQ(chunk.offset(), 0u);
  EXPECT_TRUE(chunk.has_payload());
  EXPECT_EQ(std::memcmp(
                chunk.payload().data(), kData32.data(), chunk.payload().size()),
            0);

  chunk = DecodeChunk(payloads[4]);
  EXPECT_EQ(chunk.type(), Chunk::Type::kData);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  EXPECT_EQ(chunk.session_id(), 33u);
  ASSERT_TRUE(chunk.remaining_bytes().has_value());
  EXPECT_EQ(chunk.remaining_bytes().value(), 0u);

  EXPECT_EQ(transfer_status, Status::Unknown());

  context_.server().SendServerStream<Transfer::Write>(
      EncodeChunk(Chunk::Final(ProtocolVersion::kVersionTwo, 33, OkStatus())));
  transfer_thread_.WaitUntilEventIsProcessed();

  // Client should acknowledge the completion of the transfer.
  EXPECT_EQ(payloads.size(), 6u);

  chunk = DecodeChunk(payloads[5]);
  EXPECT_EQ(chunk.type(), Chunk::Type::kCompletionAck);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  EXPECT_EQ(chunk.session_id(), 33u);

  EXPECT_EQ(transfer_status, OkStatus());
}

TEST_F(WriteTransfer, Version2_ServerErrorDuringHandshake) {
  stream::MemoryReader reader(kData32);
  Status transfer_status = Status::Unknown();

  ASSERT_EQ(OkStatus(),
            client_.Write(
                3,
                reader,
                [&transfer_status](Status status) { transfer_status = status; },
                cfg::kDefaultChunkTimeout,
                ProtocolVersion::kVersionTwo));
  transfer_thread_.WaitUntilEventIsProcessed();

  // The client begins by sending the ID of the resource to transfer.
  rpc::PayloadsView payloads =
      context_.output().payloads<Transfer::Write>(context_.channel().id());
  ASSERT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::Unknown());

  Chunk chunk = DecodeChunk(payloads.back());
  EXPECT_EQ(chunk.type(), Chunk::Type::kStart);
  EXPECT_EQ(chunk.protocol_version(), ProtocolVersion::kVersionTwo);
  EXPECT_EQ(chunk.session_id(), 3u);
  EXPECT_EQ(chunk.resource_id(), 3u);

  // The server responds to the start request with an error.
  context_.server().SendServerStream<Transfer::Write>(EncodeChunk(
      Chunk::Final(ProtocolVersion::kVersionTwo, 3, Status::NotFound())));
  transfer_thread_.WaitUntilEventIsProcessed();

  EXPECT_EQ(payloads.size(), 1u);
  EXPECT_EQ(transfer_status, Status::NotFound());
}

}  // namespace
}  // namespace pw::transfer::test
