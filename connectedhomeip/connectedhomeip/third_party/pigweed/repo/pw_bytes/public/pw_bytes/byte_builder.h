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
#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstring>

#include "pw_bytes/bit.h"
#include "pw_bytes/endian.h"
#include "pw_bytes/span.h"
#include "pw_containers/iterator.h"
#include "pw_preprocessor/compiler.h"
#include "pw_status/status.h"
#include "pw_status/status_with_size.h"

namespace pw {

// ByteBuilder facilitates building bytes in a fixed-size buffer.
// BytesBuilders never overflow. Status is tracked for each operation and
// an overall status is maintained, which reflects the most recent error.
//
// A ByteBuilder does not own the buffer it writes to. It can be used to write
// bytes to any buffer. The ByteBuffer template class, defined below,
// allocates a buffer alongside a ByteBuilder.
class ByteBuilder {
 public:
  // iterator class will allow users of ByteBuilder and ByteBuffer to access
  // the data stored in the buffer. It has the functionality of C++'s
  // random access iterator.
  class iterator {
   public:
    using difference_type = ptrdiff_t;
    using value_type = std::byte;
    using element_type = const std::byte;
    using pointer = const std::byte*;
    using reference = const std::byte&;
    using iterator_category = containers::contiguous_iterator_tag;

    explicit constexpr iterator(const std::byte* byte_ptr = nullptr)
        : byte_(byte_ptr) {}

    constexpr iterator& operator++() {
      byte_ += 1;
      return *this;
    }

    constexpr iterator operator++(int) {
      iterator previous(byte_);
      operator++();
      return previous;
    }

    constexpr iterator& operator--() {
      byte_ -= 1;
      return *this;
    }

    constexpr iterator operator--(int) {
      iterator previous(byte_);
      operator--();
      return previous;
    }

    constexpr iterator& operator+=(int n) {
      byte_ += n;
      return *this;
    }

    constexpr iterator operator+(int n) const { return iterator(byte_ + n); }

    constexpr iterator& operator-=(int n) { return operator+=(-n); }

    constexpr iterator operator-(int n) const { return iterator(byte_ - n); }

    constexpr difference_type operator-(const iterator& rhs) const {
      return byte_ - rhs.byte_;
    }

    constexpr reference operator*() const { return *byte_; }

    constexpr pointer operator->() const { return byte_; }

    constexpr reference operator[](int index) const { return byte_[index]; }

    constexpr bool operator==(const iterator& rhs) const {
      return byte_ == rhs.byte_;
    }

    constexpr bool operator!=(const iterator& rhs) const {
      return byte_ != rhs.byte_;
    }

    constexpr bool operator<(const iterator& rhs) const {
      return byte_ < rhs.byte_;
    }

    constexpr bool operator>(const iterator& rhs) const {
      return byte_ > rhs.byte_;
    }

    constexpr bool operator<=(const iterator& rhs) const {
      return !operator>(rhs);
    }

    constexpr bool operator>=(const iterator& rhs) const {
      return !operator<(rhs);
    }

    // The Peek methods will retreive ordered (Little/Big Endian) values
    // located at the iterator position without moving the iterator forward.
    int8_t PeekInt8() const { return static_cast<int8_t>(PeekUint8()); }

    uint8_t PeekUint8() const {
      return bytes::ReadInOrder<uint8_t>(endian::little, byte_);
    }

    int16_t PeekInt16(endian order = endian::little) const {
      return static_cast<int16_t>(PeekUint16(order));
    }

    uint16_t PeekUint16(endian order = endian::little) const {
      return bytes::ReadInOrder<uint16_t>(order, byte_);
    }

    int32_t PeekInt32(endian order = endian::little) const {
      return static_cast<int32_t>(PeekUint32(order));
    }

    uint32_t PeekUint32(endian order = endian::little) const {
      return bytes::ReadInOrder<uint32_t>(order, byte_);
    }

    int64_t PeekInt64(endian order = endian::little) const {
      return static_cast<int64_t>(PeekUint64(order));
    }

    uint64_t PeekUint64(endian order = endian::little) const {
      return bytes::ReadInOrder<uint64_t>(order, byte_);
    }

    // The Read methods will retreive ordered (Little/Big Endian) values
    // located at the iterator position and move the iterator forward by
    // sizeof(value) positions forward.
    int8_t ReadInt8() { return static_cast<int8_t>(ReadUint8()); }

