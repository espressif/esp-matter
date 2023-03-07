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
#pragma once

#include <limits>
#include <mutex>

#include "pw_bytes/span.h"
#include "pw_function/function.h"
#include "pw_multisink/config.h"
#include "pw_result/result.h"
#include "pw_ring_buffer/prefixed_entry_ring_buffer.h"
#include "pw_status/status.h"
#include "pw_sync/lock_annotations.h"

namespace pw {
namespace multisink {

// An asynchronous single-writer multi-reader queue that ensures readers can
// poll for dropped message counts, which is useful for logging or similar
// scenarios where readers need to be aware of the input message sequence.
//
// This class is thread-safe but NOT IRQ-safe when
// PW_MULTISINK_LOCK_INTERRUPT_SAFE is disabled.
class MultiSink {
 public:
  // An asynchronous reader which is attached to a MultiSink via AttachDrain.
  // Each Drain holds a PrefixedEntryRingBufferMulti::Reader and abstracts away
  // entry sequence information for clients when popping.
  class Drain {
   public:
    // Holds the context for a peeked entry, tha the user may pass to `PopEntry`
    // to advance the drain.
    class PeekedEntry {
     public:
      // Provides access to the peeked entry's data.
      ConstByteSpan entry() const { return entry_; }

     private:
      friend MultiSink;
      friend MultiSink::Drain;

      constexpr PeekedEntry(ConstByteSpan entry, uint32_t sequence_id)
          : entry_(entry), sequence_id_(sequence_id) {}

      uint32_t sequence_id() const { return sequence_id_; }

      const ConstByteSpan entry_;
      const uint32_t sequence_id_;
    };

    constexpr Drain()
        : last_handled_sequence_id_(0),
          last_peek_sequence_id_(0),
          last_handled_ingress_drop_count_(0),
          multisink_(nullptr) {}

    // Returns the next available entry if it exists and acquires the latest
    // drop count in parallel.
    //
    // If the read operation was successful or returned OutOfRange (i.e. no
    // entries to read) then the `drain_drop_count_out` is set to the number of
    // entries that were dropped since the last call to PopEntry due to
    // advancing the drain, and `ingress_drop_count_out` is set to the number of
    // logs that were dropped before being added to the MultiSink. Otherwise,
    // the drop counts are set to zero, so should always be processed.
    //
    // Drop counts are internally maintained with a 32-bit counter. If
    // UINT32_MAX entries have been handled by the attached multisink between
    // subsequent calls to PopEntry, the drop count will overflow and will
    // report a lower count erroneously. Users should ensure that sinks call
    // PopEntry at least once every UINT32_MAX entries.
    //
    // Example Usage:
    //
    // void ProcessEntriesFromDrain(Drain& drain) {
    //   std::array<std::byte, kEntryBufferSize> buffer;
    //   uint32_t drop_count = 0;
    //
    //   // Example#1: Request the drain for a new entry.
    //   {
    //     const Result<ConstByteSpan> result = drain.PopEntry(buffer,
    //                                                         drop_count);
    //
    //     // If a non-zero drop count is received, process them.
    //     if (drop_count > 0) {
    //       ProcessDropCount(drop_count);
    //     }
    //
    //     // If the call was successful, process the entry that was received.
    //     if (result.ok()) {
    //       ProcessEntry(result.value());
    //     }
    //   }
    //
    //   // Example#2: Drain out all messages.
    //   {
    //     Result<ConstByteSpan> result = Status::OutOfRange();
    //     do {
    //       result = drain.PopEntry(buffer, drop_count);
    //
    //       if (drop_count > 0) {
    //         ProcessDropCount(drop_count);
    //       }
    //
    //       if (result.ok()) {
    //         ProcessEntry(result.value());
    //       }
    //
    //       // Keep trying until we hit OutOfRange. Note that a new entry may
    //       // have arrived after the PopEntry call.
    //     } while (!result.IsOutOfRange());
    //   }
    // }
    // Precondition: the buffer data must not be corrupt, otherwise there will
    // be a crash.
    //
    // Return values:
    // OK - An entry was successfully read from the multisink.
    // OUT_OF_RANGE - No entries were available.
    // FAILED_PRECONDITION - The drain must be attached to a sink.
    // RESOURCE_EXHAUSTED - The provided buffer was not large enough to store
    // the next available entry, which was discarded.
    Result<ConstByteSpan> PopEntry(ByteSpan buffer,
                                   uint32_t& drain_drop_count_out,
                                   uint32_t& ingress_drop_count_out)
        PW_LOCKS_EXCLUDED(multisink_->lock_);
    // Overload that combines drop counts.
    // TODO(cachinchilla): remove when downstream projects migrated to new API.
    [[deprecated("Use PopEntry with different drop count outputs")]] Result<
        ConstByteSpan>
    PopEntry(ByteSpan buffer, uint32_t& drop_count_out)
        PW_LOCKS_EXCLUDED(multisink_->lock_) {
      uint32_t ingress_drop_count = 0;
      Result<ConstByteSpan> result =
          PopEntry(buffer, drop_count_out, ingress_drop_count);
      drop_count_out += ingress_drop_count;
      return result;
    }

