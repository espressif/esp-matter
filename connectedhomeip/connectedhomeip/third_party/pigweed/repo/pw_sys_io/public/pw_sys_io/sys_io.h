// Copyright 2019 The Pigweed Authors
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

// This module defines a simple and unoptimized interface for byte-by-byte
// input/output. This can be done over a logging system, stdio, UART, via a
// photodiode and modulated kazoo, or basically any way to get data in and out
// of an application.
//
// This facade doesn't dictate any policies on input and output data encoding,
// format, or transmission protocol. It only requires that backends return a
// OkStatus() if the operation succeeds. Backends may provide useful error
// Status types, but depending on the implementation-specific Status values is
// NOT recommended. Since this facade provides a very vague I/O interface, it
// does NOT provide tests. Backends are expected to provide their own testing to
// validate correctness.
//
// The intent of this module for simplifying bringup or otherwise getting data
// in/out of a CPU in a way that is platform-agnostic. The interface is designed
// to be easy to understand. There's no initialization as part of this
// interface, there's no configuration, and the interface is no-frills WYSIWYG
// byte-by-byte i/o.
//
//
//          PLEASE DON'T BUILD PROJECTS ON TOP OF THIS INTERFACE.

#include <cstddef>
#include <cstring>
#include <string_view>

#include "pw_bytes/span.h"
#include "pw_status/status.h"
#include "pw_status/status_with_size.h"

namespace pw::sys_io {

// Read a single byte from the sys io backend.
// Implemented by: Backend
//
// This function will block until it either succeeds or fails to read a byte
// from the pw_sys_io backend.
//
// Returns OkStatus() - A byte was successfully read.
//         Status::ResourceExhausted() - if the underlying source vanished.
Status ReadByte(std::byte* dest);

// Read a single byte from the sys io backend, if available.
// Implemented by: Backend
//
// Returns OkStatus() - A byte was successfully read, and is in dest.
//         Status::Unavailable() - No byte is available to read; try later.
//         Status::Unimplemented() - Not supported on this target.
Status TryReadByte(std::byte* dest);

// Write a single byte out the sys io backend.
// Implemented by: Backend
//
// This function will block until it either succeeds or fails to write a byte
// out the pw_sys_io backend.
//
// Returns OkStatus() if a byte was successfully read.
Status WriteByte(std::byte b);

// Write a string out the sys io backend.
// Implemented by: Backend
//
// This function takes a null-terminated string and writes it out the sys io
// backend, adding any platform-specific newline character(s) (these are
// accounted for in the returned StatusWithSize).
//
// Return status is OkStatus() if all the bytes from the source string were
// successfully written. In all cases, the number of bytes successfully written
// are returned as part of the StatusWithSize.
StatusWithSize WriteLine(const std::string_view& s);

// Fill a byte span from the sys io backend using ReadByte().
// Implemented by: Facade
//
// This function is implemented by this facade and simply uses ReadByte() to
// read enough bytes to fill the destination span. If there's an error reading a
// byte, the read is aborted and the contents of the destination span are
// undefined. This function blocks until either an error occurs, or all bytes
// are successfully read from the backend's ReadByte() implementation.
//
// Return status is OkStatus() if the destination span was successfully
// filled. In all cases, the number of bytes successuflly read to the
// destination span are returned as part of the StatusWithSize.
StatusWithSize ReadBytes(ByteSpan dest);

// Write span of bytes out the sys io backend using WriteByte().
// Implemented by: Facade
//
// This function is implemented by this facade and simply writes the source
// contents using WriteByte(). If an error writing a byte is encountered, the
// write is aborted and the error status returned. This function blocks until
// either an error occurs, or all bytes are successfully read from the backend's
// WriteByte() implementation.
//
// Return status is OkStatus() if all the bytes from the source span were
// successfully written. In all cases, the number of bytes successfully written
// are returned as part of the StatusWithSize.
StatusWithSize WriteBytes(ConstByteSpan src);

}  // namespace pw::sys_io
