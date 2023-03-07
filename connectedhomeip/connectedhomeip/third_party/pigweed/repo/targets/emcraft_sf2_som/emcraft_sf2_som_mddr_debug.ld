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

/* This relatively simplified linker script will work with many ARMv7-M and
 * ARMv8-M cores that have on-board memory-mapped RAM and FLASH. For more
 * complex projects and devices, it's possible this linker script will not be
 * sufficient as-is.
 *
 * This linker script is likely not suitable for a project with a bootloader.
 */

/* Provide useful error messages when required configurations are not set. */
#ifndef PW_BOOT_VECTOR_TABLE_BEGIN
#error "PW_BOOT_VECTOR_TABLE_BEGIN is not defined, and is required to use pw_boot_cortex_m"
#endif  // PW_BOOT_VECTOR_TABLE_BEGIN

#ifndef PW_BOOT_VECTOR_TABLE_SIZE
#error "PW_BOOT_VECTOR_TABLE_SIZE is not defined, and is required to use pw_boot_cortex_m"
#endif  // PW_BOOT_VECTOR_TABLE_SIZE

#ifndef PW_BOOT_CODE_BEGIN
#error "PW_BOOT_CODE_BEGIN is not defined, and is required to use pw_boot_cortex_m"
#endif  // PW_BOOT_CODE_BEGIN

#ifndef PW_BOOT_CODE_SIZE
#error "PW_BOOT_CODE_SIZE is not defined, and is required to use pw_boot_cortex_m"
#endif  // PW_BOOT_CODE_SIZE

#ifndef PW_BOOT_RAM_BEGIN
#error "PW_BOOT_RAM_BEGIN is not defined, and is required to use pw_boot_cortex_m"
#endif  // PW_BOOT_RAM_BEGIN

#ifndef PW_BOOT_RAM_SIZE
#error "PW_BOOT_RAM_SIZE is not defined, and is required to use pw_boot_cortex_m"
#endif  // PW_BOOT_RAM_SIZE

#ifndef PW_BOOT_HEAP_SIZE
#error "PW_BOOT_HEAP_SIZE is not defined, and is required to use pw_boot_cortex_m"
#endif  // PW_BOOT_HEAP_SIZE

#ifndef PW_BOOT_MIN_STACK_SIZE
#error "PW_BOOT_MIN_STACK_SIZE is not defined, and is required to use pw_boot_cortex_m"
#endif  // PW_BOOT_MIN_STACK_SIZE


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

  /* Vector Table */
  VECTOR_TABLE(rx) : \
    ORIGIN = PW_BOOT_VECTOR_TABLE_BEGIN, \
    LENGTH = PW_BOOT_VECTOR_TABLE_SIZE
  /* External RAM being used for code. */
  TEXT_EXTERNAL_RAM(rx) : \
    ORIGIN = PW_BOOT_CODE_BEGIN, \
    LENGTH = PW_BOOT_CODE_SIZE
  /* External DDR RAM */
  EXTERNAL_RAM(rwx) : \
    ORIGIN = PW_BOOT_RAM_BEGIN, \
    LENGTH = PW_BOOT_RAM_SIZE

  /* Each memory region above has an associated .*.unused_space section that
   * overlays the unused space at the end of the memory segment. These segments
   * are used by pw_bloat.bloaty_config to create the utilization data source
   * for bloaty size reports.
   *
   * These sections MUST be located immediately after the last section that is
   * placed in the respective memory region or lld will issue a warning like:
   *
   *   warning: ignoring memory region assignment for non-allocatable section
   *      '.VECTOR_TABLE.unused_space'
   *
   * If this warning occurs, it's also likely that LLD will have created quite
   * large padded regions in the ELF file due to bad cursor operations. This
   * can cause ELF files to balloon from hundreds of kilobytes to hundreds of
   * megabytes.
   *
   * Attempting to add sections to the memory region AFTER the unused_space
   * section will cause the region to overflow.
   */
}

