// Copyright 2022 The Pigweed Authors
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

// This namespace is NOT in pw::protobuf to allow filtering of symbols not
// intended to be reflected in the size report.
namespace pw::protobuf_size_report {

// Includes ambient glue-related code that is required to set up size reports.
// Include this in base size reports to prevent irrelevant symbols from showing
// up in the final diffs.
void BloatWithBase();

// Adds pw_protobuf's StreamEncoder and MemoryEncoders to the size report.
void BloatWithEncoder();

// Adds pw_protobuf's StreamDecoder to the size report. This does not include
// the memory-buffer-only Decoder class, as the implementation is very
// different.
void BloatWithStreamDecoder();

// Adds pw_protobuf's Decoder to the size report. This does not include the
// StreamDecoder class, as the implementation is very different.
void BloatWithDecoder();

// Adds pw_protobuf's table-based Message encoder to the size report in addition
// to the StreamEncoder/MemoryEncoder.
void BloatWithTableEncoder();

// Adds pw_protobuf's table-based Message decoder to the size report in addition
// to the StreamDecoder.
void BloatWithTableDecoder();

}  // namespace pw::protobuf_size_report
