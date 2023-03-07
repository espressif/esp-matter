/***************************************************************************//**
 * @file
 * @brief Simulated EEPROM 2 public definitions.
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
#include PLATFORM_HEADER
#include "stack/config/ember-configuration-defaults.h"
#include "error-def.h"
#include "hal/hal.h"
#include "sl_sim_eeprom.h"
#include "sl_token_manager.h"

#ifndef COUNTER_TOKEN_PAD
  #define COUNTER_TOKEN_PAD        50
#endif // COUNTER_TOKEN_PAD

#if defined(CORTEXM3) || defined(TOKEN_MANAGER_TEST)
//On the Cortex-M3 chips we define a variable that holds the actual
//SimEE storage and then place this storage at the proper location
//in the linker.
// When NVM3 is used it means we are upgrading from SimEE2 to NVM3 and
// NVM3 will define the storage array
#ifdef SIMEE2_TO_NVM3_UPGRADE
#ifndef EMBER_TEST
extern uint8_t nvm3Storage[];
extern uint8_t *nvm3Address;
// simulatedEepromAddress is part of libsim_eeprom libraries.
uint8_t *simulatedEepromAddress;

#if defined(__ICCARM__)

uint8_t *simulatedEepromAddress = (uint8_t *)__segment_begin("SIMEE");

#elif defined(__GNUC__)

// NOTE: to future readers of this code:
//  In traditional SimEE this following definition is needed for placing
//  SimEE in flash.  But for SIMEE2_TO_NVM3_UPGRADE the NVM3 memory
//  section is used and having this array would double the size SimEE/NVM3
//  storage and put all the addressing references off.
//  Ultimately simulatedEepromAddress will be set to linker_nvm_begin
//  which is sensitive to having multiple arrays placed in flash.
//__attribute__((used)) uint8_t simulatedEepromStorage[SIMEE_SIZE_B] __attribute__ ((section(".simee")));

// Due to GCC linker requirements, the SimEE Storage memory
// must be accessed via the linker_nvm_begin segment name.
extern uint32_t linker_nvm_begin;
#define SIMEE_ADDR (&linker_nvm_begin)
uint8_t *simulatedEepromAddress = (uint8_t *)SIMEE_ADDR;

#else // !__ICCARM__ && !__GNUC__
#error "Unsupported toolchain"

#endif // __ICCARM__ || __GNUC__

#endif // EMBER_TEST

#else //SIMEE2_TO_NVM3_UPGRADE
#ifdef EMBER_TEST
uint8_t simulatedEepromStorage[SIMEE_SIZE_B];
#elif TOKEN_MANAGER_TEST

#if defined(__ICCARM__)

__root uint8_t simulatedEepromStorage[SIMEE_SIZE_B] @ "SIMEE";
uint8_t *simulatedEepromAddress = (uint8_t *)simulatedEepromStorage;

#elif defined(__GNUC__)

SL_ALIGN(FLASH_PAGE_SIZE)
static const uint8_t simulatedEepromStorage[SIMEE_SIZE_B] SL_ATTRIBUTE_ALIGN(FLASH_PAGE_SIZE) = { 0xff };

__attribute__((used)) static const uint8_t simulatedEepromStorage[SIMEE_SIZE_B] __attribute__ ((section(".simee")));
extern char __nvm3Base __attribute__((alias("simulatedEepromStorage")));
uint8_t *simulatedEepromAddress = (uint8_t *)simulatedEepromStorage;

#else // !__ICCARM__ && !__GNUC__
#error "Unsupported toolchain"
#endif // __ICCARM__ || __GNUC__

#else // EMBER_TEST || TOKEN_MANAGER_TEST

#if defined(__ICCARM__)

__root uint8_t simulatedEepromStorage[SIMEE_SIZE_B] @ "SIMEE";

uint8_t *simulatedEepromAddress = (uint8_t *)simulatedEepromStorage;

#elif defined(__GNUC__)

__attribute__((used)) uint8_t simulatedEepromStorage[SIMEE_SIZE_B] __attribute__ ((section(".simee")));

// Due to GCC linker requirements, the SimEE Storage memory
// must be accessed via the linker_nvm_begin segment name.
extern uint32_t linker_nvm_begin;
#define SIMEE_ADDR (&linker_nvm_begin)
uint8_t *simulatedEepromAddress = (uint8_t *)SIMEE_ADDR;

#else // !__ICCARM__ && !__GNUC__
#error "Unsupported toolchain"

#endif // __ICCARM__ || __GNUC__
#endif // EMBER_TEST || TOKEN_MANAGER_TEST
#endif // SIMEE2_TO_NVM3_UPGRADE
//NOTE: All SimEE addresses are 16bit "SimEE words" whereas the CM3
//      absolute size/addressing is 32bit words so a CM3 halfword (HW)
//      is 16bits.
//      The SimEE addresses are really just offsets into
//      the simulatedEepromStorage array.
//sl_sim_eeprom2_internal.c uses a set of defines that parameterize its behavior.
//Since the -internal file is precompiled, we must externally define the
//parameters (as constants) so an application build can alter them.  The
//two defines, NUMBER_OF_VIRTUAL_PAGES and VP_SIZE, are not used
//in sl_sim_eeprom2_internal.c but instead only used in this file to make
//the page locations a little more understandable.
//
//VPA, VPB, and VPC are the names for Virtual Page A, B, and C.
//
//SimEE2's inherent design uses three virtual.  A verbose name simply
//makes the rest of these defines easier to understand.
  #define NUMBER_OF_VIRTUAL_PAGES (3)
  #define VP_SIZE (((SIMEE_SIZE_HW / FLASH_PAGE_SIZE_HW) \
                    / NUMBER_OF_VIRTUAL_PAGES) * FLASH_PAGE_SIZE_HW)
const uint16_t VIRTUAL_PAGE_SIZE = VP_SIZE;
const uint16_t REAL_PAGE_SIZE = (FLASH_PAGE_SIZE_HW);
const uint8_t REAL_PAGES_PER_VIRTUAL = ((SIMEE_SIZE_HW / FLASH_PAGE_SIZE_HW) \
                                        / NUMBER_OF_VIRTUAL_PAGES);
const uint16_t VPA_BASE = (SIMEE_BASE_ADDR_HW);
const uint16_t VPA_TOP = (SIMEE_BASE_ADDR_HW
                          + VP_SIZE - 1);
const uint16_t VPB_BASE = (SIMEE_BASE_ADDR_HW
                           + VP_SIZE);
const uint16_t VPB_TOP = (SIMEE_BASE_ADDR_HW
                          + VP_SIZE
                          + VP_SIZE - 1);
const uint16_t VPC_BASE = (SIMEE_BASE_ADDR_HW
                           + VP_SIZE
                           + VP_SIZE);
const uint16_t VPC_TOP = (SIMEE_BASE_ADDR_HW
                          + VP_SIZE
                          + VP_SIZE
                          + VP_SIZE - 1);
#endif // defined(CORTEXM3) || defined(TOKEN_MANAGER_TEST)

const uint16_t ID_COUNT = TOKEN_COUNT;

#define DEFINETOKENS
//Manufacturing tokens do not exist in the SimEEPROM. Define them to nothing.
#define TOKEN_MFG(name, creator, iscnt, isidx, type, arraysize, ...)

//Fundamental limitations of the Simulated EEPROM design:
//These limitations are forcefully enforced via compile-time errors
//in the declarations that follow because exceeding any of them will
//cause fatal operation and data corruption.
//(1) The maximum number of tokens that can be stored is 254.
//    (Tokens are identified by one-byte values, with 0xFF as invalid.)
//(2a)The maximum size, in bytes, of a token (element) is 254.
//    (One byte, with 0xFF as invalid)
//(2b)Counter tokens must be exactly 4 bytes as determined by sizeof().
//(3) The maximum number of elements in an indexed token is 126.
//    (One byte, with MSB used as counter flag and 0x7F as non-indexed size.)
//(4) The maximum total storage for tokens plus management is SIMEE_BTS_SIZE_B/2.
//    (This limit is due to a Virtual Page only being a limited size and for
//     the Simulated EEPROM to operate with any kind of efficiency, the
//     Base Storage must be small enough to leave room for token copies.)

//If the arraysize is exactly 1, we need only a single entry in the ptrCache
//to hold this entry.  If the arraysize is not 1, we need +1 to hold
//the redirection entry in the Primary Table and then arraysize entries in
//the indexed table.  This works for all values of arraysize.
#define TOKEN_DEF(name, creator, iscnt, isidx, type, arraysize, ...) \
  + (((arraysize) < 127 /*fundamental limit (3)*/)                   \
     ? ((arraysize) + (((arraysize) == 1) ? 0 : 1))                  \
     : -10000 /*force negative array length compile-time error*/)
