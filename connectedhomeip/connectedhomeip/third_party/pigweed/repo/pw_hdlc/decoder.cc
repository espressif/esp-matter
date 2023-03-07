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

#include "pw_hdlc/decoder.h"

#include "pw_assert/check.h"
#include "pw_bytes/endian.h"
#include "pw_hdlc/internal/protocol.h"
#include "pw_log/log.h"
#include "pw_varint/varint.h"

using std::byte;

namespace pw::hdlc {

Result<Frame> Frame::Parse(ConstByteSpan frame) {
  uint64_t address;
  size_t address_size = varint::Decode(frame, &address, kAddressFormat);
  int data_size = frame.size() - address_size - kControlSize - kFcsSize;

  if (address_size == 0 || data_size < 0) {
    return Status::DataLoss();
  }

  return Frame(
      address, frame[address_size], frame.subspan(address_size + 1, data_size));
}

Result<Frame> Decoder::Process(const byte new_byte) {
  switch (state_) {
    case State::kInterFrame: {
      if (new_byte == kFlag) {
        state_ = State::kFrame;

        // Report an error if non-flag bytes were read between frames.
        if (current_frame_size_ != 0u) {
          Reset();
          return Status::DataLoss();
        }
      } else {
        // Count bytes to track how many are discarded.
        current_frame_size_ += 1;
      }
      return Status::Unavailable();  // Report error when starting a new frame.
    }
    case State::kFrame: {
      if (new_byte == kFlag) {
        const Status status = CheckFrame();

        const size_t completed_frame_size = current_frame_size_;
        Reset();

        if (status.ok()) {
          return Frame::Parse(buffer_.first(completed_frame_size));
        }
        return status;
      }

      if (new_byte == kEscape) {
        state_ = State::kFrameEscape;
      } else {
        AppendByte(new_byte);
      }
      return Status::Unavailable();
    }
    case State::kFrameEscape: {
      // The flag character cannot be escaped; return an error.
      if (new_byte == kFlag) {
        state_ = State::kFrame;
        Reset();
        return Status::DataLoss();
      }

      if (new_byte == kEscape) {
        // Two escape characters in a row is illegal -- invalidate this frame.
        // The frame is reported abandoned when the next flag byte appears.
        state_ = State::kInterFrame;

        // Count the escape byte so that the inter-frame state detects an error.
        current_frame_size_ += 1;
      } else {
        state_ = State::kFrame;
        AppendByte(Escape(new_byte));
      }
      return Status::Unavailable();
    }
  }
  PW_CRASH("Bad decoder state");
}

void Decoder::AppendByte(byte new_byte) {
  if (current_frame_size_ < max_size()) {
    buffer_[current_frame_size_] = new_byte;
  }

  if (current_frame_size_ >= last_read_bytes_.size()) {
    // A byte will be ejected. Add it to the running checksum.
    fcs_.Update(last_read_bytes_[last_read_bytes_index_]);
  }

  last_read_bytes_[last_read_bytes_index_] = new_byte;
  last_read_bytes_index_ =
      (last_read_bytes_index_ + 1) % last_read_bytes_.size();

  // Always increase size: if it is larger than the buffer, overflow occurred.
  current_frame_size_ += 1;
}

Status Decoder::CheckFrame() const {
  // Empty frames are not an error; repeated flag characters are okay.
  if (current_frame_size_ == 0u) {
    return Status::Unavailable();
  }

  if (current_frame_size_ < Frame::kMinContentSizeBytes) {
    PW_LOG_ERROR("Received %lu-byte frame; frame must be at least 6 bytes",
                 static_cast<unsigned long>(current_frame_size_));
    return Status::DataLoss();
  }

  if (!VerifyFrameCheckSequence()) {
    PW_LOG_ERROR("Frame check sequence verification failed");
    return Status::DataLoss();
  }

  if (current_frame_size_ > max_size()) {
    // Frame does not fit into the provided buffer; indicate this to the caller.
    // This may not be considered an error if the caller is doing a partial
    // decode.
    return Status::ResourceExhausted();
  }

  return OkStatus();
}

bool Decoder::VerifyFrameCheckSequence() const {
  // De-ring the last four bytes read, which at this point contain the FCS.
  std::array<std::byte, sizeof(uint32_t)> fcs_buffer;
  size_t index = last_read_bytes_index_;

  for (size_t i = 0; i < fcs_buffer.size(); ++i) {
    fcs_buffer[i] = last_read_bytes_[index];
    index = (index + 1) % last_read_bytes_.size();
  }

  uint32_t actual_fcs =
      bytes::ReadInOrder<uint32_t>(endian::little, fcs_buffer);
  return actual_fcs == fcs_.value();
}

}  // namespace pw::hdlc
