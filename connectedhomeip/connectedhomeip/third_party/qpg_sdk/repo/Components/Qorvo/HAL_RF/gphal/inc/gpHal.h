/*
 * Copyright (c) 2008-2016, GreenPeak Technologies
 * Copyright (c) 2017-2019, Qorvo Inc
 *
 * The GreenPeak Hardware Abstraction Layer (HAL) implements the API to control the GP chip
 *
 * The HAL code is subdivided in different C-files as the chip itself consists of different blocks.
 * In this way the user can quickly use the code of the block he needs.
 *
 *  The file gpHal.h contains the general functions of the HAL (init, reset, interrupts).
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * Alternatively, this software may be distributed under the terms of the
 * modified BSD License or the 3-clause BSD License as published by the Free
 * Software Foundation @ https://directory.fsf.org/wiki/License:BSD-3-Clause
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

#ifndef _HAL_GP_H_
#define _HAL_GP_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "gpHal_RomCode.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

/**
 * @file gpHal.h
 *
 * The general functions of the HAL including initialization, reset and interrupt mask control and MCU settings
 * are included in the file gpHal.h
 *
 * @brief The file gpHal.h contains the general functions of the HAL (init, reset, interrupts).
*/

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#ifdef GP_HAL_DIVERSITY_SINGLE_ANTENNA
#if (GP_HAL_DIVERSITY_SINGLE_ANTENNA!=0) && (GP_HAL_DIVERSITY_SINGLE_ANTENNA!=1)
#error error: gpHal GP_HAL_DIVERSITY_SINGLE_ANTENNA is unsupported single antenna
#endif
#endif

#ifndef GP_DIVERSITY_NR_OF_STACKS
//Overruled from build normally
#define GP_DIVERSITY_NR_OF_STACKS 1
#endif //GP_DIVERSITY_NR_OF_STACKS

#include "global.h"
#include "gp_global.h"

//Chip specific includes
#include "gpHal_reg.h"

//Different GP blocks
#include "gpHal_HW.h"


#if defined(GP_COMP_GPHAL_PBM)
#include "gpHal_Pbm.h"
#endif
#ifdef GP_COMP_GPHAL_MAC
#include "gpHal_MAC.h"          //All 802.15.4 RF related functions
#include "gpHal_MAC_Ext.h"
#endif //GP_COMP_GPHAL_MAC
#ifdef GP_COMP_GPHAL_BLE
#include "gpHal_Ble.h"          //All BLE related functions
#endif
#include "gpHal_ES.h"           //Event Scheduler functions
#ifdef GP_COMP_GPHAL_SEC
#include "gpHal_SEC.h"          //Security functions
#endif //GP_COMP_GPHAL_SEC
#include "gpHal_MISC.h"         //Miscellaneous functions
#include "gpHal_DP.h"           //Data Pending functionality
#include "gpHal_DPI.h"          //Deep Packet Inspection functionality

#if defined(GP_HAL_DIVERSITY_INCLUDE_IPC)
#include "gpHal_kx_Ipc.h"
#endif //GP_HAL_DIVERSITY_INCLUDE_IPC


/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define GP_HAL_DEFAULT_TIMEOUT  10000UL //10ms
#define GP_HAL_MAXIMUM_TIMEOUT  2097119UL // 2.097s - max 16-bit counter value shifted 5 positions -1

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                   Functional Macro Definitions
 *****************************************************************************/

#define GP_HAL_TIME_DIFF(t1,t2)                   (t2 <= t1 ? (t1 - t2) : (0xFFFFFFFF - (t1 - t2)))
/** @brief Compares times from the chip's timebase - check if t1 < t2
*/
#define GP_HAL_TIME_COMPARE_LOWER(t1,t2)          (!((UInt32)((t1)  - (t2))/*&(0xFFFFFFFF)*/ < (0x80000000LU)))
/** @brief Compares times from the chip's timebase - check if t1 >= t2
*/
#define GP_HAL_TIME_COMPARE_BIGGER_EQUAL(t1,t2)   ( ((UInt32)((t1)  - (t2))/*&(0xFFFFFFFF)*/ < (0x80000000LU)))
/** @brief Compares times from the chip's timebase - check if t1 > t2
*/
#define GP_HAL_TIME_COMPARE_BIGGER(t1,t2)         (!((UInt32)((t2)  - (t1))/*&(0xFFFFFFFF)*/ < (0x80000000LU)))
/** @brief Compares times from the chip's timebase - check if t1 <= t2
*/
#define GP_HAL_TIME_COMPARE_LOWER_EQUAL(t1,t2)    ( ((UInt32)((t2)  - (t1))/*&(0xFFFFFFFF)*/ < (0x80000000LU)))

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpHal_CodeJumpTableFlash_Defs.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */

/** @brief Reads a register of the GP chip.
 *
 *  Reads a register of the GP chip.
 *  @param Register The register address to read data from.
*/
GP_API UInt8 gpHal_ReadReg(gpHal_Address_t Register);
/** @brief Reads a block of registers of the GP chip.
 *
 *  Reads a block of registers of the GP chip.
 *  @param Address The register address where the block read starts.
 *  @param pBuffer The pointer to a byte buffer where the read data will be stored.
 *  @param Length  The number of bytes to be read.
*/
GP_API void gpHal_ReadRegs(gpHal_Address_t Address, void* pBuffer, UInt8 Length);

