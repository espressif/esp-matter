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

#include <cstddef>

#include "pw_bytes/span.h"
#include "pw_polyfill/language_feature_macros.h"
#include "pw_span/span.h"
#include "pw_status/status.h"
#include "pw_status/status_with_size.h"
#include "pw_stream/stream.h"

namespace pw::stream {

// Stream that silently drops written data and returns nothing on reads, similar
// to /dev/null.
class NullStream final : public SeekableReaderWriter {
 public:
  // Gives access to a global NullStream instance. It is not necessary to have
  // multiple NullStream instances since they have no state and do nothing.
  static NullStream& Instance() {
    PW_CONSTINIT static NullStream stream;
    return stream;
  }

 private:
  Status DoWrite(ConstByteSpan) final { return OkStatus(); }
  StatusWithSize DoRead(ByteSpan) final { return StatusWithSize::OutOfRange(); }
  Status DoSeek(ptrdiff_t, Whence) final { return OkStatus(); }
};

}  // namespace pw::stream
