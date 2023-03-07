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

#include "pw_hex_dump/hex_dump.h"

#include <cctype>
#include <cstddef>
#include <string_view>

#include "pw_status/status_with_size.h"
#include "pw_string/string_builder.h"
#include "pw_string/type_to_string.h"

using pw::string::HexDigitCount;
using pw::string::IntToHexString;

namespace pw::dump {
namespace {

constexpr const std::string_view kAddressSeparator(": ");
constexpr const std::string_view kSectionSeparator("  ");
constexpr const std::string_view kAddressHeader("Address");
constexpr const std::string_view kOffsetHeader("Offs.");
constexpr const std::string_view kAsciiHeader("Text");

// Minimum number of hex characters to use when displaying dump offset.
constexpr const size_t kMinOffsetChars = 4;

char PrintableChar(std::byte b) {
  if (std::isprint(std::to_integer<char>(b)) == 0) {
    return '.';
  }
  return std::to_integer<char>(b);
}

void AddGroupingByte(size_t byte_index,
                     FormattedHexDumper::Flags& flags,
                     StringBuilder& builder) {
  // Never add grouping when it is disabled.
  if (flags.group_every == 0) {
    return;
  }
  // If this byte isn't at the end of a group, don't add a space.
  if ((byte_index + 1) % flags.group_every != 0) {
    return;
  }
  // If this byte is the last byte in a line, don't add a grouping byte
  // (prevents trailing spaces).
  if (byte_index + 1 == flags.bytes_per_line) {
    return;
  }

  builder << ' ';
}

}  // namespace

Status DumpAddr(span<char> dest, uintptr_t addr) {
  if (dest.data() == nullptr) {
    return Status::InvalidArgument();
  }
  // Include null terminator.
  if (dest.size() < kHexAddrStringSize + 1) {
    return Status::ResourceExhausted();
  }
  dest[0] = '0';
  dest[1] = 'x';

  return IntToHexString(addr, dest.subspan(2), sizeof(uintptr_t) * 2).status();
}

Status FormattedHexDumper::PrintFormatHeader() {
  StringBuilder builder(dest_);

  if (flags.prefix_mode != AddressMode::kDisabled) {
    std::string_view header(flags.prefix_mode == AddressMode::kOffset
                                ? kOffsetHeader
                                : kAddressHeader);
    // Pad to align to address width.
    size_t padding = 0;
    if (flags.prefix_mode == AddressMode::kOffset) {
      size_t offs_width =
          HexDigitCount(source_data_.size_bytes() + current_offset_);
      padding = std::max(offs_width, kMinOffsetChars);
    } else {
      padding = kHexAddrStringSize;
    }

    padding += kAddressSeparator.length();
    padding -= header.size();

    builder << header;
    builder.append(padding, ' ');
  }

  // Print offsets.
  for (size_t i = 0; i < static_cast<size_t>(flags.bytes_per_line); ++i) {
    // Early loop termination for when bytes_remaining <
    // bytes_per_line.
    if (flags.group_every != 0 &&
        i % static_cast<uint8_t>(flags.group_every) == 0) {
      builder << std::byte(i);
    } else {
      builder.append(2, ' ');
    }
    AddGroupingByte(i, flags, builder);
  }

  if (flags.show_ascii) {
    builder << kSectionSeparator;
    builder << kAsciiHeader;
  }

  return builder.status();
}

Status FormattedHexDumper::DumpLine() {
  if (source_data_.empty()) {
    return Status::ResourceExhausted();
  }

  if (!ValidateBufferSize().ok() || dest_.data() == nullptr) {
    return Status::FailedPrecondition();
  }

  if (dest_[0] == 0 && flags.show_header) {
    // First line, print out dump format header.
    return PrintFormatHeader();
  }

  StringBuilder builder(dest_);
  // Dump address/offset prefix.
  // TODO(amontanez): This block can be much nicer if StringBuilder exposed an
  // easy way to control zero padding for hex address.
  if (flags.prefix_mode != AddressMode::kDisabled) {
    uintptr_t val;
    size_t significant;
    if (flags.prefix_mode == AddressMode::kAbsolute) {
      val = reinterpret_cast<uintptr_t>(source_data_.data());
      builder << "0x";
      significant = HexDigitCount(val);
      builder.append(sizeof(uintptr_t) * 2 - significant, '0');
    } else {
      val = current_offset_;
      significant = HexDigitCount(val);
      if (significant < kMinOffsetChars) {
        builder.append(kMinOffsetChars - significant, '0');
      }
    }
    if (val != 0) {
      builder << reinterpret_cast<void*>(val);
    } else {
      builder.append(significant, '0');
    }
    builder << kAddressSeparator;
  }

  size_t bytes_in_line = std::min(source_data_.size_bytes(),
                                  static_cast<size_t>(flags.bytes_per_line));
  // Convert raw bytes to hex characters.
  for (size_t i = 0; i < bytes_in_line; ++i) {
    // Early loop termination for when bytes_remaining <
    // bytes_per_line.
    builder << source_data_[i];
    AddGroupingByte(i, flags, builder);
  }
  // Add padding spaces to ensure lines are aligned.
  if (flags.show_ascii) {
    for (size_t i = bytes_in_line;
         i < static_cast<size_t>(flags.bytes_per_line);
         ++i) {
      builder.append(2, ' ');
      AddGroupingByte(i, flags, builder);
    }
  }

  // Interpret bytes as characters.
  if (flags.show_ascii) {
    builder << kSectionSeparator;
    for (size_t i = 0; i < bytes_in_line; ++i) {
      builder << PrintableChar(source_data_[i]);
    }
  }

  source_data_ = source_data_.subspan(bytes_in_line);
  current_offset_ += bytes_in_line;
  return builder.status();
}

Status FormattedHexDumper::SetLineBuffer(span<char> dest) {
  if (dest.data() == nullptr || dest.size_bytes() == 0) {
    return Status::InvalidArgument();
  }
  dest_ = dest;
  return ValidateBufferSize().ok() ? OkStatus() : Status::ResourceExhausted();
}

Status FormattedHexDumper::BeginDump(ConstByteSpan data) {
  current_offset_ = 0;
  source_data_ = data;
  if (data.data() == nullptr) {
    return Status::InvalidArgument();
  }
  if (dest_.data() != nullptr && dest_.size_bytes() > 0) {
    dest_[0] = 0;
  }
  return ValidateBufferSize().ok() ? OkStatus() : Status::FailedPrecondition();
}

Status FormattedHexDumper::ValidateBufferSize() {
  // Minimum size is number of bytes per line as hex pairs plus the null
  // terminator.
  size_t required_size = flags.bytes_per_line * 2 + 1;
  if (flags.show_ascii) {
    required_size += kSectionSeparator.length() + flags.bytes_per_line;
  }
  if (flags.prefix_mode == AddressMode::kAbsolute) {
    required_size += kHexAddrStringSize;
    required_size += kAddressSeparator.length();
  } else if (flags.prefix_mode == AddressMode::kOffset) {
    required_size +=
        HexDigitCount(std::max(source_data_.size_bytes(), kMinOffsetChars));
    required_size += kAddressSeparator.length();
  }
  if (flags.group_every != 0) {
    required_size += (flags.bytes_per_line - 1) / flags.group_every;
  }

  if (dest_.size_bytes() < required_size) {
    return Status::ResourceExhausted();
  }

  return OkStatus();
}

}  // namespace pw::dump
