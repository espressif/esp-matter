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

/* This linker script is for test purposes only and should not be used in a
 * production environment.
*/
#ifndef PW_BOOT_VECTOR_TABLE_BEGIN
#error "PW_BOOT_VECTOR_TABLE_BEGIN is not defined, and is required to use pw_boot_cortex_m"
#endif  // PW_BOOT_VECTOR_TABLE_BEGIN


 /* Note: This technically doesn't set the firmware's entry point. Setting the
 *       firmware entry point is done by setting vector_table[1]
 *       (Reset_Handler). However, this DOES tell the compiler how to optimize
 *       when --gc-sections is enabled.
 */
ENTRY(pw_boot_Entry)

MEMORY
{
  /* TODO(b/234892223): Make it possible for projects to freely customize
   * memory regions.
   */

  /* Vector Table (typically in flash) */
  VECTOR_TABLE(rx) : \
    ORIGIN = PW_BOOT_VECTOR_TABLE_BEGIN, \
    LENGTH = PW_BOOT_VECTOR_TABLE_SIZE
  /* Internal Flash */
  FLASH(rx) : \
    ORIGIN = PW_BOOT_FLASH_BEGIN, \
    LENGTH = PW_BOOT_FLASH_SIZE
  /* Internal SRAM */
  RAM(rwx) : \
    ORIGIN = PW_BOOT_RAM_BEGIN, \
    LENGTH = PW_BOOT_RAM_SIZE
}

