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
// This files provides support to dump the trace buffer to the logging module.
#pragma once

#include "pw_status/status.h"

namespace pw {
namespace trace {

// Dumps the trace buffer to the log. The output format to the log is the
// base64-encoded buffer, split into lines of an implementation-defined length.
// The trace logs are surrounded by 'begin' and 'end' tags.
//
// Ex. Invoking PW_TRACE_INSTANT with 'test1' and 'test2', then calling this
// function would produce this in the output logs:
//
// [TRACE] begin
// [TRACE] data: BWdDMRoABWj52YMB
// [TRACE] end
pw::Status DumpTraceBufferToLog();

}  // namespace trace
}  // namespace pw