//value of all index counts added together
const uint16_t PTR_CACHE_SIZE = (0
                                 // Multiple inclusion of unguarded token-related header files is by design; suppress violation.
                                 //cstat !MISRAC2012-Dir-4.10
    #include "stack/config/token-stack.h"
                                 );
//the ptrCache definition - used to provide efficient access, based upon
//ID and index, to the freshest data in the Simulated EEPROM.
uint16_t ptrCache[0    //Compiler error here means too many elements in an array token
                  // Multiple inclusion of unguarded token-related header files is by design; suppress violation.
                  //cstat !MISRAC2012-Dir-4.10
    #include "stack/config/token-stack.h"
];
#undef TOKEN_DEF

//*totalTokenStorage notes the initial allocation of token data words
//in the flash virtual page. It is checked against the limit
//SIMEE_BTS_SIZE_B/2 and used to determine how much pre-copying is
//needed per token write to migrate all tokens for virtual page rotation.
#define TOKEN_DEF(name, creator, iscnt, isidx, type, arraysize, ...)            \
  + ((arraysize) * (3 /*info words*/ + ((iscnt)                                 \
                                        ? 2 + BYTES_TO_WORDS(COUNTER_TOKEN_PAD) \
                                        : BYTES_TO_WORDS(sizeof(type)))))
