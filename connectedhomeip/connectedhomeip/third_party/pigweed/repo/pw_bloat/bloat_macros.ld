/*
 * Copyright 2022 The Pigweed Authors
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

/* Helper macros to define the symbols requires by pw_bloat to detect the
 * utilization and memory regions of your program.
 *
 * Include this file into your pw_linker_script() as follows:
 *
 *   pw_linker_script("my_linker_script") {
 *     includes = [ "$dir_pw_bloat/bloat_macros.ld" ]
 *     linker_script = "my_project_linker_script.ld"
 *   }
 */

/* Default alignment used when declaring new sections. In most cases the free
 * space should be measured down to a multiple of 4 bytes, but this can be
 * changed in needed. */
#ifndef PW_BLOAT_SECTION_ALIGN
#define PW_BLOAT_SECTION_ALIGN 4
#endif

/* Declares an unused_space section. Instantiate this macro from within the
 * SECTIONS of your linker script, after every other section, for example:
 *   PW_BLOAT_UNUSED_SPACE_SECTION(FLASH)
 *   PW_BLOAT_UNUSED_SPACE_SECTION(RAM)
 */
#define PW_BLOAT_UNUSED_SPACE_SECTION(memory_region)                  \
  .memory_region.unused_space(NOLOAD) : ALIGN(PW_BLOAT_SECTION_ALIGN) \
  {                                                                   \
    . = ABSOLUTE(ORIGIN(memory_region) + LENGTH(memory_region));      \
  } > memory_region

/* Declares a memory region in pw_bloat mapped to the same name. Example:
 *   PW_BLOAT_MEMORY_REGION(FLASH)
 */
#define PW_BLOAT_MEMORY_REGION(memory_region) \
  PW_BLOAT_MEMORY_REGION_MAP(memory_region, memory_region)

/* Declares a memory region in pw_bloat mapped to a different name. Can be used
 * multiple times to map multiple aliased memory regions to the same name.
 *   PW_BLOAT_MEMORY_REGION_MAP(RAM, ITCM)
 *   PW_BLOAT_MEMORY_REGION_MAP(RAM, DTCM)
 */
#define PW_BLOAT_MEMORY_REGION_MAP(name, memory_region) \
  PW_BLOAT_MEMORY_REGION_MAP_N(name, memory_region, __COUNTER__)

/* Alternative version of PW_BLOAT_MEMORY_REGION_MAP to also specify the index
 * value. This index value is irrelevant for pw_bloat tools as long as it
 * doesn't repeat for the same name. Use this macro if you need to specify the
 * index value for other tools.
 * Note: this uses two macros to expand __COUNTER__ in
 *       PW_BLOAT_MEMORY_REGION_MAP. */
#define PW_BLOAT_MEMORY_REGION_MAP_N(name, memory_region, index) \
  _PW_BLOAT_MEMORY_REGION_MAP_N(name, memory_region, index)

#define _PW_BLOAT_MEMORY_REGION_MAP_N(name, memory_region, index) \
  pw_bloat_config_memory_region_##name##_start_##index =          \
      ORIGIN(memory_region);                                      \
  pw_bloat_config_memory_region_##name##_end_##index =            \
      ORIGIN(memory_region) + LENGTH(memory_region);
