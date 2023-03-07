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

#include "pw_thread/thread_snapshot_service.h"

#include "gtest/gtest.h"
#include "pw_protobuf/decoder.h"
#include "pw_rpc/raw/server_reader_writer.h"
#include "pw_span/span.h"
#include "pw_thread/thread_info.h"
#include "pw_thread/thread_iteration.h"
#include "pw_thread_private/thread_snapshot_service.h"
#include "pw_thread_protos/thread.pwpb.h"
#include "pw_thread_protos/thread_snapshot_service.pwpb.h"

namespace pw::thread::proto {
namespace {

// Iterates through each proto encoded thread in the buffer.
bool EncodedThreadExists(ConstByteSpan serialized_thread_buffer,
                         ConstByteSpan thread_name) {
  protobuf::Decoder decoder(serialized_thread_buffer);
  while (decoder.Next().ok()) {
    switch (decoder.FieldNumber()) {
      case static_cast<uint32_t>(proto::SnapshotThreadInfo::Fields::THREADS): {
        ConstByteSpan thread_buffer;
        EXPECT_EQ(OkStatus(), decoder.ReadBytes(&thread_buffer));
        ConstByteSpan encoded_name;
        EXPECT_EQ(OkStatus(), DecodeThreadName(thread_buffer, encoded_name));
        if (encoded_name.size() == thread_name.size()) {
          if (std::equal(thread_name.begin(),
                         thread_name.end(),
                         encoded_name.begin())) {
            return true;
          }
        }
      }
    }
  }
  return false;
}

ThreadInfo CreateThreadInfoObject(std::optional<ConstByteSpan> name,
                                  std::optional<uintptr_t> low_addr,
                                  std::optional<uintptr_t> high_addr,
                                  std::optional<uintptr_t> peak_addr) {
  ThreadInfo thread_info;

  if (name.has_value()) {
    thread_info.set_thread_name(name.value());
  }
  if (low_addr.has_value()) {
    thread_info.set_stack_low_addr(low_addr.value());
  }
  if (high_addr.has_value()) {
    thread_info.set_stack_high_addr(high_addr.value());
  }
  if (peak_addr.has_value()) {
    thread_info.set_stack_peak_addr(peak_addr.value());
  }

  return thread_info;
}

// Test creates a custom thread info object and proto encodes. Checks that the
// custom object is encoded properly.
TEST(ThreadSnapshotService, DecodeSingleThreadInfoObject) {
  std::array<std::byte, RequiredServiceBufferSize(1)> encode_buffer;

  proto::SnapshotThreadInfo::MemoryEncoder encoder(encode_buffer);

  ConstByteSpan name = bytes::String("MyThread\0");
  ThreadInfo thread_info = CreateThreadInfoObject(
      std::make_optional(name), /* thread name */
      std::make_optional(
          static_cast<uintptr_t>(12345678u)) /* stack low address */,
      std::make_optional(static_cast<uintptr_t>(0u)) /* stack high address */,
      std::make_optional(
          static_cast<uintptr_t>(987654321u)) /* stack peak address */);

  EXPECT_EQ(OkStatus(), ProtoEncodeThreadInfo(encoder, thread_info));

  ConstByteSpan response_span(encoder);
  EXPECT_TRUE(
      EncodedThreadExists(response_span, thread_info.thread_name().value()));
}

TEST(ThreadSnapshotService, DecodeMultipleThreadInfoObjects) {
  std::array<std::byte, RequiredServiceBufferSize(3)> encode_buffer;

  proto::SnapshotThreadInfo::MemoryEncoder encoder(encode_buffer);

  ConstByteSpan name = bytes::String("MyThread1\0");
  ThreadInfo thread_info_1 =
      CreateThreadInfoObject(std::make_optional(name),
                             std::make_optional(static_cast<uintptr_t>(123u)),
                             std::make_optional(static_cast<uintptr_t>(1023u)),
                             std::make_optional(static_cast<uintptr_t>(321u)));

  name = bytes::String("MyThread2\0");
  ThreadInfo thread_info_2 = CreateThreadInfoObject(
      std::make_optional(name),
      std::make_optional(static_cast<uintptr_t>(1000u)),
      std::make_optional(static_cast<uintptr_t>(999999u)),
      std::make_optional(static_cast<uintptr_t>(0u)));

  name = bytes::String("MyThread3\0");
  ThreadInfo thread_info_3 =
      CreateThreadInfoObject(std::make_optional(name),
                             std::make_optional(static_cast<uintptr_t>(123u)),
                             std::make_optional(static_cast<uintptr_t>(1023u)),
                             std::make_optional(static_cast<uintptr_t>(321u)));

  // Encode out of order.
  EXPECT_EQ(OkStatus(), ProtoEncodeThreadInfo(encoder, thread_info_3));
  EXPECT_EQ(OkStatus(), ProtoEncodeThreadInfo(encoder, thread_info_1));
  EXPECT_EQ(OkStatus(), ProtoEncodeThreadInfo(encoder, thread_info_2));

  ConstByteSpan response_span(encoder);
  EXPECT_TRUE(
      EncodedThreadExists(response_span, thread_info_1.thread_name().value()));
  EXPECT_TRUE(
      EncodedThreadExists(response_span, thread_info_2.thread_name().value()));
  EXPECT_TRUE(
      EncodedThreadExists(response_span, thread_info_3.thread_name().value()));
}

TEST(ThreadSnapshotService, DefaultBufferSize) {
  static std::array<std::byte, RequiredServiceBufferSize()> encode_buffer;

  proto::SnapshotThreadInfo::MemoryEncoder encoder(encode_buffer);

  ConstByteSpan name = bytes::String("MyThread\0");
  std::optional<uintptr_t> example_addr =
      std::make_optional(std::numeric_limits<uintptr_t>::max());

  ThreadInfo thread_info = CreateThreadInfoObject(
      std::make_optional(name), example_addr, example_addr, example_addr);

  for (int i = 0; i < PW_THREAD_MAXIMUM_THREADS; i++) {
    EXPECT_EQ(OkStatus(), ProtoEncodeThreadInfo(encoder, thread_info));
  }

  ConstByteSpan response_span(encoder);
  EXPECT_TRUE(
      EncodedThreadExists(response_span, thread_info.thread_name().value()));
}

TEST(ThreadSnapshotService, FailedPrecondition) {
  static std::array<std::byte, RequiredServiceBufferSize(1)> encode_buffer;

  proto::SnapshotThreadInfo::MemoryEncoder encoder(encode_buffer);

  ThreadInfo thread_info_no_name = CreateThreadInfoObject(
      std::nullopt,
      std::make_optional(static_cast<uintptr_t>(1111111111u)),
      std::make_optional(static_cast<uintptr_t>(2222222222u)),
      std::make_optional(static_cast<uintptr_t>(3333333333u)));
  Status status = ProtoEncodeThreadInfo(encoder, thread_info_no_name);
  EXPECT_EQ(status, Status::FailedPrecondition());
  // Expected log: "Thread missing information needed by service."
  ErrorLog(status);

  // Same error log as above.
  ConstByteSpan name = bytes::String("MyThread\0");
  ThreadInfo thread_info_no_high_addr = CreateThreadInfoObject(
      std::make_optional(name),
      std::make_optional(static_cast<uintptr_t>(1111111111u)),
      std::nullopt,
      std::make_optional(static_cast<uintptr_t>(3333333333u)));
  EXPECT_EQ(ProtoEncodeThreadInfo(encoder, thread_info_no_high_addr),
            Status::FailedPrecondition());
}

TEST(ThreadSnapshotService, Unimplemented) {
  static std::array<std::byte, RequiredServiceBufferSize(1)> encode_buffer;

  proto::SnapshotThreadInfo::MemoryEncoder encoder(encode_buffer);

  ConstByteSpan name = bytes::String("MyThread\0");
  ThreadInfo thread_info_no_peak_addr =
      CreateThreadInfoObject(std::make_optional(name),
                             std::make_optional(static_cast<uintptr_t>(0u)),
                             std::make_optional(static_cast<uintptr_t>(0u)),
                             std::nullopt);

  Status status = ProtoEncodeThreadInfo(encoder, thread_info_no_peak_addr);
  EXPECT_EQ(status, Status::Unimplemented());
  // Expected log: "Peak stack usage reporting not supported by your current OS
  // or configuration."
  ErrorLog(status);
}

}  // namespace
}  // namespace pw::thread::proto
