/***************************************************************************//**
 * @file app_se_manager_host_firmware_upgrade.c
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_se_manager_host_firmware_upgrade.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// Command context
static sl_se_command_context_t cmd_ctx;

/// Host firmware upgrade status
static uint32_t status;

/// Host or SE firmware version
static uint32_t version;

/// Debug lock status
static sl_se_debug_status_t debug_lock_status;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Get status data.
 ******************************************************************************/
uint32_t get_status(void)
{
  return(status);
}

/***************************************************************************//**
 * Get version data.
 ******************************************************************************/
uint32_t get_version(void)
{
  return(version);
}

/***************************************************************************//**
 * Get debug lock status pointer.
 ******************************************************************************/
sl_se_debug_status_t * get_debug_lock_status_ptr(void)
{
  return(&debug_lock_status);
}

/***************************************************************************//**
 * Initialize the SE Manager.
 ******************************************************************************/
sl_status_t init_se_manager(void)
{
  print_error_cycle(sl_se_init(), NULL);
}

/***************************************************************************//**
 * Deinitialize the SE Manager.
 ******************************************************************************/
sl_status_t deinit_se_manager(void)
{
  print_error_cycle(sl_se_deinit(), NULL);
}

/***************************************************************************//**
 * Get upgrade status of the host firmware image.
 ******************************************************************************/
sl_status_t get_host_upgrade_status(void)
{
  print_error_cycle(sl_se_get_upgrade_status_host_image(&cmd_ctx,
                                                        &status,
                                                        &version), &cmd_ctx);
}

/***************************************************************************//**
 * Validate the host firmware image.
 ******************************************************************************/
sl_status_t validate_host_image(void)
{
  print_error_cycle(sl_se_check_host_image(&cmd_ctx,
                                           (uint8_t *)get_host_firmware_addr(),
                                           get_host_firmware_size()),
                    &cmd_ctx);
}

/***************************************************************************//**
 * Upgrade the host firmware image.
 ******************************************************************************/
sl_status_t upgrade_host_image(void)
{
  print_error_cycle(sl_se_apply_host_image(&cmd_ctx,
                                           (uint8_t *)get_host_firmware_addr(),
                                           get_host_firmware_size()),
                    &cmd_ctx);
}

/***************************************************************************//**
 * Get the SE firmware version.
 ******************************************************************************/
sl_status_t get_se_version(void)
{
  print_error_cycle(sl_se_get_se_version(&cmd_ctx, &version), &cmd_ctx);
}

/***************************************************************************//**
 * Get the current debug lock status.
 ******************************************************************************/
sl_status_t get_debug_lock_status(void)
{
  print_error_cycle(sl_se_get_debug_lock_status(&cmd_ctx, &debug_lock_status),
                    &cmd_ctx);
}

/***************************************************************************//**
 * Enable the debug lock for the part.
 ******************************************************************************/
sl_status_t apply_debug_lock(void)
{
  print_error_cycle(sl_se_apply_debug_lock(&cmd_ctx), &cmd_ctx);
}

#if defined(CRYPTOACC_PRESENT)
/***************************************************************************//**
 * Get executed command from command context.
 ******************************************************************************/
uint32_t get_executed_command(void)
{
  return(cmd_ctx.command.command);
}

/***************************************************************************//**
 * Check if any command was executed.
 ******************************************************************************/
sl_status_t check_executed_command(void)
{
  // Keep command context to get the response
  sl_se_init_command_context(&cmd_ctx);

  // Read executed command if any
  print_error_cycle(sl_se_read_executed_command(&cmd_ctx), NULL);
}

/***************************************************************************//**
 * Get the executed command response.
 ******************************************************************************/
sl_status_t get_executed_command_response(uint8_t *buf, uint32_t buf_len)
{
  // Allocate buffer for reading the executed command response
  SE_DataTransfer_t out_buf = SE_DATATRANSFER_DEFAULT(buf, buf_len);

  // Set executed command and buffer to run sl_se_ack_command()
  SE_addDataOutput(&cmd_ctx.command, &out_buf);

  // Acknowledge and get status and output data of an executed command
  print_error_cycle(sl_se_ack_command(&cmd_ctx), NULL);
}
#endif

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
