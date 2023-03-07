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

#include <limits>

#include "pw_log/proto/log.pwpb.h"
#include "pw_multisink/multisink.h"
#include "pw_status/status.h"

namespace pw::multisink {

// Uses MultiSink's unsafe iteration to dump the contents as a series of log
// entries. max_num_entries can be used to limit the dump to the most recent
// entries. This can be used to dump proto-encoded logs to a
// pw.snapshot.Snapshot.
Status UnsafeDumpMultiSinkLogs(
    MultiSink& sink,
    pw::log::LogEntries::StreamEncoder& encoder,
    size_t max_num_entries = std::numeric_limits<size_t>::max());

}  // namespace pw::multisink
