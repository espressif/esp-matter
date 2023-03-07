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
#include <functional>  // std::invoke

#include "pw_assert/assert.h"
#include "pw_bytes/span.h"
#include "pw_checksum/crc32.h"
#include "pw_hdlc/internal/protocol.h"
#include "pw_result/result.h"
#include "pw_status/status.h"

namespace pw::hdlc {

// Represents the contents of an HDLC frame -- the unescaped data between two
// flag bytes. Instances of Frame are only created when a full, valid frame has
// been read.
class Frame {
 public:
  // The minimum size of a frame, excluding control bytes (flag or escape).
  static constexpr size_t kMinContentSizeBytes =
      kMinAddressSize + kControlSize + kFcsSize;

  static Result<Frame> Parse(ConstByteSpan frame);

  constexpr uint64_t address() const { return address_; }

  constexpr std::byte control() const { return control_; }

  constexpr ConstByteSpan data() const { return data_; }

 private:
  // Creates a Frame with the specified data. The data MUST be valid frame data
  // with a verified frame check sequence.
  constexpr Frame(uint64_t address, std::byte control, ConstByteSpan data)
      : data_(data), address_(address), control_(control) {}

  ConstByteSpan data_;
  uint64_t address_;
  std::byte control_;
};

// The Decoder class facilitates decoding of data frames using the HDLC
// protocol, by returning packets as they are decoded and storing incomplete
// data frames in a buffer.
//
// The Decoder class does not own the buffer it writes to. It can be used to
// write bytes to any buffer. The DecoderBuffer template class, defined below,
// allocates a buffer.
class Decoder {
 public:
  constexpr Decoder(ByteSpan buffer)
      : buffer_(buffer),
        last_read_bytes_({}),
        last_read_bytes_index_(0),
        current_frame_size_(0),
        state_(State::kInterFrame) {}

  Decoder(const Decoder&) = delete;
  Decoder& operator=(const Decoder&) = delete;

  // Parses a single byte of an HDLC stream. Returns a Result with the complete
  // frame if the byte completes a frame. The status is the following:
  //
  //     OK - A frame was successfully decoded. The Result contains the Frame,
  //         which is invalidated by the next Process call.
  //     UNAVAILABLE - No frame is available.
  //     RESOURCE_EXHAUSTED - A frame completed, but it was too large to fit in
  //         the decoder's buffer.
  //     DATA_LOSS - A frame completed, but it was invalid. The frame was
  //         incomplete or the frame check sequence verification failed.
  //
  Result<Frame> Process(std::byte new_byte);

  // Returns the buffer space required for a `Decoder` to successfully decode a
  // frame whose on-the-wire HDLC encoded size does not exceed `max_frame_size`.
  static constexpr size_t RequiredBufferSizeForFrameSize(
      size_t max_frame_size) {
    // Flag bytes aren't stored in the internal buffer, so we can save a couple
    // bytes.
    return max_frame_size < Frame::kMinContentSizeBytes
               ? Frame::kMinContentSizeBytes
               : max_frame_size - 2;
  }

  // Processes a span of data and calls the provided callback with each frame or
  // error.
  template <typename F, typename... Args>
  void Process(ConstByteSpan data, F&& callback, Args&&... args) {
    for (std::byte b : data) {
      auto result = Process(b);
      if (result.status() != Status::Unavailable()) {
        std::invoke(
            std::forward<F>(callback), std::forward<Args>(args)..., result);
      }
    }
  }

  // Returns the maximum size of the Decoder's frame buffer.
  size_t max_size() const { return buffer_.size(); }

  // Clears and resets the decoder.
  void Clear() {
    state_ = State::kInterFrame;
    Reset();
  }

 private:
  // State enum class is used to make the Decoder a finite state machine.
  enum class State {
    kInterFrame,
    kFrame,
    kFrameEscape,
  };

  void Reset() {
    current_frame_size_ = 0;
    last_read_bytes_index_ = 0;
    fcs_.clear();
  }

  void AppendByte(std::byte new_byte);

  Status CheckFrame() const;

  bool VerifyFrameCheckSequence() const;

  const ByteSpan buffer_;

  // Ring buffer of the last four bytes read into the current frame, to allow
  // calculating the frame's CRC incrementally. As data is evicted from this
  // buffer, it is added to the running CRC. Once a frame is complete, the
  // buffer contains the frame's FCS.
  std::array<std::byte, sizeof(uint32_t)> last_read_bytes_;
  size_t last_read_bytes_index_;

  // Incremental checksum of the current frame.
  checksum::Crc32 fcs_;

  size_t current_frame_size_;

  State state_;
};

// DecoderBuffers declare a buffer along with a Decoder.
template <size_t kSizeBytes>
class DecoderBuffer : public Decoder {
 public:
  DecoderBuffer() : Decoder(frame_buffer_) {}

  // Returns the maximum length of the bytes that can be inserted in the bytes
  // buffer.
  static constexpr size_t max_size() { return kSizeBytes; }

 private:
  static_assert(kSizeBytes >= Frame::kMinContentSizeBytes);

  std::array<std::byte, kSizeBytes> frame_buffer_;
};

}  // namespace pw::hdlc
