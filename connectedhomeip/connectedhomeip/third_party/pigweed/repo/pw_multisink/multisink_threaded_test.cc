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

#include <cstddef>
#include <cstdint>

#include "gtest/gtest.h"
#include "pw_containers/vector.h"
#include "pw_multisink/multisink.h"
#include "pw_multisink/test_thread.h"
#include "pw_span/span.h"
#include "pw_string/string_builder.h"
#include "pw_thread/thread.h"
#include "pw_thread/yield.h"

namespace pw::multisink {
namespace {
constexpr size_t kEntryBufferSize = sizeof("message 000");
constexpr size_t kMaxMessageCount = 250;
constexpr size_t kBufferSize = kMaxMessageCount * kEntryBufferSize;

using MessageSpan = span<const StringBuffer<kEntryBufferSize>>;

void CompareSentAndReceivedMessages(const MessageSpan& sent_messages,
                                    const MessageSpan& received_messages) {
  ASSERT_EQ(sent_messages.size(), received_messages.size());
  for (size_t i = 0; i < sent_messages.size(); ++i) {
    ASSERT_EQ(sent_messages[i].size(), received_messages[i].size());
    EXPECT_EQ(std::string_view(sent_messages[i]),
              std::string_view(received_messages[i]));
  }
}
}  // namespace

// Static message pool to avoid recreating messages for every test and avoids
// using std::string.
class MessagePool {
 public:
  static MessagePool& Instance() {
    static MessagePool instance;
    return instance;
  }

  MessagePool(const MessagePool&) = delete;
  MessagePool& operator=(const MessagePool&) = delete;
  MessagePool(MessagePool&&) = delete;
  MessagePool& operator=(MessagePool&&) = delete;

  MessageSpan GetMessages(size_t message_count) const {
    PW_ASSERT(message_count <= messages_.size());
    return MessageSpan(messages_.begin(), message_count);
  }

 private:
  MessagePool() {
    for (size_t i = 0; i < kMaxMessageCount; ++i) {
      messages_.emplace_back();
      messages_.back() << "message %u" << static_cast<unsigned int>(i);
    }
  }

  Vector<StringBuffer<kEntryBufferSize>, kMaxMessageCount> messages_;
};

// Continuously reads logs from a multisink, using PopEntry() and stores copies
// of the retrieved messages for later verification. The thread stops when the
// the number of read messages and total drop count matches the expected count.
class LogPopReaderThread : public thread::ThreadCore {
 public:
  LogPopReaderThread(MultiSink& multisink,
                     uint32_t expected_message_and_drop_count)
      : multisink_(multisink),
        total_drop_count_(0),
        expected_message_and_drop_count_(expected_message_and_drop_count) {
    PW_ASSERT(expected_message_and_drop_count_ <= kMaxMessageCount);
  }

  uint32_t drop_count() { return total_drop_count_; }

  const MessageSpan received_messages() {
    return MessageSpan(received_messages_.begin(), received_messages_.size());
  }

  void Run() override {
    multisink_.AttachDrain(drain_);
    ReadAllEntries();
  }

  virtual void ReadAllEntries() {
    do {
      uint32_t drop_count = 0;
      uint32_t ingress_drop_count = 0;
      const Result<ConstByteSpan> possible_entry =
          drain_.PopEntry(entry_buffer_, drop_count, ingress_drop_count);
      total_drop_count_ += drop_count + ingress_drop_count;
      if (possible_entry.status().IsOutOfRange()) {
        pw::this_thread::yield();
        continue;
      }
      ASSERT_EQ(possible_entry.status(), OkStatus());
      if (received_messages_.full()) {
        return;
      }
      received_messages_.emplace_back();
      received_messages_.back() << std::string_view(
          reinterpret_cast<const char*>(possible_entry.value().data()),
          possible_entry.value().size());
      pw::this_thread::yield();
    } while (total_drop_count_ + received_messages_.size() <
             expected_message_and_drop_count_);
  }

 protected:
  MultiSink::Drain drain_;
  MultiSink& multisink_;
  std::array<std::byte, kEntryBufferSize> entry_buffer_;
  uint32_t total_drop_count_;
  const uint32_t expected_message_and_drop_count_;
  Vector<StringBuffer<kEntryBufferSize>, kMaxMessageCount> received_messages_;
};

class LogPeekAndCommitReaderThread : public LogPopReaderThread {
 public:
  LogPeekAndCommitReaderThread(MultiSink& multisink,
                               uint32_t expected_message_and_drop_count)
      : LogPopReaderThread(multisink, expected_message_and_drop_count) {}