    // Removes the previously peeked entry from the multisink.
    //
    // Example Usage:
    //
    //  // Peek entry to send it, and remove entry from multisink on success.
    //  uint32_t drop_count;
    //  const Result<PeekedEntry> peek_result =
    //      PeekEntry(out_buffer, drop_count);
    //  if (!peek_result.ok()) {
    //    return peek_result.status();
    //  }
    //  Status send_status = UserSendFunction(peek_result.value().entry())
    //  if (!send_status.ok())
    //    return send_status;
    //  }
    //  PW_CHECK_OK(PopEntry(peek_result.value());
    //
    // Precondition: the buffer data must not be corrupt, otherwise there will
    // be a crash.
    //
    // Return values:
    // OK - the entry or entries were removed from the multisink succesfully.
    // FAILED_PRECONDITION - The drain must be attached to a sink.
    Status PopEntry(const PeekedEntry& entry)
        PW_LOCKS_EXCLUDED(multisink_->lock_);

    // Returns a copy of the next available entry if it exists and acquires the
    // latest drop count if the drain was advanced, and the latest ingress drop
    // count, without moving the drain forward, except if there is a
    // RESOURCE_EXHAUSTED error when peeking, in which case the drain is
    // automatically advanced.
    // The `drain_drop_count_out` follows the same logic as `PopEntry`. The user
    // must call `PopEntry` once the data in peek was used successfully.
    //
    // Precondition: the buffer data must not be corrupt, otherwise there will
    // be a crash.
    //
    // Return values:
    // OK - An entry was successfully read from the multisink.
    // OUT_OF_RANGE - No entries were available.
    // FAILED_PRECONDITION - The drain must be attached to a sink.
    // RESOURCE_EXHAUSTED - The provided buffer was not large enough to store
    // the next available entry, which was discarded.
    Result<PeekedEntry> PeekEntry(ByteSpan buffer,
                                  uint32_t& drain_drop_count_out,
                                  uint32_t& ingress_drop_count_out)
        PW_LOCKS_EXCLUDED(multisink_->lock_);

    // Drains are not copyable or movable.
    Drain(const Drain&) = delete;
    Drain& operator=(const Drain&) = delete;
    Drain(Drain&&) = delete;
    Drain& operator=(Drain&&) = delete;

   protected:
    friend MultiSink;

    // The `reader_` and `last_handled_sequence_id_` are managed by attached
    // multisink and are guarded by `multisink_->lock_` when used.
    ring_buffer::PrefixedEntryRingBufferMulti::Reader reader_;
    uint32_t last_handled_sequence_id_;
    uint32_t last_peek_sequence_id_;
    uint32_t last_handled_ingress_drop_count_;
    MultiSink* multisink_;
  };

  // A pure-virtual listener of a MultiSink, attached via AttachListener.
  // MultiSink's invoke listeners when new data arrives, allowing them to
  // schedule the draining of messages out of the MultiSink.
  class Listener : public IntrusiveList<Listener>::Item {
   public:
    constexpr Listener() {}
    virtual ~Listener() = default;

