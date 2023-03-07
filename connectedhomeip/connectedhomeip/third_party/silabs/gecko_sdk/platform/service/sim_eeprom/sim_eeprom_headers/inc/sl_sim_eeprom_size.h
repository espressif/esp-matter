/***************************************************************************//**
 * @file
 * @brief File to create defines for the size of the sim_eeprom
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef __SIM_EEPROM_SIZE_H__
#define __SIM_EEPROM_SIZE_H__

#ifndef DOXYGEN_SHOULD_SKIP_THIS

//Fundamental limitations of the Simulated EEPROM design:
//These limitations are forcefully enforced via compile-time errors
//in the declarations that follow because exceeding any of them will
//cause fatal operation and data corruption.
//(1) The maximum number of tokens that can be stored is 254.
//    (Tokens are identified by one-byte values, with 0xFF as invalid.)
//(2) The maximum size, in bytes, of a token (element) is 254.
//    (One byte, with 0xFF as invalid)
//(3) The maximum number of elements in an indexed token is 126.
//    (One byte, with MSB used as counter flag and 0x7F as non-indexed size.)
//(4) The maximum total storage for tokens plus management is SIMEE_BTS_SIZE_B/2.
//    (This limit is due to a Virtual Page only being a limited size and for
//     the Simulated EEPROM to operate with any kind of efficiency, the
//     Base Storage must be small enough to leave room for token copies.)

//The default choice is an 8kByte SimEE1.  Defining
//EMBER_SIM_EEPROM_4KB in your build or configuration header
//will still use SimEE1, but consume 4kB of flash for the SimEE.
//Both choices of 8kByte and 4kByte of SimEE size allow for a maximum of
//2kByte of data+mgmt.

//Verify that the application does not specify a SIMEE1 size when trying to use SIMEE2 or NVM3.
#if defined(USE_SIMEE2) || defined(USE_NVM3) || defined(SIMEE2_TO_NVM3_UPGRADE)
  #if defined(EMBER_SIMEE1_2KB) || defined(EMBER_SIM_EEPROM_2KB) \
  || defined(EMBER_SIMEE1_4KB) || defined(EMBER_SIM_EEPROM_4KB)
    #error Invalid SimEEPROM configuration. SIMEE1 size specified when using SIMEE2 or NVM3.
  #endif
#endif

#if defined(EMBER_SIMEE1_2KB) || defined(EMBER_SIM_EEPROM_2KB)
  #define SIMEE_SIZE_B        2048  //Use a 2k 8bit SimEE1
  #define SIMEE_BTS_SIZE_B    1024
#elif defined(EMBER_SIMEE1_4KB) || defined(EMBER_SIM_EEPROM_4KB)
  #define SIMEE_SIZE_B        4096  //Use a 4k 8bit SimEE1
  #define SIMEE_BTS_SIZE_B    2048
#elif defined(USE_SIMEE2)
//NOTE: USE_SIMEE2 defaults to 36k
//Defining USE_SIMEE2 in your build or configuration header
//will consume 36kB of flash for the SimEE and allows for a maximum
//of 8kByte of data+mgmt.
  #define SIMEE_SIZE_B        36864 //Use a 36k 8bit SimEE2
  #define SIMEE_BTS_SIZE_B    8192
#elif defined(SIMEE2_TO_NVM3_UPGRADE)
//Defining SIMEE2_TO_NVM3_UPGRADE in your build or configuration header
//will consume 36kB of flash for the SimEE2 data and allows for a maximum
//of 8kByte of data+mgmt.
  #define SIMEE_SIZE_B        36864 //Use a 36k 8bit NVM3
  #define SIMEE_BTS_SIZE_B    8192
#else //EMBER_SIMEE1 || EMBER_SIMEE1_8KB
//NOTE: EMBER_SIMEE1 defaults to 8k
  #define SIMEE_SIZE_B        8192  //Use a 8k 8bit SimEE1
  #define SIMEE_BTS_SIZE_B    2104
#endif
#ifndef MFB_PAGE_SIZE_B
  #define MFB_PAGE_SIZE_B 2048u
#endif // MFB_PAGE_SIZE_B
#ifndef MFB_PAGE_SIZE_W
  #define MFB_PAGE_SIZE_W     (MFB_PAGE_SIZE_B / 4)
#endif // MFB_PAGE_SIZE_W
//NOTE: SIMEE_SIZE_B size must be a precise multiple of the page size.
#if ((SIMEE_SIZE_B % MFB_PAGE_SIZE_B) != 0)
  #error Illegal SimEE storage size.  SIMEE_SIZE_B must be a multiple of the flash page size.
#endif

//Convenience define to provide the SimEE size in 16bits.
#define SIMEE_SIZE_HW       (SIMEE_SIZE_B / 2)
//Convenience define to provide the flash page size in 16bits.
#define FLASH_PAGE_SIZE_HW  (MFB_PAGE_SIZE_W * 2) //derived from a chip's memmap.h
//Convenience define to provide the maximum size for a token
#define SIMEE_MAX_TOKEN_SIZE (254)

#endif //DOXYGEN_SHOULD_SKIP_THIS

#endif //__SIM_EEPROM_SIZE_H__