  void ReadAllEntries() override {
    do {
      uint32_t drop_count = 0;
      uint32_t ingress_drop_count = 0;
      const Result<MultiSink::Drain::PeekedEntry> possible_entry =
          drain_.PeekEntry(entry_buffer_, drop_count, ingress_drop_count);
      total_drop_count_ += drop_count + ingress_drop_count;
      if (possible_entry.status().IsOutOfRange()) {
        pw::this_thread::yield();
        continue;
      }
      ASSERT_EQ(possible_entry.status(), OkStatus());
      if (received_messages_.full()) {
        return;
      }
      pw::this_thread::yield();
      received_messages_.emplace_back();
      received_messages_.back() << std::string_view(
          reinterpret_cast<const char*>(possible_entry.value().entry().data()),
          possible_entry.value().entry().size());
      ASSERT_EQ(drain_.PopEntry(possible_entry.value()), OkStatus());
      pw::this_thread::yield();
    } while (total_drop_count_ + received_messages_.size() <
             expected_message_and_drop_count_);
  }
};

// Adds the provided messages to the shared multisink.
class LogWriterThread : public thread::ThreadCore {
 public:
  LogWriterThread(MultiSink& multisink, const MessageSpan& message_stack)
      : multisink_(multisink), message_stack_(message_stack) {}

  void Run() override {
    for (const auto& message : message_stack_) {
      multisink_.HandleEntry(as_bytes(span(std::string_view(message))));
      pw::this_thread::yield();
    }
  }

 private:
  MultiSink& multisink_;
  const MessageSpan& message_stack_;
};

class MultiSinkTest : public ::testing::Test {
 protected:
  MultiSinkTest() : multisink_(buffer_) {}

  std::byte buffer_[kBufferSize];
  MultiSink multisink_;

