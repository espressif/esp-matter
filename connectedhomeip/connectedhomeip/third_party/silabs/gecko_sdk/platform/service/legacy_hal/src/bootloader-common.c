/***************************************************************************//**
 * @file
 * @brief Common bootloader definitions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include PLATFORM_HEADER

#ifdef _SILICON_LABS_32B_SERIES_1_CONFIG_1

#include "bootloader-common.h"
#include "sl_legacy_bootloader_device_info.h"
#include "api/btl_interface.h"

#define AAT_VERSION                     0x010A
#define APP_ADDRESS_TABLE_TYPE          0x0AA7
#define SOFTWARE_VERSION                3000
#define EMBER_BUILD_NUMBER              0
#ifndef CUSTOMER_APPLICATION_VERSION
  #define CUSTOMER_APPLICATION_VERSION 0
#endif

const HalBootloaderAddressTableType *halBootloaderAddressTable = (HalBootloaderAddressTableType*)(FLASH_BASE);

extern const ApplicationProperties_t appProperties;
extern const tVectorEntry __VECTOR_TABLE[];

VAR_AT_SEGMENT(NO_STRIPPING const HalAppAddressTableType halAppAddressTable, __AAT__) = {
  { _CSTACK_SEGMENT_END,
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    APP_ADDRESS_TABLE_TYPE,
    AAT_VERSION,
    __VECTOR_TABLE },
  SL_PLAT,
  SL_MICRO,
  SL_PHY,
  sizeof(HalAppAddressTableType),  // size of aat itself
  SOFTWARE_VERSION,   // uint16_t softwareVersion
  EMBER_BUILD_NUMBER, // uint16_t softwareBuild
  0,  //uint32_t timestamp; // Unix epoch time of .ebl file, filled in by ebl gen
  "", //uint8_t imageInfo[IMAGE_INFO_MAXLEN];  // string, filled in by ebl generation
  &appProperties,
  { 0 }, //uint8_t reserved[] (zero fill)
  0,  //uint32_t imageCrc;  // CRC over following pageRanges, filled in by ebl gen
  { { 0xFF, 0xFF },
    { 0xFF, 0xFF },
    { 0xFF, 0xFF },
    { 0xFF, 0xFF },
    { 0xFF, 0xFF },
    { 0xFF, 0xFF }, },
  _SIMEE_SEGMENT_BEGIN,                               //void *simeeBottom;
  CUSTOMER_APPLICATION_VERSION,                       //uint32_t customerApplicationVersion;
  _INTERNAL_STORAGE_BEGIN,                            //void *internalStorageBottom;
  { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0XFF }, // image stamp (filled in by em3xx_convert)
  SL_FAMILY,
  { 0 },                                              //uint8_t bootloaderReserved[] (zero fill, previously was 0xFF fill)
  0,
  _NO_INIT_SEGMENT_BEGIN,                             //void *noInitBottom;
  0,
  _BSS_SEGMENT_END,                                   //void *globalTop;
  _CSTACK_SEGMENT_END,                                //void *cstackTop;
  0,
  _TEXT_SEGMENT_END,                                  //void *codeTop;
  _CSTACK_SEGMENT_BEGIN,                              //void *cstackBottom;
  0,
  _SIMEE_SEGMENT_END,                                 //void *simeeTop;
  0
};

#endif