SECTIONS
{
  /* This is the link-time vector table. If used, the VTOR (Vector Table Offset
   * Register) MUST point to this memory location in order to be used. This can
   * be done by ensuring this section exists at the default location of the VTOR
   * so it's used on reset, or by explicitly setting the VTOR in a bootloader
   * manually to point to &pw_boot_vector_table_addr before interrupts are
   * enabled.
   *
   * The ARMv7-M architecture requires this is at least aligned to 128 bytes,
   * and aligned to a power of two that is greater than 4 times the number of
   * supported exceptions. 512 has been selected as it accommodates this
   * device's vector table.
   */
  .vector_table : ALIGN(512)
  {
    pw_boot_vector_table_addr = .;
    KEEP(*(.vector_table))
  } >VECTOR_TABLE

  /* Represents unused space in the VECTOR_TABLE segment. This MUST be the last
   * section assigned to the VECTOR_TABLE region.
   */
  .VECTOR_TABLE.unused_space (NOLOAD) : ALIGN(4)
  {
    . = ABSOLUTE(ORIGIN(VECTOR_TABLE) + LENGTH(VECTOR_TABLE));
  } >VECTOR_TABLE

  /* Main executable code. */
  .code : ALIGN(4)
  {
   CREATE_OBJECT_SYMBOLS
    __text_load = LOADADDR(.code);
    /* Application code. */
    *(.text)
    *(.text*)
    KEEP(*(.init))
    KEEP(*(.fini))

    . = ALIGN(4);
    /* Constants.*/
    *(.rodata)
    *(.rodata*)

    /* .preinit_array, .init_array, .fini_array are used by libc.
     * Each section is a list of function pointers that are called pre-main and
     * post-exit for object initialization and tear-down.
     * Since the region isn't explicitly referenced, specify KEEP to prevent
     * link-time garbage collection. SORT is used for sections that have strict
     * init/de-init ordering requirements. */
    . = ALIGN(4);
    PROVIDE_HIDDEN(__preinit_array_start = .);
    KEEP(*(.preinit_array*))
    PROVIDE_HIDDEN(__preinit_array_end = .);

    PROVIDE_HIDDEN(__init_array_start = .);
    KEEP(*(SORT(.init_array.*)))
    KEEP(*(.init_array*))
    PROVIDE_HIDDEN(__init_array_end = .);

    PROVIDE_HIDDEN(__fini_array_start = .);
    KEEP(*(SORT(.fini_array.*)))
    KEEP(*(.fini_array*))
    PROVIDE_HIDDEN(__fini_array_end = .);
  } >TEXT_EXTERNAL_RAM

  /* Used by unwind-arm/ */
  .ARM : ALIGN(4) {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >TEXT_EXTERNAL_RAM

  /* Explicitly initialized global and static data. (.data)*/
  .static_init_ram : ALIGN(4)
  {
    *(.data)
    *(.data*)
    . = ALIGN(4);
  } >EXTERNAL_RAM AT> TEXT_EXTERNAL_RAM

  /* Represents unused space in the TEXT_EXTERNAL_RAM segment. This MUST be the
   * last section assigned to the TEXT_EXTERNAL_RAM region.
   */
  .TEXT_EXTERNAL_RAM.unused_space (NOLOAD) : ALIGN(4)
  {
    . = ABSOLUTE(ORIGIN(TEXT_EXTERNAL_RAM) + LENGTH(TEXT_EXTERNAL_RAM));
  } >TEXT_EXTERNAL_RAM

  /* The .zero_init_ram, .heap, and .stack sections below require (NOLOAD)
   * annotations for LLVM lld, but not GNU ld, because LLVM's lld intentionally
   * interprets the linker file differently from ld:
   *
   * https://discourse.llvm.org/t/lld-vs-ld-section-type-progbits-vs-nobits/5999/3
   *
   * Zero initialized global/static data (.bss) is initialized in
   * pw_boot_Entry() via memset(), so the section doesn't need to be loaded from
   * flash. The .heap and .stack sections don't require any initialization,
   * as they only represent allocated memory regions, so they also do not need
   * to be loaded.
   */
  .zero_init_ram (NOLOAD) : ALIGN(4)
  {
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
  } >EXTERNAL_RAM

  .heap (NOLOAD) : ALIGN(4)
  {
    pw_boot_heap_low_addr = .;
    . = . + PW_BOOT_HEAP_SIZE;
    . = ALIGN(4);
    pw_boot_heap_high_addr = .;
  } >EXTERNAL_RAM

  /* Link-time check for stack overlaps.
   *
   * The ARMv7-M architecture may require 8-byte alignment of the stack pointer
   * rather than 4 in some contexts and implementations, so this region is
   * 8-byte aligned (see ARMv7-M Architecture Reference Manual DDI0403E
   * section B1.5.7).
   */
  .stack (NOLOAD) : ALIGN(8)
  {
    /* Set the address that the main stack pointer should be initialized to. */
    pw_boot_stack_low_addr = .;
    HIDDEN(_stack_size = ORIGIN(EXTERNAL_RAM) + LENGTH(EXTERNAL_RAM) - .);
    /* Align the stack to a lower address to ensure it isn't out of range. */
    HIDDEN(_stack_high = (. + _stack_size) & ~0x7);
    ASSERT(_stack_high - . >= PW_BOOT_MIN_STACK_SIZE,
           "Error: Not enough RAM for desired minimum stack size.");
    . = _stack_high;
    pw_boot_stack_high_addr = .;
  } >EXTERNAL_RAM

  /* Represents unused space in the EXTERNAL_RAM segment. This MUST be the last
   * section assigned to the EXTERNAL_RAM region.
   */
  .EXTERNAL_RAM.unused_space (NOLOAD) : ALIGN(4)
  {
    . = ABSOLUTE(ORIGIN(EXTERNAL_RAM) + LENGTH(EXTERNAL_RAM));
  } >EXTERNAL_RAM

  /* Discard unwind info. */
  .ARM.extab 0x0 (INFO) :
  {
    KEEP(*(.ARM.extab*))
  }
}

/* Symbols used by core_init.c: */
/* Start of .static_init_ram in TEXT_EXTERNAL_RAM. */
_pw_static_init_flash_start = LOADADDR(.static_init_ram);

/* Region of .static_init_ram in RAM. */
_pw_static_init_ram_start = ADDR(.static_init_ram);
_pw_static_init_ram_end = _pw_static_init_ram_start + SIZEOF(.static_init_ram);

/* Region of .zero_init_ram. */
_pw_zero_init_ram_start = ADDR(.zero_init_ram);
_pw_zero_init_ram_end = _pw_zero_init_ram_start + SIZEOF(.zero_init_ram);

/* arm-none-eabi expects `end` symbol to point to start of heap for sbrk. */
PROVIDE(end = _pw_zero_init_ram_end);

/* These symbols are used by pw_bloat.bloaty_config to create the memoryregions
 * data source for bloaty in this format (where the optional _N defaults to 0):
 * pw_bloat_config_memory_region_NAME_{start,end}{_N,} */
pw_bloat_config_memory_region_VECTOR_TABLE_start = ORIGIN(VECTOR_TABLE);
pw_bloat_config_memory_region_VECTOR_TABLE_end =
    ORIGIN(VECTOR_TABLE) + LENGTH(VECTOR_TABLE);
pw_bloat_config_memory_region_FLASH_start = ORIGIN(TEXT_EXTERNAL_RAM);
pw_bloat_config_memory_region_FLASH_end = ORIGIN(TEXT_EXTERNAL_RAM) + LENGTH(TEXT_EXTERNAL_RAM);
pw_bloat_config_memory_region_RAM_start = ORIGIN(EXTERNAL_RAM);
pw_bloat_config_memory_region_RAM_end = ORIGIN(EXTERNAL_RAM) + LENGTH(EXTERNAL_RAM);

/* Symbol mapping used by MSS Startup and Debugger. */
PROVIDE (__smartfusion2_memory_remap = 2);  /* Remap according to LMA (this script) */
PROVIDE (__mirrored_nvm = 1);   /* GDB will load to LMA directly no need to load again. */
PROVIDE (_estack = pw_boot_stack_high_addr);
PROVIDE (__stack_start__ = pw_boot_stack_low_addr);
PROVIDE (__vector_table_load = LOADADDR(.vector_table));
PROVIDE (__vector_table_start = pw_boot_vector_table_addr);
PROVIDE (__vector_table_vma_base_address = __vector_table_start);  /* required by debugger for start address */
PROVIDE (__text_end =  __exidx_end);
PROVIDE (__heap_start__ = pw_boot_heap_low_addr);
PROVIDE (_eheap = pw_boot_heap_high_addr);
PROVIDE (_etext =  __exidx_end);
PROVIDE (_evector_table = pw_boot_vector_table_addr);
PROVIDE (__data_start = _pw_static_init_ram_start);
PROVIDE (_edata = _pw_static_init_ram_end);
PROVIDE (__text_start = __text_load);
PROVIDE (__bss_start__ = _pw_zero_init_ram_start);
PROVIDE (__bss_end__ = _pw_static_init_ram_end);
PROVIDE (__data_load = _pw_static_init_flash_start);
