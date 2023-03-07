/***************************************************************************//**
 * @file app_dci_task.h
 * @brief DCI task functions.
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
#ifndef APP_DCI_TASK_H
#define APP_DCI_TASK_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <string.h>
#include "app_process.h"
#include "app_prog_error.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// AES-128 key size
#define AES_KEY_SIZE                    (16)

/// Public key size
#define PUB_KEY_SIZE                    (64)

/// HSE user configuration buffer size
#define HSE_USER_CONF_SIZE              (7)

/// VSE user configuration buffer size
#define VSE_USER_CONF_SIZE              (2)

/// Length of DCI commands
#define NO_ARGUMENT_LENGTH              (0x00000008UL)
#define UPGRADE_SE_FIRMWARE_LENGTH      (0x0000000CUL)
#define INIT_AES_KEY_LENGTH             (0x0000001CUL)
#define INIT_PUB_KEY_LENGTH             (0x0000004CUL)
#define INIT_VSE_OTP_LENGTH             (0x00000008UL)
#define INIT_HSE_OTP_LENGTH             (0x0000001CUL)

/// DCI commands
#define COMMAND_READ_SERIAL             (0xFE000000UL)
#define COMMAND_GET_SE_STATUS           (0xFE010000UL)
#define COMMAND_READ_PUB_SIGN_KEY       (0xFF080101UL)
#define COMMAND_READ_PUB_CMD_KEY        (0xFF080201UL)
#define COMMAND_READ_OTP                (0xFE040000UL)
#define COMMAND_DBG_LOCK_ENABLE_SECURE  (0x430D0000UL)
#define COMMAND_DBG_LOCK_DISABLE_SECURE (0x430E0000UL)
#define COMMAND_DBG_LOCK_APPLY          (0x430C0000UL)
#define COMMAND_DEVICE_ERASE            (0x430F0000UL)
#define COMMAND_CHECK_SE_IMAGE          (0x43020000UL)
#define COMMAND_APPLY_SE_IMAGE          (0x43030000UL)
#define COMMAND_INIT_AES_128_KEY        (0xFF0B0501UL)
#define COMMAND_INIT_PUB_SIGN_KEY       (0xFF070101UL)
#define COMMAND_INIT_PUB_CMD_KEY        (0xFF070201UL)
#define COMMAND_INIT_OTP                (0xFF000001UL)
#define COMMAND_DEVICE_ERASE_DISABLE    (0x43100000UL)

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Set up buffer to implement DCI command.
 *
 * @param cmd_buf Pointer to DCI command buffer.
 ******************************************************************************/
void set_dci_command(uint32_t *cmd_buf);

#endif  // APP_DCI_TASK_H
