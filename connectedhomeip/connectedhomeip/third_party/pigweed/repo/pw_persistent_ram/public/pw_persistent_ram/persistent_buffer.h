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

#include <cstdint>
#include <cstring>
#include <type_traits>
#include <utility>

#include "pw_bytes/span.h"
#include "pw_checksum/crc16_ccitt.h"
#include "pw_preprocessor/compiler.h"
#include "pw_span/span.h"
#include "pw_status/status.h"
#include "pw_stream/stream.h"

namespace pw::persistent_ram {

// A PersistentBufferWriter implements the pw::stream::Writer interface and
// provides handles to mutate and access the underlying data of a
// PersistentBuffer. This object should NOT be stored in persistent RAM.
//
// Only one writer should be open at a given time.
class PersistentBufferWriter : public stream::NonSeekableWriter {
 public:
  PersistentBufferWriter() = delete;

 private:
  template <size_t>
  friend class PersistentBuffer;

  PersistentBufferWriter(ByteSpan buffer,
                         volatile size_t& size,
                         volatile uint16_t& checksum)
      : buffer_(buffer), size_(size), checksum_(checksum) {}

  // Implementation for writing data to this stream.
  Status DoWrite(ConstByteSpan data) override;

  size_t ConservativeLimit(LimitType limit) const override {
    if (limit == LimitType::kWrite) {
      return buffer_.size_bytes() - size_;
    }
    return 0;
  }

  ByteSpan buffer_;
  volatile size_t& size_;
  volatile uint16_t& checksum_;
};

// The PersistentBuffer class intentionally uses uninitialized memory, which
// triggers compiler warnings. Disable those warnings for this file.
PW_MODIFY_DIAGNOSTICS_PUSH();
PW_MODIFY_DIAGNOSTIC(ignored, "-Wuninitialized");
PW_MODIFY_DIAGNOSTIC_GCC(ignored, "-Wmaybe-uninitialized");

// When a PersistentBuffer is statically allocated in persistent memory, its
// state will persist across soft resets in accordance with the expected
// behavior of the underlying RAM. This object is completely safe to use before
// static constructors are called as its constructor is effectively a no-op.
//
// While the stored data can be read by PersistentBuffer's public functions,
// each public function must validate the integrity of the stored data. It's
// typically more performant to get a handle to a PersistentBufferWriter
// instead, as data is validated on creation of the PersistentBufferWriter,
// which allows access to the underlying data without needing to validate the
// data's integrity with each call to PersistentBufferWriter functions.
template <size_t kMaxSizeBytes>
class PersistentBuffer {
 public:
  // The default constructor intentionally does not initialize anything. This
  // allows a persistent buffer statically allocated in persistent RAM to be
  // highly available.
  //
  // Explicitly declaring an empty constructor rather than using the default
  // constructor prevents the object from being zero-initialized when the object
  // is value initialized. If this was left as a default constructor,
  // PersistentBuffer objects declared as value-initialized would be
  // zero-initialized.
  //
  //   // Value initialization:
  //   PersistentBuffer<256> persistent_buffer();
  //
  //   // Default initialization:
  //   PersistentBuffer<256> persistent_buffer;
  PersistentBuffer() {}
  // Disable copy and move constructors.
  PersistentBuffer(const PersistentBuffer&) = delete;
  PersistentBuffer(PersistentBuffer&&) = delete;
  // Explicit no-op destructor.
  ~PersistentBuffer() {}

  PersistentBufferWriter GetWriter() {
    if (!has_value()) {
      clear();
    }
    return PersistentBufferWriter(
        ByteSpan(const_cast<std::byte*>(buffer_), kMaxSizeBytes),
        size_,
        checksum_);
  }

  size_t size() const {
    if (has_value()) {
      return size_;
    }
    return 0;
  }

  const std::byte* data() const { return const_cast<std::byte*>(buffer_); }

  void clear() {
    size_ = 0;
    checksum_ = checksum::Crc16Ccitt::kInitialValue;
  }

  bool has_value() const {
    if (size_ > kMaxSizeBytes || size_ == 0) {
      return false;
    }

    // Check checksum. This is more costly.
    return checksum_ == checksum::Crc16Ccitt::Calculate(ConstByteSpan(
                            const_cast<std::byte*>(buffer_), size_));
  }

 private:
  // None of these members are initialized by the constructor by design.
  volatile uint16_t checksum_;
  volatile size_t size_;
  volatile std::byte buffer_[kMaxSizeBytes];
};

PW_MODIFY_DIAGNOSTICS_POP();

}  // namespace pw::persistent_ram