    // Listeners are not copyable or movable.
    Listener(const Listener&) = delete;
    Listener& operator=(const Drain&) = delete;
    Listener(Listener&&) = delete;
    Listener& operator=(Drain&&) = delete;

   protected:
    friend MultiSink;

    // Invoked by the attached multisink when a new entry or drop count is
    // available. The multisink lock is held during this call, so neither the
    // multisink nor it's drains can be used during this callback.
    virtual void OnNewEntryAvailable() = 0;
  };

  class iterator {
   public:
    iterator& operator++() {
      it_++;
      return *this;
    }
    iterator operator++(int) {
      iterator original = *this;
      ++*this;
      return original;
    }

    ConstByteSpan& operator*() {
      entry_ = (*it_).buffer;
      return entry_;
    }
    ConstByteSpan* operator->() { return &operator*(); }

    constexpr bool operator==(const iterator& rhs) const {
      return it_ == rhs.it_;
    }

    constexpr bool operator!=(const iterator& rhs) const {
      return it_ != rhs.it_;
    }

    // Returns the status of the last iteration operation. If the iterator
    // fails to read an entry, it will move to iterator::end() and indicate
    // the failure reason here.
    //
    // Return values:
    // OK - iteration is successful and iterator points to the next entry.
    // DATA_LOSS - Failed to read the metadata at this location.
    Status status() const { return it_.status(); }

   private:
    friend class MultiSink;

    iterator(ring_buffer::PrefixedEntryRingBufferMulti::Reader& reader)
        : it_(reader) {}
    iterator() {}

    ring_buffer::PrefixedEntryRingBufferMulti::iterator it_;
    ConstByteSpan entry_;
  };

  class UnsafeIterationWrapper {
   public:
    using element_type = ConstByteSpan;
    using value_type = std::remove_cv_t<ConstByteSpan>;
    using pointer = ConstByteSpan*;
    using reference = ConstByteSpan&;
    using const_iterator = iterator;  // Standard alias for iterable types.

    iterator begin() const { return iterator(*reader_); }
    iterator end() const { return iterator(); }
    const_iterator cbegin() const { return begin(); }
    const_iterator cend() const { return end(); }

   private:
    friend class MultiSink;
    UnsafeIterationWrapper(
        ring_buffer::PrefixedEntryRingBufferMulti::Reader& reader)
        : reader_(&reader) {}
    ring_buffer::PrefixedEntryRingBufferMulti::Reader* reader_;
  };

  UnsafeIterationWrapper UnsafeIteration() PW_NO_LOCK_SAFETY_ANALYSIS {
    return UnsafeIterationWrapper(oldest_entry_drain_.reader_);
  }

  // Constructs a multisink using a ring buffer backed by the provided buffer.
  MultiSink(ByteSpan buffer)
      : ring_buffer_(true), sequence_id_(0), total_ingress_drops_(0) {
    ring_buffer_.SetBuffer(buffer)
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
    AttachDrain(oldest_entry_drain_);
  }

  // Write an entry to the multisink. If available space is less than the
  // size of the entry, the internal ring buffer will push the oldest entries
  // out to make space, so long as the entry is not larger than the buffer.
  // The sequence ID of the multisink will always increment as a result of
  // calling HandleEntry, regardless of whether pushing the entry succeeds.
  //
  // Precondition: If PW_MULTISINK_LOCK_INTERRUPT_SAFE is disabled, this
  // function must not be called from an interrupt context.
  // Precondition: entry.size() <= `ring_buffer_` size
  void HandleEntry(ConstByteSpan entry) PW_LOCKS_EXCLUDED(lock_);

  // Notifies the multisink of messages dropped before ingress. The writer
  // may use this to signal to readers that an entry (or entries) failed
  // before being sent to the multisink (e.g. the writer failed to encode
  // the message). This API increments the sequence ID of the multisink by
  // the provided `drop_count`.
  void HandleDropped(uint32_t drop_count = 1) PW_LOCKS_EXCLUDED(lock_);