/** @brief Write a register of the GP chip.
 *
 *  Write a register of the GP chip.
 *  @param Register The register address to write to.
 *  @param Data     The data to write to the register.
*/
GP_API void gpHal_WriteReg(gpHal_Address_t Register, UInt8 Data);
/**
 *  @brief Writes a block of registers to the GP chip.
 *
 *  Writes a block of registers to the GP chip.
 *  @param Address The register address where the block write starts.
 *  @param pBuffer The pointer to a byte buffer where the data to be written are stored.
 *  @param Length  The number of bytes to be written.
*/
GP_API void gpHal_WriteRegs(gpHal_Address_t Address, void* pBuffer, UInt8 Length);

// AND register contents with inverted mask, OR in data, and write the result
/** @brief Reads a register, modifies the data with a certain mask and data, writes the register back. */
GP_API void gpHal_ReadModifyWriteReg(gpHal_Address_t Register, UInt8 Mask, UInt8 Data);

/** @brief Checks if MSI communication is possible and correct by reading a known register. */
GP_API Bool gpHal_CheckMsi(void);

/**
 *  @brief Returns the version information of the chip
 *
 *  This function returns the version information of the chip. It indicates the current revision of the chip and other information
 *
*/
GP_API UInt16 gpHal_GetHWVersionId(void);

/**
 * @brief Getter method for the chip ID.
 * @return The identifier of the silicon.
*/
#define gpHal_GetChipId() (GP_HAL_EXPECTED_CHIP_ID)

/**
 * @brief Getter method for the chip version.
 * @return The metal fix version of the chip.
*/
GP_API UInt8 gpHal_GetChipVersion(void);

/**
 * @brief Getter method for the ROM BL version.
 *
 * Note that this function returns -on purpose- only the MSB of the ROM bootloader version.
 *
 * @return The version of the ROM bootloader.
*/
GP_API UInt8 gpHal_GetRomBlVersion(void);

/**
 * @brief Initializes HAL variables and sets basic GP chip register values.
 *
 * The function has to be called at the beginning of the application as it initializes variables that are used throughout operation.
 * At startup a decision is made to use the timed operation of the GP chip or not, by setting the parameter timedMAC.
 *
 * @param timedMAC If set to true, the GP chip will be used with a timed MAC.
 *                 When a timed MAC is used all transmission is done using scheduled triggers from the Event Scheduler (ES).
 *
*/
GP_API void gpHal_Init(Bool timedMAC);

/**
 * @brief Initializes Advanced HAL variables
 *
 * The function has to be called at the end of the application initialization. In comparison with the basic gpHal_Init()
 * method, the settings triggered by this initialization could interfere with initializations of other components.
 *
*/
GP_API void gpHal_AdvancedInit(void);

/**
 * @brief This function performs a reset of the chip.
 *
 * This function performs a reset of the functional registers. All registers are reverted to their default value and all memories are cleared.
 *
*/
GP_API void gpHal_Reset(void);
/**
 * @brief This function checks if radio is awake
 * @return Returns if value is awake. If value is 0, chip is asleep else the device is awake.
 *
*/
GP_API UInt8 gpHal_IsRadioAccessible(void);
/**
 * @brief This function detects if a reset of the GP chip has occurred.
 *
 * This function will return true if the GP chip has resetted since its first startup.
 *
*/
GP_API Bool gpHal_DidGPReset(void);
/**
 * @brief Thie functions enables the interrupt line of the GP chip.
 *
 * Sets the main interrupt mask of the GP chip.
 *
 * @param enable If set to true: main interrupt mask is switched on.
*/
#define gpHal_EnableInterrupts(enable) GP_HAL_ENABLE_INTERRUPTS(enable)

// Main interrupt service routine - to be registered by user using HAL_GP_INTERRUPT_INIT()
/**
 * @brief The interrupt service routine to be called when the interrupt of the GP chip is seen.
 *
 * This ISR needs to be called when the interrupt line (INTOUTn) goes low (active low signal).
 * It will handle the pending interrupt according to the callback functions registered to the different sources.
 *
*/
GP_API void gpHal_Interrupt(void);

/**
 * @brief This function regulates the GP chip sleep behaviour
 *
 * This function enables the sleep mode of the GP chip when the GP chip is idle.
 * Enabling GoToSleepWhenIdle will cause the GP chip to go to sleep mode when nothing is busy (TX/RX/Receiver on/...)
 * Be aware that the GP chip remains in sleep if no wakeup trigger or event is enabled and registered.
 *
 * It also regulates the GP chip sleep behaviour by keeping track of a "stay awake counter".
 * When the counter reaches '0' the GP chip is put into sleep.
 * It always needs to be used in pairs, one to wake up the GP chip when its needed and one to set it to sleep when the GP chip is no longer of use.
 * In this way different functions can use the sleep functionality without overriding each others settings.
 *
 * @param enable
 *                  - if set to true : The GP chip will be put to sleep if no other wake requests are pending, counter decremented
 *                  - if set to false: The GP chip will be kept awake, counter incremented
*/
GP_API void gpHal_GoToSleepWhenIdle(Bool enable);

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */

#ifdef __cplusplus
}
#endif

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif //_HAL_GP_H_
