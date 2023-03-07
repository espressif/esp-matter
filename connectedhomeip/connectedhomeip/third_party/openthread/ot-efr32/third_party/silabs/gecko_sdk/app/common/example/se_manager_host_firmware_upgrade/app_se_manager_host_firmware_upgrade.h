/***************************************************************************//**
 * @file app_se_manager_host_firmware_upgrade.h
 * @brief SE manager host firmware upgrade functions.
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
#ifndef APP_SE_MANAGER_HOST_FIRMWARE_UPGRADE_H
#define APP_SE_MANAGER_HOST_FIRMWARE_UPGRADE_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_se_manager_macro.h"
#include "app_host_firmware_image.h"
#include "sl_se_manager.h"
#include "sl_se_manager_util.h"
#include <stdint.h>
#include <stdio.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Get status data.
 *
 * @returns Returns the status.
 ******************************************************************************/
uint32_t get_status(void);

/***************************************************************************//**
 * Get version data.
 *
 * @returns Returns the version.
 ******************************************************************************/
uint32_t get_version(void);

/***************************************************************************//**
 * Get debug lock status pointer.
 *
 * @returns Returns pointer to debug lock status.
 ******************************************************************************/
sl_se_debug_status_t * get_debug_lock_status_ptr(void);

/***************************************************************************//**
 * Initialize the SE Manager.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t init_se_manager(void);

/***************************************************************************//**
 * Deinitialize the SE Manager.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t deinit_se_manager(void);

/***************************************************************************//**
 * Get upgrade status of the host firmware image.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t get_host_upgrade_status(void);

/***************************************************************************//**
 * Validate the host firmware image.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t validate_host_image(void);

/***************************************************************************//**
 * Upgrade the host firmware image.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t upgrade_host_image(void);

/***************************************************************************//**
 * Get the SE firmware version.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t get_se_version(void);

/***************************************************************************//**
 * Get the current debug lock status.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t get_debug_lock_status(void);

/***************************************************************************//**
 * Enable the debug lock for the part.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t apply_debug_lock(void);

#if defined(CRYPTOACC_PRESENT)
/***************************************************************************//**
 * Get executed command from command context.
 *
 * @returns The command in command context.
 ******************************************************************************/
uint32_t get_executed_command(void);

/***************************************************************************//**
 * Check if any command was executed.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t check_executed_command(void);

/***************************************************************************//**
 * Get the executed command response.
 *
 * @param buf Pointer to the response buffer.
 * @param buf_len The size of the response buffer.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t get_executed_command_response(uint8_t *buf, uint32_t buf_len);
#endif

#endif  // APP_SE_MANAGER_HOST_FIRMWARE_UPGRADE_H