  // Attach a drain to the multisink. Drains may not be associated with more
  // than one multisink at a time. Drains can consume entries pushed before
  // the drain was attached, so long as they have not yet been evicted from
  // the underlying ring buffer.
  //
  // Precondition: The drain must not be attached to a multisink.
  void AttachDrain(Drain& drain) PW_LOCKS_EXCLUDED(lock_);

  // Detaches a drain from the multisink. Drains may only be detached if they
  // were previously attached to this multisink.
  //
  // Precondition: The drain must be attached to this multisink.
  void DetachDrain(Drain& drain) PW_LOCKS_EXCLUDED(lock_);

  // Attach a listener to the multisink. The listener will be notified
  // immediately when attached, to allow late drain users to consume existing
  // entries. If draining in response to the notification, ensure that the drain
  // is attached prior to registering the listener; attempting to drain when
  // unattached will crash. Once attached, listeners are invoked on all new
  // messages.
  //
  // Precondition: The listener must not be attached to a multisink.
  void AttachListener(Listener& listener) PW_LOCKS_EXCLUDED(lock_);

  // Detaches a listener from the multisink.
  //
  // Precondition: The listener must be attached to this multisink.
  void DetachListener(Listener& listener) PW_LOCKS_EXCLUDED(lock_);

  // Removes all data from the internal buffer. The multisink's sequence ID is
  // not modified, so readers may interpret this event as droppping entries.
  void Clear() PW_LOCKS_EXCLUDED(lock_);

  // Uses MultiSink's unsafe iteration to dump the contents to a user-provided
  // callback. max_num_entries can be used to limit the dump to the N most
  // recent entries.
  //
  // Returns:
  //   OK - Successfully dumped entire multisink.
  //   DATA_LOSS - Corruption detected, some entries may have been lost.
  Status UnsafeForEachEntry(
      const Function<void(ConstByteSpan)>& callback,
      size_t max_num_entries = std::numeric_limits<size_t>::max());

 protected:
  friend Drain;

  enum class Request { kPop, kPeek };
  // Removes the previously peeked entry from the front of the multisink.
  Status PopEntry(Drain& drain, const Drain::PeekedEntry& entry)
      PW_LOCKS_EXCLUDED(lock_);

  // Gets a copy of the entry from the provided drain and unpacks sequence ID
  // information. The entry is removed from the multisink when `request` is set
  // to `Request::kPop`. Drains use this API to strip away sequence ID
  // information for drop calculation.
  //
  // Precondition: the buffer data must not be corrupt, otherwise there will
  // be a crash.
  //
  // Returns:
  // OK - An entry was successfully read from the multisink. The
  // `drain_drop_count_out` is set to the difference between the current
  // sequence ID and the last handled ID.
  // FAILED_PRECONDITION - The drain is not attached to
  // a multisink.
  // RESOURCE_EXHAUSTED - The provided buffer was not large enough to store
  // the next available entry, which was discarded.
  Result<ConstByteSpan> PeekOrPopEntry(Drain& drain,
                                       ByteSpan buffer,
                                       Request request,
                                       uint32_t& drain_drop_count_out,
                                       uint32_t& ingress_drop_count_out,
                                       uint32_t& entry_sequence_id_out)
      PW_LOCKS_EXCLUDED(lock_);

 private:
  // Notifies attached listeners of new entries or an updated drop count.
  void NotifyListeners() PW_EXCLUSIVE_LOCKS_REQUIRED(lock_);

  IntrusiveList<Listener> listeners_ PW_GUARDED_BY(lock_);
  ring_buffer::PrefixedEntryRingBufferMulti ring_buffer_ PW_GUARDED_BY(lock_);
  Drain oldest_entry_drain_ PW_GUARDED_BY(lock_);
  uint32_t sequence_id_ PW_GUARDED_BY(lock_);
  uint32_t total_ingress_drops_ PW_GUARDED_BY(lock_);
  LockType lock_;
};

}  // namespace multisink
}  // namespace pw
