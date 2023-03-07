/***************************************************************************//**
 * @file
 * @brief Ember bootloader common definitions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "ember_btl_interface.h"
#include "sl_ember_btl_device_info.h"

#define AAT_VERSION                     0x010A
#define APP_ADDRESS_TABLE_TYPE          0x0AA7
#define SOFTWARE_VERSION                3000
#define EMBER_BUILD_NUMBER              0
#ifndef CUSTOMER_APPLICATION_VERSION
  #define CUSTOMER_APPLICATION_VERSION  0
#endif

#define __NO_INIT__                         ".noinit"
#define __INTVEC__                          ".intvec"
#define __STACK__                           ".stack"
#define __BSS__                             ".bss"
#define __TEXT__                            ".text"
#define __HEAP__                            ".heap"
#if defined(__GNUC__)
#define __AAT__                             ".aat"
#define __INTERNAL_STORAGE__                ".internal_storage"
#define __SIMEE__                           ".simee"
#elif defined(__ICCARM__)
#define __AAT__                             "AAT"
#define __INTERNAL_STORAGE__                "INTERNAL_STORAGE"
#define __SIMEE__                           "SIMEE"
#endif

// GCC specific macros
#if defined(__GNUC__)

#define VAR_AT_SEGMENT(__variableDeclaration, __segmentName) \
  __variableDeclaration __attribute__ ((section(__segmentName)))

#define NO_STRIPPING __attribute__((used))

extern uint32_t __StackTop, __StackLimit;
extern uint32_t linker_code_begin, linker_code_end;
extern uint32_t __NO_INIT__begin, __NO_INIT__end;
extern uint32_t linker_storage_begin, linker_storage_end;
extern uint32_t __bss_start__, __bss_end__;
extern uint32_t linker_nvm_begin, linker_nvm_end;
extern uint32_t __HeapBase, __HeapLimit;

#define _STACK_SEGMENT_BEGIN            (&__StackLimit)
#define _TEXT_SEGMENT_BEGIN             (&linker_code_begin)
#define _NO_INIT_SEGMENT_BEGIN          (&__NO_INIT__begin)
#define _INTERNAL_STORAGE_BEGIN         (&linker_storage_begin)
#define _BSS_SEGMENT_BEGIN              (&__bss_start__)
#define _SIMEE_SEGMENT_BEGIN            (&linker_nvm_begin)
#define _HEAP_SEGMENT_BEGIN             (&__HeapBase)

#define _STACK_SEGMENT_END              (&__StackTop)
#define _TEXT_SEGMENT_END               (&linker_code_end)
#define _NO_INIT_SEGMENT_END            (&__NO_INIT__end)
#define _INTERNAL_STORAGE_END           (&linker_storage_end)
#define _BSS_SEGMENT_END                (&__bss_end__)
#define _SIMEE_SEGMENT_END              (&linker_nvm_end)
#define _HEAP_SEGMENT_END               (&__HeapLimit)

#endif // __GNUC__

// IAR Specific macros
#if defined(__ICCARM__)

#define VAR_AT_SEGMENT(__variableDeclaration, __segmentName) \
  __variableDeclaration @ __segmentName

#define NO_STRIPPING  __root

#pragma segment=__STACK__
#pragma segment=__TEXT__
#pragma segment=__NO_INIT__
#pragma segment=__INTERNAL_STORAGE__
#pragma segment=__BSS__
#pragma segment=__SIMEE__
#pragma segment=__HEAP__
#pragma segment=__AAT__

#define _STACK_SEGMENT_BEGIN            __segment_begin(__STACK__)
#define _TEXT_SEGMENT_BEGIN             __segment_begin(__TEXT__)
#define _NO_INIT_SEGMENT_BEGIN          __segment_begin(__NO_INIT__)
#define _INTERNAL_STORAGE_BEGIN         __segment_begin(__INTERNAL_STORAGE__)
#define _BSS_SEGMENT_BEGIN              __segment_begin(__BSS__)
#define _SIMEE_SEGMENT_BEGIN            __segment_begin(__SIMEE__)
#define _HEAP_SEGMENT_BEGIN             __segment_begin(__HEAP__)

#define _STACK_SEGMENT_END              __segment_end(__STACK__)
#define _TEXT_SEGMENT_END               __segment_end(__TEXT__)
#define _NO_INIT_SEGMENT_END            __segment_end(__NO_INIT__)
#define _INTERNAL_STORAGE_END           __segment_end(__INTERNAL_STORAGE__)
#define _BSS_SEGMENT_END                __segment_end(__BSS__)
#define _SIMEE_SEGMENT_END              __segment_end(__SIMEE__)
#define _HEAP_SEGMENT_END               __segment_end(__HEAP__)

#endif // __ICCARM__

const BootloaderAddressTable_t *bootloaderAddressTable = (BootloaderAddressTable_t*)(FLASH_BASE);

VAR_AT_SEGMENT(NO_STRIPPING const AppAddressTable_t appAddressTable, __AAT__) = {
  { _STACK_SEGMENT_END,
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    APP_ADDRESS_TABLE_TYPE,
    AAT_VERSION,
    __Vectors },
  SL_PLAT,
  SL_MICRO,
  SL_PHY,
  sizeof(AppAddressTable_t),
  SOFTWARE_VERSION,
  EMBER_BUILD_NUMBER,
  0,
  "",
  0,
  { 0 },
  0,
  { { 0xFF, 0xFF },
    { 0xFF, 0xFF },
    { 0xFF, 0xFF },
    { 0xFF, 0xFF },
    { 0xFF, 0xFF },
    { 0xFF, 0xFF }, },
  _SIMEE_SEGMENT_BEGIN,
  CUSTOMER_APPLICATION_VERSION,
  _INTERNAL_STORAGE_BEGIN,
  { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0XFF },
  SL_FAMILY,
  { 0 },
  0,
  _NO_INIT_SEGMENT_BEGIN,
  0,
  _BSS_SEGMENT_END,
  _STACK_SEGMENT_END,
  0,
  _TEXT_SEGMENT_END,
  _STACK_SEGMENT_BEGIN,
  _HEAP_SEGMENT_END,
  _SIMEE_SEGMENT_END,
  0
};
