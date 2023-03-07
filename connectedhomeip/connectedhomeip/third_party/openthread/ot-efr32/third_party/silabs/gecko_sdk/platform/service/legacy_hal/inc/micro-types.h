/***************************************************************************//**
 * @file
 * @brief This file handles defines and enums related to all the micros.
 * THIS IS A GENERATED FILE. DO NOT EDIT.
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

#ifndef __MICRO_DEFINES_H__
#define __MICRO_DEFINES_H__

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// Below is a list of PLATFORM and MICRO values that are used to define the
// chips our code runs on. These values are used in EBL headers, bootloader
// query response messages, and possibly other places in the code
// -----------------------------------------------------------------------------
// PLAT 1 was the AVR ATMega, no longer supported (still used for EMBER_TEST)

// PLAT 2 is the XAP2b
// for PLAT 2, MICRO 1 is the em250
// for PLAT 2, MICRO 2 is the em260

// PLAT 3 is the obsolete_3

// PLAT 4 is the CortexM3
// for PLAT 4, MICRO 1 is the em350
// for PLAT 4, MICRO 2 is the em360
// for PLAT 4, MICRO 3 is the em357
// for PLAT 4, MICRO 4 is the em367
// for PLAT 4, MICRO 5 is the em351
// for PLAT 4, MICRO 6 is the em35x
// for PLAT 4, MICRO 7 is the obsolete_cortexm3_7
// for PLAT 4, MICRO 8 is the em3588
// for PLAT 4, MICRO 9 is the em359
// for PLAT 4, MICRO 10 is the em3581
// for PLAT 4, MICRO 11 is the em3582
// for PLAT 4, MICRO 12 is the em3585
// for PLAT 4, MICRO 13 is the em3586
// for PLAT 4, MICRO 14 is the em3587
// for PLAT 4, MICRO 15 is the sky66107
// for PLAT 4, MICRO 16 is the em3597
// for PLAT 4, MICRO 17 is the em356
// for PLAT 4, MICRO 18 is the em3598
// for PLAT 4, MICRO 19 is the em3591
// for PLAT 4, MICRO 20 is the em3592
// for PLAT 4, MICRO 21 is the em3595
// for PLAT 4, MICRO 22 is the em3596
// for PLAT 4, MICRO 23 is the em317
// for PLAT 4, MICRO 24 is the efr32
// for PLAT 4, MICRO 25 is the obsolete_cortexm3_25
// for PLAT 4, MICRO 26 is the obsolete_cortexm3_26
// for PLAT 4, MICRO 27 is the em346
// for PLAT 4, MICRO 28 is the obsolete_cortexm3_28
// for PLAT 4, MICRO 29 is the obsolete_cortexm3_29
// for PLAT 4, MICRO 30 is the em355
// for PLAT 4, MICRO 31 is the em3555
// for PLAT 4, MICRO 32 is the obsolete_cortexm3_32
// for PLAT 4, MICRO 33 is the obsolete_cortexm3_33
// for PLAT 4, MICRO 34 is the obsolete_cortexm3_34

// PLAT 5 is the C8051
// for PLAT 5, MICRO 1 is the obsolete_c8051_1
// for PLAT 5, MICRO 2 is the obsolete_c8051_2
// for PLAT 5, MICRO 3 is the obsolete_c8051_3
// for PLAT 5, MICRO 4 is the obsolete_c8051_4
// for PLAT 5, MICRO 5 is the obsolete_c8051_5

// PLAT 6 is the FFD
// for PLAT 6, MICRO 1 is the si4010

// -----------------------------------------------------------------------------
// FAMILY values are used in EBL headers and bootloaders as well
// FAMILY 1 is the BGM13
// FAMILY 1 is the MGM1
// FAMILY 1 is the MGM21
// FAMILY 1 is the BGM21
// FAMILY 1 is the MGM13
// FAMILY 1 is the MGM12
// FAMILY 1 is the BGM1
// FAMILY 16 is the EFR32MG1P
// FAMILY 17 is the EFR32MG1B
// FAMILY 18 is the EFR32MG1V
// FAMILY 19 is the EFR32BG1P
// FAMILY 20 is the EFR32BG1B
// FAMILY 21 is the EFR32BG1V
// FAMILY 25 is the EFR32FG1P
// FAMILY 27 is the EFR32FG1V
// FAMILY 28 is the EFR32MG12P
// FAMILY 31 is the EFR32BG12P
// FAMILY 37 is the EFR32FG12P
// FAMILY 40 is the EFR32MG13P
// FAMILY 43 is the EFR32BG13P
// FAMILY 49 is the EFR32FG13P
// FAMILY 52 is the EFR32MG14P
// FAMILY 55 is the EFR32BG14P
// FAMILY 58 is the EFR32ZG14P
// FAMILY 61 is the EFR32FG14P
// FAMILY 63 is the EFR32FG14V
// FAMILY 128 is the EFR32MG21
// FAMILY 128 is the EFR32BG21
// -----------------------------------------------------------------------------

// Create an enum for the platforms
enum {
  EMBER_PLATFORM_UNKNOWN    = 0,
  EMBER_PLATFORM_AVR_ATMEGA = 1,
  EMBER_PLATFORM_XAP2B      = 2,
  EMBER_PLATFORM_OBSOLETE_3 = 3,
  EMBER_PLATFORM_CORTEXM3   = 4,
  EMBER_PLATFORM_C8051      = 5,
  EMBER_PLATFORM_FFD        = 6,
  EMBER_PLATFORM_MAX_VALUE
};

typedef uint16_t EmberPlatformEnum;

#define EMBER_PLATFORM_STRINGS \
  "Unknown",                   \
  "Test",                      \
  "XAP2b",                     \
  "obsolete_3",                \
  "CortexM3",                  \
  "C8051",                     \
  "FFD",                       \
  NULL,

// Create an enum for all of the different AVR ATMega micros we support
enum {
  EMBER_MICRO_AVR_ATMEGA_UNKNOWN   = 0,
  EMBER_MICRO_AVR_ATMEGA_MAX_VALUE
};

typedef uint16_t EmberMicroAvrAtmegaEnum;

#define EMBER_MICRO_AVR_ATMEGA_STRINGS \
  "Unknown",                           \
  NULL,

// Create an enum for all of the different XAP2b micros we support
enum {
  EMBER_MICRO_XAP2B_UNKNOWN   = 0,
  EMBER_MICRO_XAP2B_EM250     = 1,
  EMBER_MICRO_XAP2B_EM260     = 2,
  EMBER_MICRO_XAP2B_MAX_VALUE
};

typedef uint16_t EmberMicroXap2bEnum;

#define EMBER_MICRO_XAP2B_STRINGS \
  "Unknown",                      \
  "em250",                        \
  "em260",                        \
  NULL,

// Create an enum for all of the different obsolete_3 micros we support
enum {
  EMBER_MICRO_OBSOLETE_3_UNKNOWN   = 0,
  EMBER_MICRO_OBSOLETE_3_MAX_VALUE
};

typedef uint16_t EmberMicroObsolete3Enum;

#define EMBER_MICRO_OBSOLETE_3_STRINGS \
  "Unknown",                           \
  NULL,

// Create an enum for all of the different CortexM3 micros we support
enum {
  EMBER_MICRO_CORTEXM3_UNKNOWN              = 0,
  EMBER_MICRO_CORTEXM3_EM350                = 1,
  EMBER_MICRO_CORTEXM3_EM360                = 2,
  EMBER_MICRO_CORTEXM3_EM357                = 3,
  EMBER_MICRO_CORTEXM3_EM367                = 4,
  EMBER_MICRO_CORTEXM3_EM351                = 5,
  EMBER_MICRO_CORTEXM3_EM35X                = 6,
  EMBER_MICRO_CORTEXM3_OBSOLETE_CORTEXM3_7  = 7,
  EMBER_MICRO_CORTEXM3_EM3588               = 8,
  EMBER_MICRO_CORTEXM3_EM359                = 9,
  EMBER_MICRO_CORTEXM3_EM3581               = 10,
  EMBER_MICRO_CORTEXM3_EM3582               = 11,
  EMBER_MICRO_CORTEXM3_EM3585               = 12,
  EMBER_MICRO_CORTEXM3_EM3586               = 13,
  EMBER_MICRO_CORTEXM3_EM3587               = 14,
  EMBER_MICRO_CORTEXM3_SKY66107             = 15,
  EMBER_MICRO_CORTEXM3_EM3597               = 16,
  EMBER_MICRO_CORTEXM3_EM356                = 17,
  EMBER_MICRO_CORTEXM3_EM3598               = 18,
  EMBER_MICRO_CORTEXM3_EM3591               = 19,
  EMBER_MICRO_CORTEXM3_EM3592               = 20,
  EMBER_MICRO_CORTEXM3_EM3595               = 21,
  EMBER_MICRO_CORTEXM3_EM3596               = 22,
  EMBER_MICRO_CORTEXM3_EM317                = 23,
  EMBER_MICRO_CORTEXM3_EFR32                = 24,
  EMBER_MICRO_CORTEXM3_OBSOLETE_CORTEXM3_25 = 25,
  EMBER_MICRO_CORTEXM3_OBSOLETE_CORTEXM3_26 = 26,
  EMBER_MICRO_CORTEXM3_EM346                = 27,
  EMBER_MICRO_CORTEXM3_OBSOLETE_CORTEXM3_28 = 28,
  EMBER_MICRO_CORTEXM3_OBSOLETE_CORTEXM3_29 = 29,
  EMBER_MICRO_CORTEXM3_EM355                = 30,
  EMBER_MICRO_CORTEXM3_EM3555               = 31,
  EMBER_MICRO_CORTEXM3_OBSOLETE_CORTEXM3_32 = 32,
  EMBER_MICRO_CORTEXM3_OBSOLETE_CORTEXM3_33 = 33,
  EMBER_MICRO_CORTEXM3_OBSOLETE_CORTEXM3_34 = 34,
  EMBER_MICRO_CORTEXM3_MAX_VALUE
};

typedef uint16_t EmberMicroCortexM3Enum;

#define EMBER_MICRO_CORTEXM3_STRINGS \
  "Unknown",                         \
  "em350",                           \
  "em360",                           \
  "em357",                           \
  "em367",                           \
  "em351",                           \
  "em35x",                           \
  "obsolete_cortexm3_7",             \
  "em3588",                          \
  "em359",                           \
  "em3581",                          \
  "em3582",                          \
  "em3585",                          \
  "em3586",                          \
  "em3587",                          \
  "sky66107",                        \
  "em3597",                          \
  "em356",                           \
  "em3598",                          \
  "em3591",                          \
  "em3592",                          \
  "em3595",                          \
  "em3596",                          \
  "em317",                           \
  "efr32",                           \
  "obsolete_cortexm3_25",            \
  "obsolete_cortexm3_26",            \
  "em346",                           \
  "obsolete_cortexm3_28",            \
  "obsolete_cortexm3_29",            \
  "em355",                           \
  "em3555",                          \
  "obsolete_cortexm3_32",            \
  "obsolete_cortexm3_33",            \
  "obsolete_cortexm3_34",            \
  NULL,

// Create an enum for all of the different C8051 micros we support
enum {
  EMBER_MICRO_C8051_UNKNOWN          = 0,
  EMBER_MICRO_C8051_OBSOLETE_C8051_1 = 1,
  EMBER_MICRO_C8051_OBSOLETE_C8051_2 = 2,
  EMBER_MICRO_C8051_OBSOLETE_C8051_3 = 3,
  EMBER_MICRO_C8051_OBSOLETE_C8051_4 = 4,
  EMBER_MICRO_C8051_OBSOLETE_C8051_5 = 5,
  EMBER_MICRO_C8051_MAX_VALUE
};

typedef uint16_t EmberMicroC8051Enum;

#define EMBER_MICRO_C8051_STRINGS \
  "Unknown",                      \
  "obsolete_c8051_1",             \
  "obsolete_c8051_2",             \
  "obsolete_c8051_3",             \
  "obsolete_c8051_4",             \
  "obsolete_c8051_5",             \
  NULL,

// Create an enum for all of the different FFD micros we support
enum {
  EMBER_MICRO_FFD_UNKNOWN   = 0,
  EMBER_MICRO_FFD_SI4010    = 1,
  EMBER_MICRO_FFD_MAX_VALUE
};

typedef uint16_t EmberMicroFfdEnum;

#define EMBER_MICRO_FFD_STRINGS \
  "Unknown",                    \
  "si4010",                     \
  NULL,

// Create an enum for the families
enum {
  EMBER_FAMILY_UNKNOWN    = 0,
  EMBER_FAMILY_BGM13      = 1,
  EMBER_FAMILY_MGM1       = 1,
  EMBER_FAMILY_MGM21      = 1,
  EMBER_FAMILY_BGM21      = 1,
  EMBER_FAMILY_MGM13      = 1,
  EMBER_FAMILY_MGM12      = 1,
  EMBER_FAMILY_BGM1       = 1,
  EMBER_FAMILY_EFR32MG1P  = 16,
  EMBER_FAMILY_EFR32MG1B  = 17,
  EMBER_FAMILY_EFR32MG1V  = 18,
  EMBER_FAMILY_EFR32BG1P  = 19,
  EMBER_FAMILY_EFR32BG1B  = 20,
  EMBER_FAMILY_EFR32BG1V  = 21,
  EMBER_FAMILY_EFR32FG1P  = 25,
  EMBER_FAMILY_EFR32FG1V  = 27,
  EMBER_FAMILY_EFR32MG12P = 28,
  EMBER_FAMILY_EFR32BG12P = 31,
  EMBER_FAMILY_EFR32FG12P = 37,
  EMBER_FAMILY_EFR32MG13P = 40,
  EMBER_FAMILY_EFR32BG13P = 43,
  EMBER_FAMILY_EFR32FG13P = 49,
  EMBER_FAMILY_EFR32MG14P = 52,
  EMBER_FAMILY_EFR32BG14P = 55,
  EMBER_FAMILY_EFR32ZG14P = 58,
  EMBER_FAMILY_EFR32FG14P = 61,
  EMBER_FAMILY_EFR32FG14V = 63,
  EMBER_FAMILY_EFR32MG21  = 128,
  EMBER_FAMILY_EFR32BG21  = 128,
  EMBER_FAMILY_MAX_VALUE
};

typedef uint16_t EmberFamilyEnum;

#define EMBER_FAMILY_STRINGS \
  "Unknown",                 \
  "BGM13",                   \
  "MGM1",                    \
  "MGM21",                   \
  "BGM21",                   \
  "MGM13",                   \
  "MGM12",                   \
  "BGM1",                    \
  "EFR32MG1P",               \
  "EFR32MG1B",               \
  "EFR32MG1V",               \
  "EFR32BG1P",               \
  "EFR32BG1B",               \
  "EFR32BG1V",               \
  "EFR32FG1P",               \
  "EFR32FG1V",               \
  "EFR32MG12P",              \
  "EFR32BG12P",              \
  "EFR32FG12P",              \
  "EFR32MG13P",              \
  "EFR32BG13P",              \
  "EFR32FG13P",              \
  "EFR32MG14P",              \
  "EFR32BG14P",              \
  "EFR32ZG14P",              \
  "EFR32FG14P",              \
  "EFR32FG14V",              \
  "EFR32MG21",               \
  "EFR32BG21",               \
  NULL,

// A dummy type declared to allow generically passing around this
// data type.  Micro specific enums (such as EmberMicroCortexM3Enum)
// are required to be the same size as this.
typedef uint16_t EmberMicroEnum;

// Determine what micro and platform that we're supposed to target using the
// defines passed in at build time. Then set the PLAT and MICRO defines based
// on what was passed in
#if ((!defined(EZSP_HOST)) && (!defined(UNIX_HOST)))

#if defined(EMBER_TEST)
  #define PLAT EMBER_PLATFORM_AVR_ATMEGA
  #define MICRO 2
#elif defined(AVR_ATMEGA)
  #define PLAT EMBER_PLATFORM_AVR_ATMEGA
#elif defined(XAP2B)
  #define PLAT EMBER_PLATFORM_XAP2B
  #if defined(XAP2B_EM250)
    #define MICRO EMBER_MICRO_XAP2B_EM250
  #elif defined(XAP2B_EM260)
    #define MICRO EMBER_MICRO_XAP2B_EM260
  #endif
#elif defined(OBSOLETE_3)
  #define PLAT EMBER_PLATFORM_OBSOLETE_3
#elif defined(CORTEXM3)
  #define PLAT EMBER_PLATFORM_CORTEXM3
  #if defined(CORTEXM3_EM350)
    #define MICRO EMBER_MICRO_CORTEXM3_EM350
  #elif defined(CORTEXM3_EM360)
    #define MICRO EMBER_MICRO_CORTEXM3_EM360
  #elif defined(CORTEXM3_EM357)
    #define MICRO EMBER_MICRO_CORTEXM3_EM357
  #elif defined(CORTEXM3_EM367)
    #define MICRO EMBER_MICRO_CORTEXM3_EM367
  #elif defined(CORTEXM3_EM351)
    #define MICRO EMBER_MICRO_CORTEXM3_EM351
  #elif defined(CORTEXM3_EM35X)
    #define MICRO EMBER_MICRO_CORTEXM3_EM35X
  #elif defined(CORTEXM3_OBSOLETE_CORTEXM3_7)
    #define MICRO EMBER_MICRO_CORTEXM3_OBSOLETE_CORTEXM3_7
  #elif defined(CORTEXM3_EM3588)
    #define MICRO EMBER_MICRO_CORTEXM3_EM3588
  #elif defined(CORTEXM3_EM359)
    #define MICRO EMBER_MICRO_CORTEXM3_EM359
  #elif defined(CORTEXM3_EM3581)
    #define MICRO EMBER_MICRO_CORTEXM3_EM3581
  #elif defined(CORTEXM3_EM3582)
    #define MICRO EMBER_MICRO_CORTEXM3_EM3582
  #elif defined(CORTEXM3_EM3585)
    #define MICRO EMBER_MICRO_CORTEXM3_EM3585
  #elif defined(CORTEXM3_EM3586)
    #define MICRO EMBER_MICRO_CORTEXM3_EM3586
  #elif defined(CORTEXM3_EM3587)
    #define MICRO EMBER_MICRO_CORTEXM3_EM3587
  #elif defined(CORTEXM3_SKY66107)
    #define MICRO EMBER_MICRO_CORTEXM3_SKY66107
  #elif defined(CORTEXM3_EM3597)
    #define MICRO EMBER_MICRO_CORTEXM3_EM3597
  #elif defined(CORTEXM3_EM356)
    #define MICRO EMBER_MICRO_CORTEXM3_EM356
  #elif defined(CORTEXM3_EM3598)
    #define MICRO EMBER_MICRO_CORTEXM3_EM3598
  #elif defined(CORTEXM3_EM3591)
    #define MICRO EMBER_MICRO_CORTEXM3_EM3591
  #elif defined(CORTEXM3_EM3592)
    #define MICRO EMBER_MICRO_CORTEXM3_EM3592
  #elif defined(CORTEXM3_EM3595)
    #define MICRO EMBER_MICRO_CORTEXM3_EM3595
  #elif defined(CORTEXM3_EM3596)
    #define MICRO EMBER_MICRO_CORTEXM3_EM3596
  #elif defined(CORTEXM3_EM317)
    #define MICRO EMBER_MICRO_CORTEXM3_EM317
  #elif defined(CORTEXM3_EFR32)
    #define MICRO EMBER_MICRO_CORTEXM3_EFR32
  #elif defined(CORTEXM3_OBSOLETE_CORTEXM3_25)
    #define MICRO EMBER_MICRO_CORTEXM3_OBSOLETE_CORTEXM3_25
  #elif defined(CORTEXM3_OBSOLETE_CORTEXM3_26)
    #define MICRO EMBER_MICRO_CORTEXM3_OBSOLETE_CORTEXM3_26
  #elif defined(CORTEXM3_EM346)
    #define MICRO EMBER_MICRO_CORTEXM3_EM346
  #elif defined(CORTEXM3_OBSOLETE_CORTEXM3_28)
    #define MICRO EMBER_MICRO_CORTEXM3_OBSOLETE_CORTEXM3_28
  #elif defined(CORTEXM3_OBSOLETE_CORTEXM3_29)
    #define MICRO EMBER_MICRO_CORTEXM3_OBSOLETE_CORTEXM3_29
  #elif defined(CORTEXM3_EM355)
    #define MICRO EMBER_MICRO_CORTEXM3_EM355
  #elif defined(CORTEXM3_EM3555)
    #define MICRO EMBER_MICRO_CORTEXM3_EM3555
  #elif defined(CORTEXM3_OBSOLETE_CORTEXM3_32)
    #define MICRO EMBER_MICRO_CORTEXM3_OBSOLETE_CORTEXM3_32
  #elif defined(CORTEXM3_OBSOLETE_CORTEXM3_33)
    #define MICRO EMBER_MICRO_CORTEXM3_OBSOLETE_CORTEXM3_33
  #elif defined(CORTEXM3_OBSOLETE_CORTEXM3_34)
    #define MICRO EMBER_MICRO_CORTEXM3_OBSOLETE_CORTEXM3_34
  #endif
#elif defined(C8051)
  #define PLAT EMBER_PLATFORM_C8051
  #if defined(C8051_OBSOLETE_C8051_1)
    #define MICRO EMBER_MICRO_C8051_OBSOLETE_C8051_1
  #elif defined(C8051_OBSOLETE_C8051_2)
    #define MICRO EMBER_MICRO_C8051_OBSOLETE_C8051_2
  #elif defined(C8051_OBSOLETE_C8051_3)
    #define MICRO EMBER_MICRO_C8051_OBSOLETE_C8051_3
  #elif defined(C8051_OBSOLETE_C8051_4)
    #define MICRO EMBER_MICRO_C8051_OBSOLETE_C8051_4
  #elif defined(C8051_OBSOLETE_C8051_5)
    #define MICRO EMBER_MICRO_C8051_OBSOLETE_C8051_5
  #endif
#elif defined(FFD)
  #define PLAT EMBER_PLATFORM_FFD
  #if defined(FFD_SI4010)
    #define MICRO EMBER_MICRO_FFD_SI4010
  #endif
#endif

#endif // ((! defined(EZSP_HOST)) && (! defined(UNIX_HOST)))

// Determine what family we're supposed to target using the defines passed in at
// build time. Then set the FAMILY define based on what was passed in.
// Parts without defined family will be assigned id 0.
#if defined(BGM13)
  #define FAMILY EMBER_FAMILY_BGM13
#elif defined(MGM1)
  #define FAMILY EMBER_FAMILY_MGM1
#elif defined(MGM21)
  #define FAMILY EMBER_FAMILY_MGM21
#elif defined(BGM21)
  #define FAMILY EMBER_FAMILY_BGM21
#elif defined(MGM13)
  #define FAMILY EMBER_FAMILY_MGM13
#elif defined(MGM12)
  #define FAMILY EMBER_FAMILY_MGM12
#elif defined(BGM1)
  #define FAMILY EMBER_FAMILY_BGM1
#elif defined(EFR32MG1P)
  #define FAMILY EMBER_FAMILY_EFR32MG1P
#elif defined(EFR32MG1B)
  #define FAMILY EMBER_FAMILY_EFR32MG1B
#elif defined(EFR32MG1V)
  #define FAMILY EMBER_FAMILY_EFR32MG1V
#elif defined(EFR32BG1P)
  #define FAMILY EMBER_FAMILY_EFR32BG1P
#elif defined(EFR32BG1B)
  #define FAMILY EMBER_FAMILY_EFR32BG1B
#elif defined(EFR32BG1V)
  #define FAMILY EMBER_FAMILY_EFR32BG1V
#elif defined(EFR32FG1P)
  #define FAMILY EMBER_FAMILY_EFR32FG1P
#elif defined(EFR32FG1V)
  #define FAMILY EMBER_FAMILY_EFR32FG1V
#elif defined(EFR32MG12P)
  #define FAMILY EMBER_FAMILY_EFR32MG12P
#elif defined(EFR32BG12P)
  #define FAMILY EMBER_FAMILY_EFR32BG12P
#elif defined(EFR32FG12P)
  #define FAMILY EMBER_FAMILY_EFR32FG12P
#elif defined(EFR32MG13P)
  #define FAMILY EMBER_FAMILY_EFR32MG13P
#elif defined(EFR32BG13P)
  #define FAMILY EMBER_FAMILY_EFR32BG13P
#elif defined(EFR32FG13P)
  #define FAMILY EMBER_FAMILY_EFR32FG13P
#elif defined(EFR32MG14P)
  #define FAMILY EMBER_FAMILY_EFR32MG14P
#elif defined(EFR32BG14P)
  #define FAMILY EMBER_FAMILY_EFR32BG14P
#elif defined(EFR32ZG14P)
  #define FAMILY EMBER_FAMILY_EFR32ZG14P
#elif defined(EFR32FG14P)
  #define FAMILY EMBER_FAMILY_EFR32FG14P
#elif defined(EFR32FG14V)
  #define FAMILY EMBER_FAMILY_EFR32FG14V
#elif defined(EFR32MG21)
  #define FAMILY EMBER_FAMILY_EFR32MG21
#elif defined(EFR32BG21)
  #define FAMILY EMBER_FAMILY_EFR32BG21
#else
  #define FAMILY 0
#endif

// Define distinct literal values for each phy. These values are used in the
// bootloader query response message.
// PHY 0 is NULL
// PHY 1 is em2420 (no longer supported)
// PHY 2 is em250
// PHY 3 is em3XX
// PHY 4 is si446x (no longer supported)
// PHY 5 is cobra (no longer supported)
// PHY 6 is PRO2+
// PHY 7 is si4x55 (no longer supported)
// PHY 8 is si4010 (no longer supported)
// PHY 9 is efr32
// PHY 10 is pro2 (no longer supported)
// PHY 11 is ezr2 (no longer supported)
// PHY 12 is dual
// PHY 13 is transceiver_sim
// PHY 14 is efr32gb
// PHY 15 is rail
// PHY 16 is rail_mp
// PHY 17 is simulation_dual
// PHY 18 is railgb
// PHY 19 is railgb_mp
// PHY 20 is dualrail

enum {
  EMBER_PHY_NULL            = 0,
  EMBER_PHY_EM2420          = 1,
  EMBER_PHY_EM250           = 2,
  EMBER_PHY_EM3XX           = 3,
  EMBER_PHY_SI446X          = 4,
  EMBER_PHY_COBRA           = 5,
  EMBER_PHY_PRO2PLUS        = 6,
  EMBER_PHY_SI4X55          = 7,
  EMBER_PHY_SI4010          = 8,
  EMBER_PHY_EFR32           = 9,
  EMBER_PHY_PRO2            = 10,
  EMBER_PHY_EZR2            = 11,
  EMBER_PHY_DUAL            = 12,
  EMBER_PHY_TRANSCEIVER_SIM = 13,
  EMBER_PHY_EFR32GB         = 14,
  EMBER_PHY_RAIL            = 15,
  EMBER_PHY_RAIL_MP         = 16,
  EMBER_PHY_SIMULATION_DUAL = 17,
  EMBER_PHY_RAILGB          = 18,
  EMBER_PHY_RAILGB_MP       = 19,
  EMBER_PHY_DUALRAIL        = 20,
  EMBER_PHY_MAX_VALUE
};

typedef uint16_t EmberPhyEnum;

#define EMBER_PHY_STRINGS \
  "NULL",                 \
  "em2420",               \
  "em250",                \
  "em3XX",                \
  "si446x",               \
  "cobra",                \
  "PRO2+",                \
  "si4x55",               \
  "si4010",               \
  "efr32",                \
  "pro2",                 \
  "ezr2",                 \
  "dual",                 \
  "transceiver_sim",      \
  "efr32gb",              \
  "rail",                 \
  "rail_mp",              \
  "simulation_dual",      \
  "railgb",               \
  "railgb_mp",            \
  "dualrail",             \
  NULL,

#if defined(PHY_NULL)
  #define PHY EMBER_PHY_NULL
#elif defined(PHY_EM2420) || defined(PHY_EM2420B)
  #error em2420 PHY is no longer supported
#elif defined(PHY_EM250) || defined(PHY_EM250B)
  #define PHY EMBER_PHY_EM250
#elif defined(PHY_EM3XX)
  #define PHY EMBER_PHY_EM3XX
#elif defined(PHY_SI446X_US) || defined(PHY_SI446X_EU) || defined(PHY_SI446X_CN) || defined(PHY_SI446X_JP)
  #error si446x PHY is no longer supported
#elif defined(PHY_COBRA)
  #error cobra PHY is no longer supported
#elif defined(PHY_PRO2PLUS)
  #define PHY EMBER_PHY_PRO2PLUS
#elif defined(PHY_SI4X55)
  #error si4x55 PHY is no longer supported
#elif defined(PHY_SI4010)
  #error si4010 PHY is no longer supported
#elif defined(PHY_EFR32)
  #define PHY EMBER_PHY_EFR32
#elif defined(PHY_PRO2)
  #error pro2 PHY is no longer supported
#elif defined(PHY_EZR2)
  #error ezr2 PHY is no longer supported
#elif defined(PHY_DUAL)
  #define PHY EMBER_PHY_DUAL
#elif defined(PHY_TRANSCEIVER_SIM)
  #define PHY EMBER_PHY_TRANSCEIVER_SIM
#elif defined(PHY_EFR32GB)
  #define PHY EMBER_PHY_EFR32GB
#elif defined(PHY_RAIL)
  #define PHY EMBER_PHY_RAIL
#elif defined(PHY_RAIL_MP)
  #define PHY EMBER_PHY_RAIL_MP
#elif defined(PHY_SIMULATION_DUAL)
  #define PHY EMBER_PHY_SIMULATION_DUAL
#elif defined(PHY_RAILGB)
  #define PHY EMBER_PHY_RAILGB
#elif defined(PHY_RAILGB_MP)
  #define PHY EMBER_PHY_RAILGB_MP
#elif defined(PHY_DUALRAIL)
  #define PHY EMBER_PHY_DUALRAIL
#endif

typedef struct {
  EmberPlatformEnum platform;
  EmberMicroEnum micro;
  EmberPhyEnum phy;
} EmberChipTypeStruct;

// load up any chip-specific feature defines
// #if defined(PLAT) && defined(MICRO) && PLAT == EMBER_PLATFORM_CORTEXM3
//   #include "cortexm3/micro-features.h"
// #endif

#endif // DOXYGEN_SHOULD_SKIP_THIS

#endif // __MICRO_DEFINES_H__
