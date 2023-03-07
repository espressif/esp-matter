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

#include "pw_multisink/util.h"

#include "pw_bytes/span.h"
#include "pw_function/function.h"
#include "pw_multisink/multisink.h"
#include "pw_status/status.h"

namespace pw::multisink {

Status UnsafeDumpMultiSinkLogs(MultiSink& sink,
                               pw::log::LogEntries::StreamEncoder& encoder,
                               size_t max_num_entries) {
  auto callback = [&encoder](ConstByteSpan entry) {
    encoder
        .WriteBytes(static_cast<uint32_t>(pw::log::LogEntries::Fields::ENTRIES),
                    entry)
        .IgnoreError();
  };
  return sink.UnsafeForEachEntry(callback, max_num_entries);
}

}  // namespace pw::multisink
