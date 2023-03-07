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

#include "pw_hdlc/encoder.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstring>

#include "pw_bytes/endian.h"
#include "pw_hdlc/encoded_size.h"
#include "pw_hdlc/internal/encoder.h"
#include "pw_span/span.h"
#include "pw_varint/varint.h"

using std::byte;

namespace pw::hdlc {
namespace internal {

Status EscapeAndWrite(const byte b, stream::Writer& writer) {
  if (b == kFlag) {
    return writer.Write(kEscapedFlag);
  }
  if (b == kEscape) {
    return writer.Write(kEscapedEscape);
  }
  return writer.Write(b);
}

Status Encoder::WriteData(ConstByteSpan data) {
  auto begin = data.begin();
  while (true) {
    auto end = std::find_if(begin, data.end(), NeedsEscaping);

    if (Status status = writer_.Write(span(begin, end)); !status.ok()) {
      return status;
    }
    if (end == data.end()) {
      fcs_.Update(data);
      return OkStatus();
    }
    if (Status status = EscapeAndWrite(*end, writer_); !status.ok()) {
      return status;
    }
    begin = end + 1;
  }
}

Status Encoder::FinishFrame() {
  if (Status status =
          WriteData(bytes::CopyInOrder(endian::little, fcs_.value()));
      !status.ok()) {
    return status;
  }
  return writer_.Write(kFlag);
}

Status Encoder::StartFrame(uint64_t address, std::byte control) {
  fcs_.clear();
  if (Status status = writer_.Write(kFlag); !status.ok()) {
    return status;
  }

  std::array<std::byte, 16> metadata_buffer;
  size_t metadata_size =
      varint::Encode(address, metadata_buffer, kAddressFormat);
  if (metadata_size == 0) {
    return Status::InvalidArgument();
  }

  metadata_buffer[metadata_size++] = control;
  return WriteData(span(metadata_buffer).first(metadata_size));
}

}  // namespace internal

Status WriteUIFrame(uint64_t address,
                    ConstByteSpan payload,
                    stream::Writer& writer) {
  if (MaxEncodedFrameSize(address, payload) > writer.ConservativeWriteLimit()) {
    return Status::ResourceExhausted();
  }

  internal::Encoder encoder(writer);

  if (Status status = encoder.StartUnnumberedFrame(address); !status.ok()) {
    return status;
  }
  if (Status status = encoder.WriteData(payload); !status.ok()) {
    return status;
  }
  return encoder.FinishFrame();
}

}  // namespace pw::hdlc
