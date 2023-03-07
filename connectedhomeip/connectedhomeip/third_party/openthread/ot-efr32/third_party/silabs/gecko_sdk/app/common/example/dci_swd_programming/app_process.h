/***************************************************************************//**
 * @file app_process.h
 * @brief Top level application functions.
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
#ifndef APP_PROCESS_H
#define APP_PROCESS_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <inttypes.h>
#include "em_cmu.h"
#include "app_dci_swd.h"
#include "app_dci_task.h"
#include "app_firmware_image.h"
#include "app_prog_error.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// State machine states
typedef enum {
  APP_ENTRY,
  SELECT_INTERFACE,
  SELECT_DCI_TASK,
  GET_SE_STATUS,
  READ_USER_CONFIG,
  READ_SERIAL_NUMBER,
  READ_PUB_SIGN_KEY,
  READ_PUB_CMD_KEY,
  ENABLE_SECURE_DEBUG,
  DISABLE_SECURE_DEBUG,
  LOCK_DEVICE,
  ERASE_DEVICE,
  RECOVER_DEVICE,
  PROG_SE_FIRMWARE,
  INIT_AES_KEY_HSE,
  INIT_PUB_SIGN_KEY,
  INIT_PUB_CMD_KEY,
  INIT_SE_OTP,
  DISABLE_DEVICE_ERASE,
  CHECK_SE_FIRMWARE,
  APPLY_SE_FIRMWARE,
  CONFIRM_SELECTION,
  SELECT_SWD_TASK,
  ERASE_MAIN_FLASH,
  PROG_MAIN_FLASH_APP,
  ERASE_USER_DATA,
  PROG_USER_DATA,
  PROG_MAIN_FLASH_SE_APP,
  INIT_SW_DP,
  GET_AP_ID,
  HALT_TARGET,
  GET_DEVICE_INFO,
  PROG_MAIN_FLASH_SE,
  PROG_MAIN_FLASH_SIGNED,
  APP_ERROR,
  APP_EXIT
} state_t;

/// Number of interfaces
#define INTERFACE_NUM           (2)

/// Number of DCI tasks
#define DCI_TASK_NUM            (16)

/// Number of SWD tasks
#define SWD_TASK_NUM            (5)

/// DCI number
#define DCI_SELECT              (0)

/// Boundary for command requires confirmation
#define CONFIRM_COMMAND         (4)

/// Boundary for one-time command
#define ONE_TIME_COMMAND        (10)

/// Recover device item number
#define RECOVER_SELECT          (9)

/// Device part number buffer size
#define DEV_NAME_SIZE           (30)

/// DCI command and response buffer size
#define CMD_RESP_SIZE           (30)

/// Number of tamper signals
#define TAMPER_SIGNAL_NUM       (32)

/// SE status response with tamper
#define RESP_WITH_TAMPER        (40)

/// User configuration response with tamper
#define RESP_TAMPER_CONF        (28)

/// Serial number size
#define SERIAL_NUM_SIZE         (16)

/// Array index to check HSE or VSE
#define DEVICE_INDEX            (8)

/// Array index to check SVM or SVH
#define VAULT_INDEX             (9)

/// xG21 device
#define DEVICE_XG21             (0x31)

/// xG22 device
#define DEVICE_XG22             (0x32)

/// xG23 device
#define DEVICE_XG23             (0x33)

/// SE version mask
#define SE_VER_MASK             (0x00ffffff)

/// SE version with boot status error code
#define SE_BOOT_ERR_VER         (0x00010020)

/// Invalid MCU firmware version
#define NO_MCU_VER              (0xffffffff)

/// Debug lock mask
#define DEBUG_LOCK_MASK         (0x01)

/// Debug lock state mask
#define DEBUG_LOCK_STATE_MASK   (0x20)

/// Device erase mask
#define DEVICE_ERASE_MASK       (0x02)

/// Secure debug mask
#define SECURE_DEBUG_MASK       (0x04)

/// Secure boot not configure
#define SECURE_BOOT_NOT_CONF    (0xffffffff)

/// Secure boot mask
#define SECURE_BOOT_MASK        (0x01)

/// Error code mask
#define ERROR_CODE_MASK         (0xff00)

/// Error code shift
#define ERROR_CODE_SHIFT        (8)

/// Boot status mask
#define BOOT_STATUS_MASK        (0xff)

/// Enter main loop
#define BOOT_MAIN_LOOP          (0x20)

/// Secure boot enable mask
#define SECURE_BOOT_ENABLE_MASK (0x00010000)

/// Secure boot certificate mask
#define SECURE_BOOT_CERT_MASK   (0x00020000)

/// Secure boot anti-rollback mask
#define ANTI_ROLLBACK_MASK      (0x00040000)

/// Page lock narrow mask
#define PAGE_LOCK_NARROW_MASK   (0x00080000)

/// Page lock full mask
#define PAGE_LOCK_FULL_MASK     (0x00100000)

/// Filter counter period mask
#define COUNTER_PERIOD_MASK     (0xff)

/// Filter counter threshold mask
#define COUNTER_THRESHOLD_MASK  (0xff00)

/// Filter counter threshold shift
#define COUNTER_THRESHOLD_SHIFT (8)

/// Glitch detector mask
#define GLITCH_DETECTOR_MASK    (0x00020000)

/// Keep tamper alive during sleep mask
#define SLEEP_ALIVE_MASK        (0x00040000)

/// Tamper reset threshold shift
#define TAMPER_RESET_SHIFT      (24)

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * Application state machine, called infinitely.
 ******************************************************************************/
void app_process_action(void);

#endif  // APP_PROCESS_H
