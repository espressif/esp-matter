/***************************************************************************//**
 * @file
 * @brief Configuration header for bootloader
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef BTL_STORAGE_SLOT_CONFIG_H
#define BTL_STORAGE_SLOT_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Bootloader Storage Slot Setup
/******************s*************************************************************
 *
 * Configuration for Memory addresses for the bootloader.
 *
 *
 ******************************************************************************/
// <e SLOT0_ENABLE> Enable Slot 0
// <i> Default: 1
#define SLOT0_ENABLE  0

// <o SLOT0_START> Start Address
// <f .h>
#define SLOT0_START   0

// <o SLOT0_SIZE> Slot Size
// <f .h>
#define SLOT0_SIZE 65536

// <e SLOT1_ENABLE> Enable Slot 1
// <i> Default: 0
#define SLOT1_ENABLE  0

// <o SLOT1_START> Start Address
// <f .h>
#define SLOT1_START 0

// <o SLOT1_SIZE> Slot Size
// <f .h>
#define SLOT1_SIZE 69632

// <e SLOT2_ENABLE> Enable Slot 2
// <i> Default: 0
#define SLOT2_ENABLE  0

// <o SLOT2_START> Start Address
// <f .h>
#define SLOT2_START 0

// <o SLOT2_SIZE> Slot Size
// <f .h>
#define SLOT2_SIZE 73728
// </e>
// </e>
// </e>
// </h>

#if (SLOT0_ENABLE == 1 && SLOT1_ENABLE == 1 && SLOT2_ENABLE == 1)
    #define BTL_STORAGE_NUM_SLOTS (3)
    #define BTL_STORAGE_SLOTS                   \
  {                                             \
    { SLOT0_START, SLOT0_SIZE },   /* Slot 0 */ \
    { SLOT1_START, SLOT1_SIZE },   /* Slot 1 */ \
    { SLOT2_START, SLOT2_SIZE },   /* Slot 2 */ \
  }                                             \
  // Number of slots in bootload list
    #define BTL_STORAGE_BOOTLOAD_LIST_LENGTH BTL_STORAGE_NUM_SLOTS
#else
#if (SLOT0_ENABLE == 1 && SLOT1_ENABLE == 1)
    #define BTL_STORAGE_NUM_SLOTS (2)
    #define BTL_STORAGE_SLOTS                   \
  {                                             \
    { SLOT0_START, SLOT0_SIZE },   /* Slot 0 */ \
    { SLOT1_START, SLOT1_SIZE },   /* Slot 1 */ \
  }                                             \
  // Number of slots in bootload list
    #define BTL_STORAGE_BOOTLOAD_LIST_LENGTH BTL_STORAGE_NUM_SLOTS

#else
#if (SLOT0_ENABLE == 1)
    #define BTL_STORAGE_NUM_SLOTS (1)
    #define BTL_STORAGE_SLOTS                 \
  {                                           \
    { SLOT0_START, SLOT0_SIZE }, /* Slot 0 */ \
  }                                           \
  // Number of slots in bootload list
    #define BTL_STORAGE_BOOTLOAD_LIST_LENGTH BTL_STORAGE_NUM_SLOTS
#else
    #define BTL_STORAGE_NUM_SLOTS (0)
    #define BTL_STORAGE_SLOTS \
  {                           \
    { 0, 0 },                 \
  }                           \
  // Number of slots in bootload list
    #define BTL_STORAGE_BOOTLOAD_LIST_LENGTH BTL_STORAGE_NUM_SLOTS
#endif
#endif
#endif

// <<< end of configuration section >>>

#endif // BTL_STORAGE_SLOT_CONFIG_H