const uint32_t totalTokenStorage[(0 //Compiler error here means total token storage exceeds limit
                                  + (TOKEN_COUNT * 2)
                                  // Multiple inclusion of unguarded token-related header files is by design; suppress violation.
                                  //cstat !MISRAC2012-Dir-4.10
    #include "stack/config/token-stack.h"
                                  ) <= (SIMEE_BTS_SIZE_B / 2)/*fundamental limit (4)*/
                                 ? 1 : -1/*negative forces compile-time error*/
] = { (TOKEN_COUNT * 2)
      // Multiple inclusion of unguarded token-related header files is by design; suppress violation.
      //cstat !MISRAC2012-Dir-4.10
    #include "stack/config/token-stack.h"
};
#undef TOKEN_DEF

//The lookupCache maps compiled token IDs (cache index) to stored IDs.
//It must be declared here, outside the library, so that it can
//be appropriately sized based on the number of tokens the application
//defines at compile time.
uint8_t lookupCache[0  //Compiler error here means too many tokens declared
                    + (TOKEN_COUNT < 255/*fundamental limit (1)*/
                       ? TOKEN_COUNT : -1 /*force compile-time error*/)];

//The Size Cache definition.  Links the token to its internal storage size
//for efficient calculations.  Indexed by compile ID.  Size is in words.
//This Cache should be used for all size calculations.  halInternalSimEeStartup
//will guarantee that the compiled sizes defining this cache match the
//stored sizes.
//NOTE: All counter tokens must be 32bit numbers and the COUNTER_TOKEN_PAD is
//      added to provide room for the incremarks.
#define TOKEN_DEF(name, creator, iscnt, isidx, type, arraysize, ...) \
  + (((iscnt) ? (sizeof(type) ==  4 /*fundamental limit (2b)*/)      \
      : (sizeof(type) < 255 /*fundamental limit (2a)*/))             \
     ? 1 : -10000 /*large negative will force compile-time error*/)
