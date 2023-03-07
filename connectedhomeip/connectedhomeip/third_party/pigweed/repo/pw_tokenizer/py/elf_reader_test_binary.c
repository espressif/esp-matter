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

// This file is used to build an ELF for elf_reader_test.py. It places data into
// sections for the tool to read.
//
// To generate this test ELF, build the target
// //pw_tokenizer:elf_reader_test_binary. This produces the test binary
// elf_reader_test_binary.elf.
const char message[] __attribute__((section(".test_section_1"), used)) =
    "You cannot pass";

const unsigned number __attribute__((section(".test_section_2"), used)) =
    0xfeedbeef;
