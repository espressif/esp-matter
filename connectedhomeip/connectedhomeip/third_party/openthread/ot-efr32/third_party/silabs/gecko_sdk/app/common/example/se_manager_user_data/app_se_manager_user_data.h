/***************************************************************************//**
 * @file app_se_manager_user_data.h
 * @brief SE manager user data functions.
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
#ifndef APP_SE_MANAGER_USER_DATA_H
#define APP_SE_MANAGER_USER_DATA_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_se_manager_macro.h"
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
 * Get user data buffer pointer.
 *
 * @returns Returns pointer to user data buffer.
 ******************************************************************************/
uint32_t * get_user_data_buf_ptr(void);

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
 * Erase user data section in SE.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t erase_user_data(void);

/***************************************************************************//**
 * Write data to user data section in SE.
 *
 * @param offset The offset of the word aligned flash word to write to.
 * @param number The number of bytes to write to.
 * @returns Returns status code, @ref sl_status.h.
 *
 * @note Write data must be aligned to word size and contain a number of bytes
 *       that is divisible by four.
 ******************************************************************************/
sl_status_t write_user_data(uint32_t offset, uint32_t number);

#endif  // APP_SE_MANAGER_USER_DATA_H