const uint8_t sizeCache[0    //Compiler error here means a token's size is too large or a counter token is wrong size
                        // Multiple inclusion of unguarded token-related header files is by design; suppress violation.
                        //cstat !MISRAC2012-Dir-4.10
    #include "stack/config/token-stack.h"
] = {
#undef TOKEN_DEF
#define TOKEN_DEF(name, creator, iscnt, isidx, type, arraysize, ...) \
  (BYTES_TO_WORDS(((iscnt) ? (4 + COUNTER_TOKEN_PAD) : (sizeof(type))))),
  // Multiple inclusion of unguarded token-related header files is by design; suppress violation.
  //cstat !MISRAC2012-Dir-4.10
    #include "stack/config/token-stack.h"
};
#undef TOKEN_DEF

#undef TOKEN_MFG
#undef DEFINETOKENS

//Forward prototype for the internal, non public SimEE startup.
//This function should never be called directly.
EmberStatus halInternalSimEeStartupCore(bool forceRebuildAll,
                                        uint8_t *lookupCache);

EmberStatus halInternalSimEeStartup(bool forceRebuildAll)
{
  uint8_t i;
  uint32_t indexSkip = TOKEN_COUNT;

#if defined(SIMEE2_TO_NVM3_UPGRADE)
  #if defined(__ICCARM__)
  // Intentionally emmpty here.  __ICCARM__ left in place for any
  // future work.
  #elif defined(__GNUC__)
  simulatedEepromAddress = (uint8_t *)&linker_nvm_begin;
  #endif // __GNUC__
#endif // SIMEE2_TO_NVM3_UPGRADE
  //The value 0xFF is used as a trigger in both caches to indicate an
  //uninitialized entry.  Unitialized entries (i.e. missing tokens) will
  //trigger repairing.
  MEMSET(lookupCache, 0xFF, ID_COUNT);
  MEMSET(ptrCache, 0xFF, PTR_CACHE_SIZE * sizeof(uint16_t));

  for (i = 0; i < TOKEN_COUNT; i++) {
    uint32_t size = BYTES_TO_WORDS(tokenSize[i]);
    uint32_t arraySize = tokenArraySize[i];
    if (tokenIsCnt[i]) {
      size += BYTES_TO_WORDS(COUNTER_TOKEN_PAD);
    }

#ifndef TOKEN_MANAGER_TEST
    emberDebugPrintf("Creator: 0x%2X, Words: %d\n",
                     tokenCreators[i],
                     arraySize * (3 + size));
    //The '3 +' is for the initial info words in each token.
#endif // TOKEN_MANAGER_TEST

    //Install the indexed token ptrCache redirection value.  If a token is
    //indexed, set the ptrCache equal to the index number of where the
    //actual token addresses start in the ptrCache.
    if (arraySize != 1) {
      ptrCache[i] = indexSkip;
      indexSkip += arraySize;
    }
  }

#ifndef TOKEN_MANAGER_TEST
  emberDebugPrintf("SimEE data: %d words of %d max, tokenCount: %d\n",
                   *totalTokenStorage,
                   (SIMEE_BTS_SIZE_B / 2),
                   TOKEN_COUNT);
#endif // TOKEN_MANAGER_TEST

  return halInternalSimEeStartupCore(forceRebuildAll, lookupCache);
}

// Link libsim_eeprom2 use of halInternalResetWatchDog to the
// public halResetWatchdog
void halInternalResetWatchDog(void)
{
  halResetWatchdog();
}

#if !defined(SIMEE1_TO_SIMEE2_UPGRADE) && !defined(SIMEE2_TO_NVM3_UPGRADE)
// Stubs for when not upgrading.
bool findSimEe1TokenData(void)
{
  return false;
}

EmberStatus halInternalFlashBlockReadSimEe1(uint16_t addr, uint16_t *data, uint16_t length)
{
  (void) addr;
  (void) data;
  (void) length;

  return EMBER_ERR_FATAL;
}

void simEe1ReadCounterToken(uint16_t i, uint16_t *oldDataAddress, uint32_t *number)
{
  (void) i;
  (void) oldDataAddress;
  (void) number;
}
#endif // !SIMEE1_TO_SIMEE2_UPGRADE && !SIMEE2_TO_NVM3_UPGRADE
