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
//==============================================================================
//
// This file provides an optional trace buffer which can be used with the
#pragma once

#include "pw_bytes/span.h"
#include "pw_ring_buffer/prefixed_entry_ring_buffer.h"
#include "pw_trace_tokenized/config.h"
#include "pw_trace_tokenized/trace_tokenized.h"
#include "pw_varint/varint.h"

namespace pw {
namespace trace {

// pw_TraceClearBuffer resets the trace buffer, and all data currently stored
// in the buffer is lost.
void ClearBuffer();

// Get the ring buffer which contains the data.
pw::ring_buffer::PrefixedEntryRingBuffer* GetBuffer();

// View underlying buffer trace_tokenized provided ring_buffer at time of
// construction. This allows for bulk access to the trace events buffer. Since
// this also derings the underlying ring_buffer, ensure that tracing is disabled
// when calling this function.
ConstByteSpan DeringAndViewRawBuffer();

}  // namespace trace
}  // namespace pw