    uint8_t ReadUint8() {
      uint8_t value = bytes::ReadInOrder<uint8_t>(endian::little, byte_);
      byte_ += 1;
      return value;
    }

    int16_t ReadInt16(endian order = endian::little) {
      return static_cast<int16_t>(ReadUint16(order));
    }

    uint16_t ReadUint16(endian order = endian::little) {
      uint16_t value = bytes::ReadInOrder<uint16_t>(order, byte_);
      byte_ += 2;
      return value;
    }

    int32_t ReadInt32(endian order = endian::little) {
      return static_cast<int32_t>(ReadUint32(order));
    }

    uint32_t ReadUint32(endian order = endian::little) {
      uint32_t value = bytes::ReadInOrder<uint32_t>(order, byte_);
      byte_ += 4;
      return value;
    }

    int64_t ReadInt64(endian order = endian::little) {
      return static_cast<int64_t>(ReadUint64(order));
    }

    uint64_t ReadUint64(endian order = endian::little) {
      int64_t value = bytes::ReadInOrder<int64_t>(order, byte_);
      byte_ += 8;
      return value;
    }

   private:
    const std::byte* byte_;
  };

  using element_type = const std::byte;
  using value_type = std::byte;
  using pointer = std::byte*;
  using reference = std::byte&;
  using iterator = iterator;
  using const_iterator = iterator;

  // Creates an empty ByteBuilder.
  constexpr ByteBuilder(ByteSpan buffer) : buffer_(buffer), size_(0) {}

  // Disallow copy/assign to avoid confusion about where the bytes is actually
  // stored. ByteBuffers may be copied into one another.
  ByteBuilder(const ByteBuilder&) = delete;

  ByteBuilder& operator=(const ByteBuilder&) = delete;

  // Returns the contents of the bytes buffer.
  const std::byte* data() const { return buffer_.data(); }

  // Returns the ByteBuilder's status, which reflects the most recent error
  // that occurred while updating the bytes. After an update fails, the status
  // remains non-OK until it is cleared with clear() or clear_status(). Returns:
  //
  //     OK if no errors have occurred
  //     RESOURCE_EXHAUSTED if output to the ByteBuilder was truncated
  //     INVALID_ARGUMENT if printf-style formatting failed
  //     OUT_OF_RANGE if an operation outside the buffer was attempted
  //
  Status status() const { return status_; }

  // Returns status() and size() as a StatusWithSize.
  StatusWithSize status_with_size() const {
    return StatusWithSize(status_, size_);
  }

  // True if status() is OkStatus().
  bool ok() const { return status_.ok(); }

  // True if the bytes builder is empty.
  bool empty() const { return size() == 0u; }

  // Returns the current length of the bytes.
  size_t size() const { return size_; }

  // Returns the maximum length of the bytes.
  size_t max_size() const { return buffer_.size(); }

  // Clears the bytes and resets its error state.
  void clear() {
    size_ = 0;
    status_ = OkStatus();
  }

  // Sets the statuses to OkStatus();
  void clear_status() { status_ = OkStatus(); }

  // Appends a single byte. Sets the status to RESOURCE_EXHAUSTED if the
  // byte cannot be added because the buffer is full.
  void push_back(std::byte b) { append(1, b); }

  // Removes the last byte. Sets the status to OUT_OF_RANGE if the buffer
  // is empty (in which case the unsigned overflow is intentional).
  void pop_back() PW_NO_SANITIZE("unsigned-integer-overflow") {
    resize(size() - 1);
  }

  // Root of bytebuffer wrapped in iterator type
  const_iterator begin() const { return iterator(data()); }
  const_iterator cbegin() const { return begin(); }

  // End of bytebuffer wrapped in iterator type
  const_iterator end() const { return iterator(data() + size()); }
  const_iterator cend() const { return end(); }

  // Front and Back C++ container functions
  const std::byte& front() const { return buffer_[0]; }
  const std::byte& back() const { return buffer_[size() - 1]; }

  // Appends the provided byte count times.
  ByteBuilder& append(size_t count, std::byte b);

  // Appends count bytes from 'bytes' to the end of the ByteBuilder. If count
  // exceeds the remaining space in the ByteBuffer, no bytes will be appended
  // and the status is set to RESOURCE_EXHAUSTED.
  ByteBuilder& append(const void* bytes, size_t count);

  // Appends bytes from a byte span that calls the pointer/length version.
  ByteBuilder& append(ConstByteSpan bytes) {
    return append(bytes.data(), bytes.size());
  }