 private:
};

TEST_F(MultiSinkTest, SingleWriterSingleReader) {
  const uint32_t log_count = 100;
  const uint32_t drop_count = 5;
  const uint32_t expected_message_and_drop_count = log_count + drop_count;
  const auto message_stack = MessagePool::Instance().GetMessages(log_count);

  // Start reader thread.
  LogPopReaderThread reader_thread_core(multisink_,
                                        expected_message_and_drop_count);
  thread::Thread reader_thread(test::MultiSinkTestThreadOptions(),
                               reader_thread_core);
  // Start writer thread.
  LogWriterThread writer_thread_core(multisink_, message_stack);
  thread::Thread writer_thread(test::MultiSinkTestThreadOptions(),
                               writer_thread_core);

  // Wait for writer thread to end.
  writer_thread.join();
  multisink_.HandleDropped(drop_count);
  reader_thread.join();

  EXPECT_EQ(reader_thread_core.drop_count(), drop_count);
  CompareSentAndReceivedMessages(message_stack,
                                 reader_thread_core.received_messages());
}

TEST_F(MultiSinkTest, SingleWriterSinglePeekAndCommitReader) {
  const uint32_t log_count = 100;
  const uint32_t drop_count = 5;
  const uint32_t expected_message_and_drop_count = log_count + drop_count;
  const auto message_stack = MessagePool::Instance().GetMessages(log_count);

  // Start reader thread.
  LogPeekAndCommitReaderThread reader_thread_core(
      multisink_, expected_message_and_drop_count);
  thread::Thread reader_thread(test::MultiSinkTestThreadOptions(),
                               reader_thread_core);
  // Start writer thread.
  LogWriterThread writer_thread_core(multisink_, message_stack);
  thread::Thread writer_thread(test::MultiSinkTestThreadOptions(),
                               writer_thread_core);

  // Wait for writer thread to end.
  writer_thread.join();
  multisink_.HandleDropped(drop_count);
  reader_thread.join();

  EXPECT_EQ(reader_thread_core.drop_count(), drop_count);
  CompareSentAndReceivedMessages(message_stack,
                                 reader_thread_core.received_messages());
}

TEST_F(MultiSinkTest, SingleWriterMultipleReaders) {
  const uint32_t log_count = 100;
  const uint32_t drop_count = 5;
  const uint32_t expected_message_and_drop_count = log_count + drop_count;
  const auto message_stack = MessagePool::Instance().GetMessages(log_count);

  // Start reader threads.
  LogPopReaderThread reader_thread_core1(multisink_,
                                         expected_message_and_drop_count);
  thread::Thread reader_thread1(test::MultiSinkTestThreadOptions(),
                                reader_thread_core1);
  LogPopReaderThread reader_thread_core2(multisink_,
                                         expected_message_and_drop_count);
  thread::Thread reader_thread2(test::MultiSinkTestThreadOptions(),
                                reader_thread_core2);
  LogPeekAndCommitReaderThread reader_thread_core3(
      multisink_, expected_message_and_drop_count);
  thread::Thread reader_thread3(test::MultiSinkTestThreadOptions(),
                                reader_thread_core3);
  // Start writer thread.
  LogWriterThread writer_thread_core(multisink_, message_stack);
  thread::Thread writer_thread(test::MultiSinkTestThreadOptions(),
                               writer_thread_core);

  // Wait for writer thread to end.
  writer_thread.join();
  multisink_.HandleDropped(drop_count);
  reader_thread1.join();
  reader_thread2.join();
  reader_thread3.join();

  EXPECT_EQ(reader_thread_core1.drop_count(), drop_count);
  CompareSentAndReceivedMessages(message_stack,
                                 reader_thread_core1.received_messages());
  EXPECT_EQ(reader_thread_core2.drop_count(), drop_count);
  CompareSentAndReceivedMessages(message_stack,
                                 reader_thread_core2.received_messages());
  EXPECT_EQ(reader_thread_core3.drop_count(), drop_count);
  CompareSentAndReceivedMessages(message_stack,
                                 reader_thread_core3.received_messages());
}

TEST_F(MultiSinkTest, MultipleWritersMultipleReaders) {
  const uint32_t log_count = 100;
  const uint32_t drop_count = 7;
  const uint32_t expected_message_and_drop_count = 2 * log_count + drop_count;
  const auto message_stack = MessagePool::Instance().GetMessages(log_count);

  // Start reader threads.
  LogPopReaderThread reader_thread_core1(multisink_,
                                         expected_message_and_drop_count);
  thread::Thread reader_thread1(test::MultiSinkTestThreadOptions(),
                                reader_thread_core1);
  LogPopReaderThread reader_thread_core2(multisink_,
                                         expected_message_and_drop_count);
  thread::Thread reader_thread2(test::MultiSinkTestThreadOptions(),
                                reader_thread_core2);
  LogPeekAndCommitReaderThread reader_thread_core3(
      multisink_, expected_message_and_drop_count);
  thread::Thread reader_thread3(test::MultiSinkTestThreadOptions(),
                                reader_thread_core3);
  // Start writer threads.
  LogWriterThread writer_thread_core1(multisink_, message_stack);
  thread::Thread writer_thread1(test::MultiSinkTestThreadOptions(),
                                writer_thread_core1);
  LogWriterThread writer_thread_core2(multisink_, message_stack);
  thread::Thread writer_thread2(test::MultiSinkTestThreadOptions(),
                                writer_thread_core2);

  // Wait for writer thread to end.
  writer_thread1.join();
  writer_thread2.join();
  multisink_.HandleDropped(drop_count);
  reader_thread1.join();
  reader_thread2.join();
  reader_thread3.join();

  EXPECT_EQ(reader_thread_core1.drop_count(), drop_count);
  EXPECT_EQ(reader_thread_core2.drop_count(), drop_count);
  EXPECT_EQ(reader_thread_core3.drop_count(), drop_count);
  // Since we don't know the order that messages came in, we can't check them.
  EXPECT_EQ(reader_thread_core1.received_messages().size(),
            expected_message_and_drop_count - drop_count);
  EXPECT_EQ(reader_thread_core2.received_messages().size(),
            expected_message_and_drop_count - drop_count);
  EXPECT_EQ(reader_thread_core3.received_messages().size(),
            expected_message_and_drop_count - drop_count);
}

TEST_F(MultiSinkTest, OverflowMultisink) {
  // Expect the multisink to overflow and readers to not fail when poping, or
  // peeking and commiting entries.
  const size_t log_count = kMaxMessageCount;
  const size_t max_buffer_entry_count = 20;
  std::byte small_multisink_buffer[max_buffer_entry_count * kEntryBufferSize];
  MultiSink small_multisink(small_multisink_buffer);

  const auto message_stack = MessagePool::Instance().GetMessages(log_count);

  // Start reader threads.
  LogPeekAndCommitReaderThread reader_thread_core1(small_multisink, log_count);
  thread::Thread reader_thread1(test::MultiSinkTestThreadOptions(),
                                reader_thread_core1);
  LogPopReaderThread reader_thread_core2(small_multisink, log_count);
  thread::Thread reader_thread2(test::MultiSinkTestThreadOptions(),
                                reader_thread_core2);

  // Start writer threads.
  LogWriterThread writer_thread_core1(small_multisink, message_stack);
  thread::Thread writer_thread1(test::MultiSinkTestThreadOptions(),
                                writer_thread_core1);
  LogWriterThread writer_thread_core2(small_multisink, message_stack);
  thread::Thread writer_thread2(test::MultiSinkTestThreadOptions(),
                                writer_thread_core2);

  // Wait for writer thread to end.
  writer_thread1.join();
  writer_thread2.join();
  reader_thread1.join();
  reader_thread2.join();

  // Verifying received messages and drop message counts is unreliable as we
  // can't control the order threads will operate.
}

}  // namespace pw::multisink
