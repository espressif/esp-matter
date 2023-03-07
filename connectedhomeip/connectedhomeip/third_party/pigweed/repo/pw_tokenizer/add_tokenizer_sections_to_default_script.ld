/*
 * Copyright 2020 The Pigweed Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

INCLUDE pw_tokenizer_linker_sections.ld

/*
 * The INSERT directive instructs the linker to append the directives in this
 * script to the default linker script, rather than replace the default with
 * this script. It doesn't matter where the tokenizer sections are inserted, so
 * insert them after the .debug_info section, which both clang and GCC use.
 */
INSERT AFTER .debug_info