  // Sets the ByteBuilder's size. This function only truncates; if
  // new_size > size(), it sets status to OUT_OF_RANGE and does nothing.
  void resize(size_t new_size);

  // Put methods for inserting different 8-bit ints
  ByteBuilder& PutUint8(uint8_t val) { return WriteInOrder(val); }

  ByteBuilder& PutInt8(int8_t val) { return WriteInOrder(val); }

  // Put methods for inserting different 16-bit ints
  ByteBuilder& PutUint16(uint16_t value, endian order = endian::little) {
    return WriteInOrder(bytes::ConvertOrderTo(order, value));
  }

  ByteBuilder& PutInt16(int16_t value, endian order = endian::little) {
    return PutUint16(static_cast<uint16_t>(value), order);
  }

  // Put methods for inserting different 32-bit ints
  ByteBuilder& PutUint32(uint32_t value, endian order = endian::little) {
    return WriteInOrder(bytes::ConvertOrderTo(order, value));
  }

  ByteBuilder& PutInt32(int32_t value, endian order = endian::little) {
    return PutUint32(static_cast<uint32_t>(value), order);
  }

  // Put methods for inserting different 64-bit ints
  ByteBuilder& PutUint64(uint64_t value, endian order = endian::little) {
    return WriteInOrder(bytes::ConvertOrderTo(order, value));
  }

  ByteBuilder& PutInt64(int64_t value, endian order = endian::little) {
    return PutUint64(static_cast<uint64_t>(value), order);
  }

 protected:
  // Functions to support ByteBuffer copies.
  constexpr ByteBuilder(const ByteSpan& buffer, const ByteBuilder& other)
      : buffer_(buffer), size_(other.size_), status_(other.status_) {}

  void CopySizeAndStatus(const ByteBuilder& other) {
    size_ = other.size_;
    status_ = other.status_;
  }

 private:
  template <typename T>
  ByteBuilder& WriteInOrder(T value) {
    return append(&value, sizeof(value));
  }
  size_t ResizeForAppend(size_t bytes_to_append);

  const ByteSpan buffer_;

  size_t size_;
  Status status_;
};

// ByteBuffers declare a buffer along with a ByteBuilder.
template <size_t kSizeBytes>
class ByteBuffer : public ByteBuilder {
 public:
  ByteBuffer() : ByteBuilder(buffer_) {}

  // ByteBuffers of the same size may be copied and assigned into one another.
  ByteBuffer(const ByteBuffer& other) : ByteBuilder(buffer_, other) {
    CopyContents(other);
  }

  // A smaller ByteBuffer may be copied or assigned into a larger one.
  template <size_t kOtherSizeBytes>
  ByteBuffer(const ByteBuffer<kOtherSizeBytes>& other)
      : ByteBuilder(buffer_, other) {
    static_assert(ByteBuffer<kOtherSizeBytes>::max_size() <= max_size(),
                  "A ByteBuffer cannot be copied into a smaller buffer");
    CopyContents(other);
  }

  template <size_t kOtherSizeBytes>
  ByteBuffer& operator=(const ByteBuffer<kOtherSizeBytes>& other) {
    assign<kOtherSizeBytes>(other);
    return *this;
  }

  ByteBuffer& operator=(const ByteBuffer& other) {
    assign<kSizeBytes>(other);
    return *this;
  }

  template <size_t kOtherSizeBytes>
  ByteBuffer& assign(const ByteBuffer<kOtherSizeBytes>& other) {
    static_assert(ByteBuffer<kOtherSizeBytes>::max_size() <= max_size(),
                  "A ByteBuffer cannot be copied into a smaller buffer");
    CopySizeAndStatus(other);
    CopyContents(other);
    return *this;
  }

  // Returns the maximum length of the bytes that can be inserted in the bytes
  // buffer.
  static constexpr size_t max_size() { return kSizeBytes; }

  // Returns a ByteBuffer<kSizeBytes>& instead of a generic ByteBuilder& for
  // append calls.
  template <typename... Args>
  ByteBuffer& append(Args&&... args) {
    ByteBuilder::append(std::forward<Args>(args)...);
    return *this;
  }

 private:
  template <size_t kOtherSize>
  void CopyContents(const ByteBuffer<kOtherSize>& other) {
    std::memcpy(buffer_.data(), other.data(), other.size());
  }

  std::array<std::byte, kSizeBytes> buffer_;
};

constexpr ByteBuilder::iterator operator+(int n, ByteBuilder::iterator it) {
  return it + n;
}

}  // namespace pw
