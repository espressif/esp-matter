/*
 * Copyright 2021 The Pigweed Authors
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

/* This linker script is derived from pw_boot_cortex_m/basic_cortex_m.ld for use
 * with the NXP MIMXRT595-EVK, booting from FLASH.
 */

/* Provide useful error messages when required configurations are not set. */
#ifndef PW_BOOT_VECTOR_TABLE_BEGIN
#error "PW_BOOT_VECTOR_TABLE_BEGIN is not defined, and is required to use pw_boot_cortex_m"
#endif  // PW_BOOT_VECTOR_TABLE_BEGIN

#ifndef PW_BOOT_VECTOR_TABLE_SIZE
#error "PW_BOOT_VECTOR_TABLE_SIZE is not defined, and is required to use pw_boot_cortex_m"
#endif  // PW_BOOT_VECTOR_TABLE_SIZE

#ifndef PW_BOOT_FLASH_BEGIN
#error "PW_BOOT_FLASH_BEGIN is not defined, and is required to use pw_boot_cortex_m"
#endif  // PW_BOOT_FLASH_BEGIN

#ifndef PW_BOOT_FLASH_SIZE
#error "PW_BOOT_FLASH_SIZE is not defined, and is required to use pw_boot_cortex_m"
#endif  // PW_BOOT_FLASH_SIZE

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
  /* Flash Config for bootloader */
  FLASH_CONFIG(rx) : \
    ORIGIN = 0x08000400, \
    LENGTH = 0x00000200
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
  /* USB SRAM */
  USB_SRAM(rw) : \
    ORIGIN = 0x40140000, \
    LENGTH = 0x00004000

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
  .flash_config :
  {
    . = ALIGN(4);
    KEEP(*(.flash_conf))
  } >FLASH_CONFIG

  /* This is the link-time vector table. If used, the VTOR (Vector Table Offset
   * Register) MUST point to this memory location in order to be used. This can
   * be done by ensuring this section exists at the default location of the VTOR
   * so it's used on reset, or by explicitly setting the VTOR in a bootloader
   * manually to point to &pw_boot_vector_table_addr before interrupts are
   * enabled.
   *
   * The ARMv8-M architecture requires this is at least aligned to 128 bytes,
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
    . = ALIGN(4);
    /* Application code. */
    *(.text)
    *(.text*)
    KEEP(*(.init))
    KEEP(*(.fini))

    . = ALIGN(4);
    /* Constants.*/
    *(.rodata)
    *(.rodata*)

    /* Glue ARM to Thumb code, and vice-versa */
    *(.glue_7)
    *(.glue_7t)

    /* Exception handling frame */
    *(.eh_frame)

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
  } >FLASH

  /* Used by unwind-arm/ */
  .ARM : ALIGN(4) {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >FLASH

  /* Explicitly initialized global and static data. (.data)*/
  .static_init_ram : ALIGN(4)
  {
    *(CodeQuickAccess)
    *(DataQuickAccess)
    *(.data)
    *(.data*)
    . = ALIGN(4);
  } >RAM AT> FLASH

  /* Represents unused space in the FLASH segment. This MUST be the last section
   * assigned to the FLASH region.
   */
  .FLASH.unused_space (NOLOAD) : ALIGN(4)
  {
    . = ABSOLUTE(ORIGIN(FLASH) + LENGTH(FLASH));
  } >FLASH

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
  } >RAM

  .heap (NOLOAD) : ALIGN(4)
  {
    pw_boot_heap_low_addr = .;
    . = . + PW_BOOT_HEAP_SIZE;
    . = ALIGN(4);
    pw_boot_heap_high_addr = .;
  } >RAM

  /* Link-time check for stack overlaps.
   *
   * The ARMv8-M architecture requires 8-byte alignment of the stack pointer
   * rather than 4 in some contexts, so this region is 8-byte aligned (see
   * ARMv8-M Architecture Reference Manual DDI0553 section B3.8).
   */
  .stack (NOLOAD) : ALIGN(8)
  {
    /* Set the address that the main stack pointer should be initialized to. */
    pw_boot_stack_low_addr = .;
    HIDDEN(_stack_size = ORIGIN(RAM) + LENGTH(RAM) - .);
    /* Align the stack to a lower address to ensure it isn't out of range. */
    HIDDEN(_stack_high = (. + _stack_size) & ~0x7);
    ASSERT(_stack_high - . >= PW_BOOT_MIN_STACK_SIZE,
           "Error: Not enough RAM for desired minimum stack size.");
    . = _stack_high;
    pw_boot_stack_high_addr = .;
  } >RAM

  /* Represents unused space in the RAM segment. This MUST be the last section
   * assigned to the RAM region.
   */
  .RAM.unused_space (NOLOAD) : ALIGN(4)
  {
    . = ABSOLUTE(ORIGIN(RAM) + LENGTH(RAM));
  } >RAM

  m_usb_bdt (NOLOAD) :
  {
    . = ALIGN(512);
    *(m_usb_bdt)
  } >USB_SRAM

  m_usb_global (NOLOAD) :
  {
    *(m_usb_global)
  } >USB_SRAM

  /* Represents unused space in the USB_SRAM segment. This MUST be the last
   * section assigned to the USB_SRAM region.
   */
  .USB_SRAM.unused_space (NOLOAD) : ALIGN(4)
  {
    . = ABSOLUTE(ORIGIN(USB_SRAM) + LENGTH(USB_SRAM));
  } >USB_SRAM

  /* Discard unwind info. */
  .ARM.extab 0x0 (INFO) :
  {
    KEEP(*(.ARM.extab*))
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}

/* Symbols used by core_init.c: */
/* Start of .static_init_ram in FLASH. */
_pw_static_init_flash_start = LOADADDR(.static_init_ram);

/* Region of .static_init_ram in RAM. */
_pw_static_init_ram_start = ADDR(.static_init_ram);
_pw_static_init_ram_end = _pw_static_init_ram_start + SIZEOF(.static_init_ram);

/* Region of .zero_init_ram. */
_pw_zero_init_ram_start = ADDR(.zero_init_ram);
_pw_zero_init_ram_end = _pw_zero_init_ram_start + SIZEOF(.zero_init_ram);

/* Size of image for bootloader header. */
_pw_image_size = _pw_static_init_flash_start + (_pw_static_init_ram_end - _pw_static_init_ram_start) - pw_boot_vector_table_addr;

/* arm-none-eabi expects `end` symbol to point to start of heap for sbrk. */
PROVIDE(end = _pw_zero_init_ram_end);
