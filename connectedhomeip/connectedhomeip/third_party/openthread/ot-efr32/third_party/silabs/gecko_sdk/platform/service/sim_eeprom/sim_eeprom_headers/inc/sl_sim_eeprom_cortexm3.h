/***************************************************************************//**
 * @file
 * @brief Simulated EEPROM system for wear leveling token storage across flash.
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
#ifndef __PLAT_SIM_EEPROM_H__
#define __PLAT_SIM_EEPROM_H__

/***************************************************************************//**
 * @addtogroup legacyhal
 * @{
 ******************************************************************************/

// Note to reader: The following block was originally part of
// "simeepromcom" doxygen but it was removed from Doxygen
// since SimEE is legacy, should not be used, and provides informaton that
// is inconcistent with the Token Manager.
/*
 * @{
 * By default, the EM35x Simulated EEPROM is designed to consume 8kB of
 * upper flash within which it will perform wear leveling.
 *
 * While there is no specific ::EMBER_SIM_EEPROM_8KB, it is possible to use
 * the define ::EMBER_SIM_EEPROM_8KB for clarity in your application.
 *
 * @note Simulated EEPROM and Simulated EEPROM 2 functions cannot be
 * intermixed; SimEE and SimEE2 are mutually exclusive.  The functions
 * in @ref simeeprom1 cannot be used with
 * SimEE2 and the functons in @ref simeeprom2 cannot be used
 * with SimEE1.
 *
 * The Simulated EEPROM 2 system (typically referred to as SimEE2) is
 * designed to operate under the @ref tokens
 * API and provide a non-volatile storage system.  Since the flash write cycles
 * are finite, the SimEE2's primary purpose is to perform wear
 * leveling across several hardware flash pages, ultimately increasing the
 * number of times tokens may be written before a hardware failure.
 *
 * Compiling the application with the define USE_SIMEE2 will switch
 * the application from using the original SimEE to SimEE2.
 *
 * @note Only the NCP is capable of upgrading it's existing SimEE data
 * to SimEE2.  It's not possible to downgrade from SimEE2.
 *
 * The Simulated EEPROM 2 needs to periodically perform a page erase
 * operation to reclaim storage area for future token writes.  The page
 * erase operation requires an ATOMIC block of 21ms.  Since this is such a long
 * time to not be able to service any interrupts, the page erase operation is
 * under application control providing the application the opportunity to
 * decide when to perform the operation and complete any special handling
 * that might be needed.
 *
 * @note The best, safest, and recommended practice is for the application
 * to regularly and always call the function halSimEepromErasePage() when the
 * application can expect and deal with the page erase delay.
 * halSimEepromErasePage() will immediately return if there is nothing to
 * erase.  If there is something that needs to be erased, doing so
 * as regularly and as soon as possible will keep the SimEE2 in the
 * healthiest state possible.
 *
 * SimEE2  differs from the
 * original SimEE in terms of size and speed.  SimEE2 holds more data
 * but at the expense of consuming more overall flash to support a new
 * wear levelling technique.  SimEE2's worst case execution time under
 * normal behavior is faster and finite, but the average execution time is
 * longer.
 *
 * See sl_sim_eeprom_cortexm3.h for source code.
 */

/** @} (end addtogroup legacyhal) */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// See the sl_sim_eeprom_size.h file for the logic used to determine the ultimate
// size of the simeeprom.
#include "sl_sim_eeprom_size.h"

//This is confusing, so pay attention...  :-)
//The actual SimEE storage lives inside of simulatedEepromStorage and begins
//at the very bottom of simulatedEepromStorage and fills the entirety of
//this storage array.  The SimEE code, though, uses 16bit addresses for
//everything since it was originally written for the XAP2b.  On a 250,
//the base address was 0xF000 since this corresponded to the actual absolute
//address that was used in flash.  On a non-250, though, the base address
//is largely irrelevant since there is a translation shim layer that converts
//SimEE 16bit addresses into the real 32bit addresses needed to access flash.
//If you look at the translation shim layer in
//sim_eeprom_internal.c you'll see that the address used by the SimEE is
//subtracted by VPA_BASE (which is the same as SIMEE_BASE_ADDR_HW) to
//return back to the bottom of the simulatedEepromStorage area.
//[BugzId:14448 fix removes need for this to be anything but 0x0000]
#define SIMEE_BASE_ADDR_HW  0x0000

//Define a variable that holds the actual SimEE storage the linker will
//place at the proper location in flash.
#ifndef TOKEN_MANAGER_TEST
extern uint8_t simulatedEepromStorage[SIMEE_SIZE_B];
#endif // TOKEN_MANAGER_TEST
//Define a pointer to the actual SimEE storage in flash.
extern uint8_t *simulatedEepromAddress;

//these parameters frame the sim_eeprom and are derived from the location
//of the sim_eeprom as defined in memmap.h

/**
 * @brief The size of a physical flash page, in SimEE addressing units.
 */
extern const uint16_t REAL_PAGE_SIZE;

/**
 * @brief The size of a Virtual Page, in SimEE addressing units.
 */
extern const uint16_t VIRTUAL_PAGE_SIZE;

/**
 * @brief The number of physical pages in a Virtual Page.
 */
extern const uint8_t REAL_PAGES_PER_VIRTUAL;

/**
 * @brief The bottom address of the Left Virtual Page.
 * Only used in Simulated EEPROM 1.
 */
extern const uint16_t LEFT_BASE;

/**
 * @brief The top address of the Left Virtual Page.
 * Only used in Simulated EEPROM 1.
 */
extern const uint16_t LEFT_TOP;

/**
 * @brief The bottom address of the Right Virtual Page.
 * Only used in Simulated EEPROM 1.
 */
extern const uint16_t RIGHT_BASE;

/**
 * @brief The top address of the Right Virtual Page.
 * Only used in Simulated EEPROM 1.
 */
extern const uint16_t RIGHT_TOP;

/**
 * @brief The bottom address of Virtual Page A.
 * Only used in Simulated EEPROM 2.
 */
extern const uint16_t VPA_BASE;

/**
 * @brief The top address of Virtual Page A.
 * Only used in Simulated EEPROM 2.
 */
extern const uint16_t VPA_TOP;

/**
 * @brief The bottom address of Virtual Page B.
 * Only used in Simulated EEPROM 2.
 */
extern const uint16_t VPB_BASE;

/**
 * @brief The top address of Virtual Page B.
 * Only used in Simulated EEPROM 2.
 */
extern const uint16_t VPB_TOP;

/**
 * @brief The bottom address of Virtual Page C.
 * Only used in Simulated EEPROM 2.
 */
extern const uint16_t VPC_BASE;

/**
 * @brief The top address of Virtual Page C.
 * Only used in Simulated EEPROM 2.
 */
extern const uint16_t VPC_TOP;

/**
 * @brief The memory address at which point erasure requests transition
 * from being "GREEN" to "RED" when the freePtr crosses this address.
 * Only used in Simulated EEPROM 1.
 */
extern const uint16_t ERASE_CRITICAL_THRESHOLD;

#endif //__PLAT_SIM_EEPROM_H__

#endif //DOXYGEN_SHOULD_SKIP_THIS
