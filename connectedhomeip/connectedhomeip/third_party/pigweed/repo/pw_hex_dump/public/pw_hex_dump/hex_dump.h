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

#include <cstdint>

#include "pw_bytes/span.h"
#include "pw_span/span.h"
#include "pw_status/status.h"

namespace pw::dump {

// Size, in bytes, of the resulting string after converting an address to a
// UTF-8 encoded hex representation. This constant depends on the size
// a of a pointer.
//
// Example (32-bit):
//   0x0000F00D
//
// Note: the +2 accounts for the "0x" prefix.
constexpr const size_t kHexAddrStringSize = sizeof(uintptr_t) * 2 + 2;

// It is strongly recommended NOT to directly depend on this dump format;
// pw_hex_dump does NOT guarantee stability for the output format, but strives
// to remain xxd compatible.
//
// Default:
//   Offs.  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  Text
//   0000: A4 CC 32 62 9B 46 38 1A 23 1A 2A 7A BC E2 40 A0  ..2b.F8.#.*z..@.
//   0010: FF 33 E5 2B 9E 9F 6B 3C BE 9B 89 3C 7E 4A 7A 48  .3.+..k<...<~JzH
//   0020: 18                                               .
//
// Example 1 (32-bit machine, group_every=4, prefix_mode=kAbsolute,
//            bytes_per_line = 8):
//   Address      0        4        Text
//   0x20000000: A4CC3262 9B46381A  ..2b.F8.
//   0x20000008: 231A2A7A BCE240A0  #.*z..@.
//   0x20000010: FF33E52B 9E9F6B3C  .3.+..k<
//   0x20000018: BE9B893C 7E4A7A48  ...<~JzH
//   0x20000020: 18                 .
//
// Example 2 (group_every=1, bytes_per_line = 16):
//   Offs.  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
//   0000: A4 CC 32 62 9B 46 38 1A 23 1A 2A 7A BC E2 40 A0
//   0010: FF 33 E5 2B 9E 9F 6B 3C BE 9B 89 3C 7E 4A 7A 48
//   0020: 18
//
// Example 3 (group_every=0, prefix_mode=kNone, show_header=false,
//            show_ascii=false):
//   A4CC32629B46381A231A2A7ABCE240A0
//   FF33E52B9E9F6B3CBE9B893C7E4A7A48
//   18
class FormattedHexDumper {
 public:
  enum AddressMode {
    kDisabled = 0,
    kOffset = 1,
    kAbsolute = 2,
  };

  struct Flags {
    // Sets the number of source data bytes to print in each formatted line.
    uint8_t bytes_per_line : 8;

    // Inserts a space every N bytes for readability. Note that this is in
    // number of bytes converted to characters. Set to zero to disable. I.e. a
    // value of 2 results in:
    //   0x00000000: 0102 0304 0506 0708
    uint8_t group_every : 8;

    // Show or hide ascii interpretation of binary data.
    bool show_ascii : 1;

    // Show descriptive column headers.
    bool show_header : 1;

    // Prefix each line of the dump with an offset or absolute address.
    AddressMode prefix_mode : 2;
  };

  Flags flags = {.bytes_per_line = 16,
                 .group_every = 1,
                 .show_ascii = true,
                 .show_header = true,
                 .prefix_mode = AddressMode::kOffset};

  FormattedHexDumper() = default;
  FormattedHexDumper(span<char> dest) {
    SetLineBuffer(dest)
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
  }
  FormattedHexDumper(span<char> dest, Flags config_flags)
      : flags(config_flags) {
    SetLineBuffer(dest)
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
  }

  // TODO(b/234892215): Add iterator support.

  // Set the destination buffer that the hex dumper will write to line-by-line.
  //
  // Returns:
  //   RESOURCE_EXHAUSTED - The buffer was set, but is too small to fit the
  //     current formatting configuration.
  //   INVALID_ARGUMENT - The destination buffer is invalid (nullptr or zero-
  //     length).
  Status SetLineBuffer(span<char> dest);

  // Begin dumping the provided data. Does NOT populate the line buffer with
  // a string, simply resets the statefulness to track this buffer.
  //
  // Returns:
  //   OK - Ready to begin dump.
  //   INVALID_ARGUMENT - The source data starts at null, but has been set.
  //   FAILED_PRECONDITION - Line buffer too small to hold current formatting
  //     settings.
  Status BeginDump(ConstByteSpan data);

  // Dumps a single line to the line buffer.
  //
  // Example usage:
  //
  //   std::array<char, 80> temp;
  //   FormattedHexDumper hex_dumper(temp);
  //   hex_dumper.BeginDump(my_data);
  //   while(hex_dumper.DumpLine().ok()) {
  //     LOG_INFO("%s", temp.data());
  //   }
  //
  // Returns:
  //   OK - A line has been written to the line buffer.
  //   RESOURCE_EXHAUSTED - All the data has been dumped.
  //   FAILED_PRECONDITION - Destination line buffer is too small to fit current
  //     formatting configuration.
  Status DumpLine();

 private:
  Status ValidateBufferSize();
  Status PrintFormatHeader();

  size_t current_offset_;
  span<char> dest_;
  ConstByteSpan source_data_;
};

// Dumps a uintptr_t to a character buffer as a hex address. This may be useful
// to print out an address in a generalized way when %z and %p aren't supported
// by a standard library implementation. The destination buffer MUST be large
// enough to hold kHexAddrStringSize + 1 (null terminator) bytes.
//
// Example (64-bit):
//   0x000000000022b698
// Example (32-bit):
//   0x70000000
//
// Returns:
//   OK - Address has been written to the buffer.
//   INVALID_ARGUMENT - The destination buffer is invalid (nullptr).
//   RESOURCE_EXHAUSTED - The destination buffer is too small. No data written.
Status DumpAddr(span<char> dest, uintptr_t addr);
inline Status DumpAddr(span<char> dest, const void* ptr) {
  uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
  return DumpAddr(dest, addr);
}

}  // namespace pw::dump
