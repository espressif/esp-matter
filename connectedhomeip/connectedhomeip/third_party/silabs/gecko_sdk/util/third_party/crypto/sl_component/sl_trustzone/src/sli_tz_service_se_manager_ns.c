/***************************************************************************//**
 * @file
 * @brief TrustZone secure SE Manager service (non-secure side).
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include "psa/error.h"

#include "sli_tz_ns_interface.h"
#include "sli_tz_s_interface.h"

#include "sli_tz_service_se_manager.h"
#include "sl_se_manager.h"
#include "sl_se_manager_util.h"

#include "em_device.h"

//------------------------------------------------------------------------------
// Function definitions

/***************************************************************************//**
 * Dummy implementation for init.
 ******************************************************************************/
sl_status_t sl_se_init(void)
{
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Dummy implementation for deinit.
 ******************************************************************************/
sl_status_t sl_se_deinit(void)
{
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Validate SE firmware image.
 ******************************************************************************/
sl_status_t sl_se_check_se_image(sl_se_command_context_t *cmd_ctx,
                                 void *image_addr)
{
  sl_status_t sl_status;
  sli_tz_fn_id fn_id = SLI_TZ_SERVICE_SE_MANAGER_CHECKSEIMAGE_SID;
  psa_invec in_vec[] = {
    { .base = &fn_id, .len = sizeof(sli_tz_fn_id) },
    { cmd_ctx, sizeof(cmd_ctx) },
    { image_addr, sizeof(image_addr) },
  };
  psa_outvec out_vec[] = {
    { &sl_status, sizeof(sl_status) },
  };

  psa_status_t psa_status =
    sli_tz_ns_interface_dispatch((sli_tz_veneer_fn)sli_tz_s_interface_dispatch_se_manager,
                                 (uint32_t)in_vec,
                                 IOVEC_LEN(in_vec),
                                 (uint32_t)out_vec,
                                 IOVEC_LEN(out_vec));

  // A PSA error is returned if parameter validation failed. Otherwise the
  // return status will be in the sl_status.
  if (psa_status != PSA_SUCCESS) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return sl_status;
}

/***************************************************************************//**
 * Apply SE firmware image.
 ******************************************************************************/
sl_status_t sl_se_apply_se_image(sl_se_command_context_t *cmd_ctx,
                                 void *image_addr)
{
  sl_status_t sl_status;
  sli_tz_fn_id fn_id = SLI_TZ_SERVICE_SE_MANAGER_APPLYSEIMAGE_SID;
  psa_invec in_vec[] = {
    { .base = &fn_id, .len = sizeof(sli_tz_fn_id) },
    { cmd_ctx, sizeof(cmd_ctx) },
    { image_addr, sizeof(image_addr) },
  };
  psa_outvec out_vec[] = {
    { &sl_status, sizeof(sl_status) },
  };

  psa_status_t psa_status =
    sli_tz_ns_interface_dispatch((sli_tz_veneer_fn)sli_tz_s_interface_dispatch_se_manager,
                                 (uint32_t)in_vec,
                                 IOVEC_LEN(in_vec),
                                 (uint32_t)out_vec,
                                 IOVEC_LEN(out_vec));

  // A PSA error is returned if parameter validation failed. Otherwise the
  // return status will be in the sl_status.
  if (psa_status != PSA_SUCCESS) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return sl_status;
}

/***************************************************************************//**
 * Get upgrade status of SE firmware image.
 ******************************************************************************/
sl_status_t sl_se_get_upgrade_status_se_image(sl_se_command_context_t *cmd_ctx,
                                              uint32_t *status,
                                              uint32_t *prev_version)
{
  sl_status_t sl_status;
  sli_tz_fn_id fn_id = SLI_TZ_SERVICE_SE_MANAGER_GETUPGRADESTATUSSEIMAGE_SID;
  psa_invec in_vec[] = {
    { .base = &fn_id, .len = sizeof(sli_tz_fn_id) },
    { cmd_ctx, sizeof(cmd_ctx) },
    { status, sizeof(status) },
    { prev_version, sizeof(prev_version) },
  };
  psa_outvec out_vec[] = {
    { &sl_status, sizeof(sl_status) },
  };

  psa_status_t psa_status =
    sli_tz_ns_interface_dispatch((sli_tz_veneer_fn)sli_tz_s_interface_dispatch_se_manager,
                                 (uint32_t)in_vec,
                                 IOVEC_LEN(in_vec),
                                 (uint32_t)out_vec,
                                 IOVEC_LEN(out_vec));

  // A PSA error is returned if parameter validation failed. Otherwise the
  // return status will be in the sl_status.
  if (psa_status != PSA_SUCCESS) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return sl_status;
}

/***************************************************************************//**
 * Validate Host firmware image.
 ******************************************************************************/
sl_status_t sl_se_check_host_image(sl_se_command_context_t *cmd_ctx,
                                   void *image_addr,
                                   uint32_t size)
{
  sl_status_t sl_status;
  sli_tz_fn_id fn_id = SLI_TZ_SERVICE_SE_MANAGER_CHECKHOSTIMAGE_SID;
  psa_invec in_vec[] = {
    { .base = &fn_id, .len = sizeof(sli_tz_fn_id) },
    { cmd_ctx, sizeof(cmd_ctx) },
    { image_addr, size },
  };
  psa_outvec out_vec[] = {
    { &sl_status, sizeof(sl_status) },
  };

  psa_status_t psa_status =
    sli_tz_ns_interface_dispatch((sli_tz_veneer_fn)sli_tz_s_interface_dispatch_se_manager,
                                 (uint32_t)in_vec,
                                 IOVEC_LEN(in_vec),
                                 (uint32_t)out_vec,
                                 IOVEC_LEN(out_vec));

  // A PSA error is returned if parameter validation failed. Otherwise the
  // return status will be in the sl_status.
  if (psa_status != PSA_SUCCESS) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return sl_status;
}

/***************************************************************************//**
 * Apply Host firmware image.
 ******************************************************************************/
sl_status_t sl_se_apply_host_image(sl_se_command_context_t *cmd_ctx,
                                   void *image_addr,
                                   uint32_t size)
{
  sl_status_t sl_status;
  sli_tz_fn_id fn_id = SLI_TZ_SERVICE_SE_MANAGER_APPLYHOSTIMAGE_SID;
  psa_invec in_vec[] = {
    { .base = &fn_id, .len = sizeof(sli_tz_fn_id) },
    { cmd_ctx, sizeof(cmd_ctx) },
    { image_addr, size },
  };
  psa_outvec out_vec[] = {
    { &sl_status, sizeof(sl_status) },
  };

  psa_status_t psa_status =
    sli_tz_ns_interface_dispatch((sli_tz_veneer_fn)sli_tz_s_interface_dispatch_se_manager,
                                 (uint32_t)in_vec,
                                 IOVEC_LEN(in_vec),
                                 (uint32_t)out_vec,
                                 IOVEC_LEN(out_vec));

  // A PSA error is returned if parameter validation failed. Otherwise the
  // return status will be in the sl_status.
  if (psa_status != PSA_SUCCESS) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return sl_status;
}

/***************************************************************************//**
 * Get upgrade status of Host firmware image.
 ******************************************************************************/
sl_status_t sl_se_get_upgrade_status_host_image(sl_se_command_context_t *cmd_ctx,
                                                uint32_t *status,
                                                uint32_t *prev_version)
{
  sl_status_t sl_status;
  sli_tz_fn_id fn_id = SLI_TZ_SERVICE_SE_MANAGER_GETUPGRADESTATUSHOSTIMAGE_SID;
  psa_invec in_vec[] = {
    { .base = &fn_id, .len = sizeof(sli_tz_fn_id) },
    { cmd_ctx, sizeof(cmd_ctx) },
    { status, sizeof(status) },
    { prev_version, sizeof(prev_version) },
  };
  psa_outvec out_vec[] = {
    { &sl_status, sizeof(sl_status) },
  };

  psa_status_t psa_status =
    sli_tz_ns_interface_dispatch((sli_tz_veneer_fn)sli_tz_s_interface_dispatch_se_manager,
                                 (uint32_t)in_vec,
                                 IOVEC_LEN(in_vec),
                                 (uint32_t)out_vec,
                                 IOVEC_LEN(out_vec));

  // A PSA error is returned if parameter validation failed. Otherwise the
  // return status will be in the sl_status.
  if (psa_status != PSA_SUCCESS) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return sl_status;
}

/***************************************************************************//**
 * Initialize key to be stored in the SE OTP flash.
 ******************************************************************************/
sl_status_t sl_se_init_otp_key(sl_se_command_context_t *cmd_ctx,
                               sl_se_device_key_type_t key_type,
                               void *key,
                               uint32_t num_bytes)
{
  sl_status_t sl_status;
  sli_tz_fn_id fn_id = SLI_TZ_SERVICE_SE_MANAGER_INITOTPKEY_SID;
  psa_invec in_vec[] = {
    { .base = &fn_id, .len = sizeof(sli_tz_fn_id) },
    { cmd_ctx, sizeof(cmd_ctx) },
    { &key_type, sizeof(key_type) },
    { key, num_bytes },
  };
  psa_outvec out_vec[] = {
    { &sl_status, sizeof(sl_status) },
  };

  psa_status_t psa_status =
    sli_tz_ns_interface_dispatch((sli_tz_veneer_fn)sli_tz_s_interface_dispatch_se_manager,
                                 (uint32_t)in_vec,
                                 IOVEC_LEN(in_vec),
                                 (uint32_t)out_vec,
                                 IOVEC_LEN(out_vec));

  // A PSA error is returned if parameter validation failed. Otherwise the
  // return status will be in the sl_status.
  if (psa_status != PSA_SUCCESS) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return sl_status;
}

/***************************************************************************//**
 * Read a public key stored in the SE.
 ******************************************************************************/
sl_status_t sl_se_read_pubkey(sl_se_command_context_t *cmd_ctx,
                              sl_se_device_key_type_t key_type,
                              void *key,
                              uint32_t num_bytes)
{
  sl_status_t sl_status;
  sli_tz_fn_id fn_id = SLI_TZ_SERVICE_SE_MANAGER_READPUBKEY_SID;
  psa_invec in_vec[] = {
    { .base = &fn_id, .len = sizeof(sli_tz_fn_id) },
    { cmd_ctx, sizeof(cmd_ctx) },
    { &key_type, sizeof(key_type) },
  };
  psa_outvec out_vec[] = {
    { &sl_status, sizeof(sl_status) },
    { key, num_bytes },
  };

  psa_status_t psa_status =
    sli_tz_ns_interface_dispatch((sli_tz_veneer_fn)sli_tz_s_interface_dispatch_se_manager,
                                 (uint32_t)in_vec,
                                 IOVEC_LEN(in_vec),
                                 (uint32_t)out_vec,
                                 IOVEC_LEN(out_vec));

  // A PSA error is returned if parameter validation failed. Otherwise the
  // return status will be in the sl_status.
  if (psa_status != PSA_SUCCESS) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return sl_status;
}

/***************************************************************************//**
 * Initialize SE OTP configuration.
 ******************************************************************************/
sl_status_t sl_se_init_otp(sl_se_command_context_t *cmd_ctx,
                           sl_se_otp_init_t *otp_init)
{
  sl_status_t sl_status;
  sli_tz_fn_id fn_id = SLI_TZ_SERVICE_SE_MANAGER_INITOTP_SID;
  psa_invec in_vec[] = {
    { .base = &fn_id, .len = sizeof(sli_tz_fn_id) },
    { cmd_ctx, sizeof(cmd_ctx) },
    { otp_init, sizeof(otp_init) },
  };
  psa_outvec out_vec[] = {
    { &sl_status, sizeof(sl_status) },
  };

  psa_status_t psa_status =
    sli_tz_ns_interface_dispatch((sli_tz_veneer_fn)sli_tz_s_interface_dispatch_se_manager,
                                 (uint32_t)in_vec,
                                 IOVEC_LEN(in_vec),
                                 (uint32_t)out_vec,
                                 IOVEC_LEN(out_vec));

  // A PSA error is returned if parameter validation failed. Otherwise the
  // return status will be in the sl_status.
  if (psa_status != PSA_SUCCESS) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return sl_status;
}

/***************************************************************************//**
 * Read SE OTP configuration.
 ******************************************************************************/
sl_status_t sl_se_read_otp(sl_se_command_context_t *cmd_ctx,
                           sl_se_otp_init_t *otp_settings)
{
  sl_status_t sl_status;
  sli_tz_fn_id fn_id = SLI_TZ_SERVICE_SE_MANAGER_READOTP_SID;
  psa_invec in_vec[] = {
    { .base = &fn_id, .len = sizeof(sli_tz_fn_id) },
    { cmd_ctx, sizeof(cmd_ctx) },
  };
  psa_outvec out_vec[] = {
    { &sl_status, sizeof(sl_status) },
    { otp_settings, sizeof(otp_settings) },
  };

  psa_status_t psa_status =
    sli_tz_ns_interface_dispatch((sli_tz_veneer_fn)sli_tz_s_interface_dispatch_se_manager,
                                 (uint32_t)in_vec,
                                 IOVEC_LEN(in_vec),
                                 (uint32_t)out_vec,
                                 IOVEC_LEN(out_vec));

  // A PSA error is returned if parameter validation failed. Otherwise the
  // return status will be in the sl_status.
  if (psa_status != PSA_SUCCESS) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return sl_status;
}

/***************************************************************************//**
 * Read the SE firmware version.
 ******************************************************************************/
sl_status_t sl_se_get_se_version(sl_se_command_context_t *cmd_ctx,
                                 uint32_t *version)
{
  sl_status_t sl_status;
  sli_tz_fn_id fn_id = SLI_TZ_SERVICE_SE_MANAGER_GETSEVERSION_SID;
  psa_invec in_vec[] = {
    { .base = &fn_id, .len = sizeof(sli_tz_fn_id) },
    { cmd_ctx, sizeof(cmd_ctx) },
  };
  psa_outvec out_vec[] = {
    { &sl_status, sizeof(sl_status) },
    { version, sizeof(version) },
  };

  psa_status_t psa_status =
    sli_tz_ns_interface_dispatch((sli_tz_veneer_fn)sli_tz_s_interface_dispatch_se_manager,
                                 (uint32_t)in_vec,
                                 IOVEC_LEN(in_vec),
                                 (uint32_t)out_vec,
                                 IOVEC_LEN(out_vec));

  // A PSA error is returned if parameter validation failed. Otherwise the
  // return status will be in the sl_status.
  if (psa_status != PSA_SUCCESS) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return sl_status;
}

/***************************************************************************//**
 * Returns the current debug lock configuration.
 ******************************************************************************/
sl_status_t sl_se_get_debug_lock_status(sl_se_command_context_t *cmd_ctx,
                                        sl_se_debug_status_t *debug_status)
{
  sl_status_t sl_status;
  sli_tz_fn_id fn_id = SLI_TZ_SERVICE_SE_MANAGER_GETDEBUGLOCKSTATUS_SID;
  psa_invec in_vec[] = {
    { .base = &fn_id, .len = sizeof(sli_tz_fn_id) },
    { cmd_ctx, sizeof(cmd_ctx) },
  };
  psa_outvec out_vec[] = {
    { &sl_status, sizeof(sl_status) },
    { debug_status, sizeof(debug_status) },
  };

  psa_status_t psa_status =
    sli_tz_ns_interface_dispatch((sli_tz_veneer_fn)sli_tz_s_interface_dispatch_se_manager,
                                 (uint32_t)in_vec,
                                 IOVEC_LEN(in_vec),
                                 (uint32_t)out_vec,
                                 IOVEC_LEN(out_vec));

  // A PSA error is returned if parameter validation failed. Otherwise the
  // return status will be in the sl_status.
  if (psa_status != PSA_SUCCESS) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return sl_status;
}

/***************************************************************************//**
 * Enables the debug lock for the part.
 ******************************************************************************/
sl_status_t sl_se_apply_debug_lock(sl_se_command_context_t *cmd_ctx)
{
  sl_status_t sl_status;
  sli_tz_fn_id fn_id = SLI_TZ_SERVICE_SE_MANAGER_APPLYDEBUGLOCK_SID;
  psa_invec in_vec[] = {
    { .base = &fn_id, .len = sizeof(sli_tz_fn_id) },
    { cmd_ctx, sizeof(cmd_ctx) },
  };
  psa_outvec out_vec[] = {
    { &sl_status, sizeof(sl_status) },
  };

  psa_status_t psa_status =
    sli_tz_ns_interface_dispatch((sli_tz_veneer_fn)sli_tz_s_interface_dispatch_se_manager,
                                 (uint32_t)in_vec,
                                 IOVEC_LEN(in_vec),
                                 (uint32_t)out_vec,
                                 IOVEC_LEN(out_vec));

  // A PSA error is returned if parameter validation failed. Otherwise the
  // return status will be in the sl_status.
  if (psa_status != PSA_SUCCESS) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return sl_status;
}

#if defined(CRYPTOACC_PRESENT)

/***************************************************************************//**
 * From VSE mailbox read which command, if any, was executed.
 ******************************************************************************/
sl_status_t sl_se_read_executed_command(sl_se_command_context_t *cmd_ctx)
{
  sl_status_t sl_status;
  sli_tz_fn_id fn_id = SLI_TZ_SERVICE_SE_MANAGER_READEXECUTEDCOMMAND_SID;
  psa_invec in_vec[] = {
    { .base = &fn_id, .len = sizeof(sli_tz_fn_id) },
    { cmd_ctx, sizeof(cmd_ctx) },
  };
  psa_outvec out_vec[] = {
    { &sl_status, sizeof(sl_status) },
  };

  psa_status_t psa_status =
    sli_tz_ns_interface_dispatch((sli_tz_veneer_fn)sli_tz_s_interface_dispatch_se_manager,
                                 (uint32_t)in_vec,
                                 IOVEC_LEN(in_vec),
                                 (uint32_t)out_vec,
                                 IOVEC_LEN(out_vec));

  // A PSA error is returned if parameter validation failed. Otherwise the
  // return status will be in the sl_status.
  if (psa_status != PSA_SUCCESS) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return sl_status;
}

/***************************************************************************//**
 * Acknowledge and get status and output data of a completed command.
 ******************************************************************************/
sl_status_t sl_se_ack_command(sl_se_command_context_t *cmd_ctx)
{
  sl_status_t sl_status;
  sli_tz_fn_id fn_id = SLI_TZ_SERVICE_SE_MANAGER_ACKCOMMAND_SID;
  psa_invec in_vec[] = {
    { .base = &fn_id, .len = sizeof(sli_tz_fn_id) },
    { cmd_ctx, sizeof(cmd_ctx) },
  };
  psa_outvec out_vec[] = {
    { &sl_status, sizeof(sl_status) },
  };

  psa_status_t psa_status =
    sli_tz_ns_interface_dispatch((sli_tz_veneer_fn)sli_tz_s_interface_dispatch_se_manager,
                                 (uint32_t)in_vec,
                                 IOVEC_LEN(in_vec),
                                 (uint32_t)out_vec,
                                 IOVEC_LEN(out_vec));

  // A PSA error is returned if parameter validation failed. Otherwise the
  // return status will be in the sl_status.
  if (psa_status != PSA_SUCCESS) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return sl_status;
}

#endif //defined(CRYPTOACC_PRESENT)

#if defined(SEMAILBOX_PRESENT)

/***************************************************************************//**
 * Returns the current boot status, versions and system configuration.
 ******************************************************************************/
sl_status_t sl_se_get_status(sl_se_command_context_t *cmd_ctx,
                             sl_se_status_t *status)
{
  sl_status_t sl_status;
  sli_tz_fn_id fn_id = SLI_TZ_SERVICE_SE_MANAGER_GETSTATUS_SID;
  psa_invec in_vec[] = {
    { .base = &fn_id, .len = sizeof(sli_tz_fn_id) },
    { cmd_ctx, sizeof(cmd_ctx) },
  };
  psa_outvec out_vec[] = {
    { &sl_status, sizeof(sl_status) },
    { status, sizeof(status) },
  };

  psa_status_t psa_status =
    sli_tz_ns_interface_dispatch((sli_tz_veneer_fn)sli_tz_s_interface_dispatch_se_manager,
                                 (uint32_t)in_vec,
                                 IOVEC_LEN(in_vec),
                                 (uint32_t)out_vec,
                                 IOVEC_LEN(out_vec));

  // A PSA error is returned if parameter validation failed. Otherwise the
  // return status will be in the sl_status.
  if (psa_status != PSA_SUCCESS) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return sl_status;
}

/***************************************************************************//**
 * Read the serial number of the SE module.
 ******************************************************************************/
sl_status_t sl_se_get_serialnumber(sl_se_command_context_t *cmd_ctx,
                                   void *serial)
{
  sl_status_t sl_status;
  sli_tz_fn_id fn_id = SLI_TZ_SERVICE_SE_MANAGER_GETSERIALNUMBER_SID;
  psa_invec in_vec[] = {
    { .base = &fn_id, .len = sizeof(sli_tz_fn_id) },
    { cmd_ctx, sizeof(cmd_ctx) },
  };
  psa_outvec out_vec[] = {
    { &sl_status, sizeof(sl_status) },
    { serial, sizeof(serial) },
  };

  psa_status_t psa_status =
    sli_tz_ns_interface_dispatch((sli_tz_veneer_fn)sli_tz_s_interface_dispatch_se_manager,
                                 (uint32_t)in_vec,
                                 IOVEC_LEN(in_vec),
                                 (uint32_t)out_vec,
                                 IOVEC_LEN(out_vec));

  // A PSA error is returned if parameter validation failed. Otherwise the
  // return status will be in the sl_status.
  if (psa_status != PSA_SUCCESS) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return sl_status;
}

/***************************************************************************//**
 * Read the OTP firmware version of the SE module.
 ******************************************************************************/
sl_status_t sl_se_get_otp_version(sl_se_command_context_t *cmd_ctx,
                                  uint32_t *version)
{
  sl_status_t sl_status;
  sli_tz_fn_id fn_id = SLI_TZ_SERVICE_SE_MANAGER_GETOTPVERSION_SID;
  psa_invec in_vec[] = {
    { .base = &fn_id, .len = sizeof(sli_tz_fn_id) },
    { cmd_ctx, sizeof(cmd_ctx) },
  };
  psa_outvec out_vec[] = {
    { &sl_status, sizeof(sl_status) },
    { version, sizeof(version) },
  };

  psa_status_t psa_status =
    sli_tz_ns_interface_dispatch((sli_tz_veneer_fn)sli_tz_s_interface_dispatch_se_manager,
                                 (uint32_t)in_vec,
                                 IOVEC_LEN(in_vec),
                                 (uint32_t)out_vec,
                                 IOVEC_LEN(out_vec));

  // A PSA error is returned if parameter validation failed. Otherwise the
  // return status will be in the sl_status.
  if (psa_status != PSA_SUCCESS) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return sl_status;
}

/***************************************************************************//**
 * Enables the secure debug functionality.
 ******************************************************************************/
sl_status_t sl_se_enable_secure_debug(sl_se_command_context_t *cmd_ctx)
{
  sl_status_t sl_status;
  sli_tz_fn_id fn_id = SLI_TZ_SERVICE_SE_MANAGER_ENABLESECUREDEBUG_SID;
  psa_invec in_vec[] = {
    { .base = &fn_id, .len = sizeof(sli_tz_fn_id) },
    { cmd_ctx, sizeof(cmd_ctx) },
  };
  psa_outvec out_vec[] = {
    { &sl_status, sizeof(sl_status) },
  };

  psa_status_t psa_status =
    sli_tz_ns_interface_dispatch((sli_tz_veneer_fn)sli_tz_s_interface_dispatch_se_manager,
                                 (uint32_t)in_vec,
                                 IOVEC_LEN(in_vec),
                                 (uint32_t)out_vec,
                                 IOVEC_LEN(out_vec));

  // A PSA error is returned if parameter validation failed. Otherwise the
  // return status will be in the sl_status.
  if (psa_status != PSA_SUCCESS) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return sl_status;
}

/***************************************************************************//**
 * Disables the secure debug functionality.
 ******************************************************************************/
sl_status_t sl_se_disable_secure_debug(sl_se_command_context_t *cmd_ctx)
{
  sl_status_t sl_status;
  sli_tz_fn_id fn_id = SLI_TZ_SERVICE_SE_MANAGER_DISABLESECUREDEBUG_SID;
  psa_invec in_vec[] = {
    { .base = &fn_id, .len = sizeof(sli_tz_fn_id) },
    { cmd_ctx, sizeof(cmd_ctx) },
  };
  psa_outvec out_vec[] = {
    { &sl_status, sizeof(sl_status) },
  };

  psa_status_t psa_status =
    sli_tz_ns_interface_dispatch((sli_tz_veneer_fn)sli_tz_s_interface_dispatch_se_manager,
                                 (uint32_t)in_vec,
                                 IOVEC_LEN(in_vec),
                                 (uint32_t)out_vec,
                                 IOVEC_LEN(out_vec));

  // A PSA error is returned if parameter validation failed. Otherwise the
  // return status will be in the sl_status.
  if (psa_status != PSA_SUCCESS) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return sl_status;
}

/***************************************************************************//**
 * Set options on the debug interface.
 ******************************************************************************/
sl_status_t sl_se_set_debug_options(sl_se_command_context_t *cmd_ctx,
                                    const sl_se_debug_options_t *debug_options)
{
  sl_status_t sl_status;
  sli_tz_fn_id fn_id = SLI_TZ_SERVICE_SE_MANAGER_SETDEBUGOPTIONS_SID;
  psa_invec in_vec[] = {
    { .base = &fn_id, .len = sizeof(sli_tz_fn_id) },
    { cmd_ctx, sizeof(cmd_ctx) },
    { debug_options, sizeof(debug_options) },
  };
  psa_outvec out_vec[] = {
    { &sl_status, sizeof(sl_status) },
  };

  psa_status_t psa_status =
    sli_tz_ns_interface_dispatch((sli_tz_veneer_fn)sli_tz_s_interface_dispatch_se_manager,
                                 (uint32_t)in_vec,
                                 IOVEC_LEN(in_vec),
                                 (uint32_t)out_vec,
                                 IOVEC_LEN(out_vec));

  // A PSA error is returned if parameter validation failed. Otherwise the
  // return status will be in the sl_status.
  if (psa_status != PSA_SUCCESS) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return sl_status;
}

/***************************************************************************//**
 * Performs a device mass erase and debug unlock.
 ******************************************************************************/
sl_status_t sl_se_erase_device(sl_se_command_context_t *cmd_ctx)
{
  sl_status_t sl_status;
  sli_tz_fn_id fn_id = SLI_TZ_SERVICE_SE_MANAGER_ERASEDEVICE_SID;
  psa_invec in_vec[] = {
    { .base = &fn_id, .len = sizeof(sli_tz_fn_id) },
    { cmd_ctx, sizeof(cmd_ctx) },
  };
  psa_outvec out_vec[] = {
    { &sl_status, sizeof(sl_status) },
  };

  psa_status_t psa_status =
    sli_tz_ns_interface_dispatch((sli_tz_veneer_fn)sli_tz_s_interface_dispatch_se_manager,
                                 (uint32_t)in_vec,
                                 IOVEC_LEN(in_vec),
                                 (uint32_t)out_vec,
                                 IOVEC_LEN(out_vec));

  // A PSA error is returned if parameter validation failed. Otherwise the
  // return status will be in the sl_status.
  if (psa_status != PSA_SUCCESS) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return sl_status;
}

/***************************************************************************//**
 * Disabled device erase functionality.
 ******************************************************************************/
sl_status_t sl_se_disable_device_erase(sl_se_command_context_t *cmd_ctx)
{
  sl_status_t sl_status;
  sli_tz_fn_id fn_id = SLI_TZ_SERVICE_SE_MANAGER_DISABLEDEVICEERASE_SID;
  psa_invec in_vec[] = {
    { .base = &fn_id, .len = sizeof(sli_tz_fn_id) },
    { cmd_ctx, sizeof(cmd_ctx) },
  };
  psa_outvec out_vec[] = {
    { &sl_status, sizeof(sl_status) },
  };

  psa_status_t psa_status =
    sli_tz_ns_interface_dispatch((sli_tz_veneer_fn)sli_tz_s_interface_dispatch_se_manager,
                                 (uint32_t)in_vec,
                                 IOVEC_LEN(in_vec),
                                 (uint32_t)out_vec,
                                 IOVEC_LEN(out_vec));

  // A PSA error is returned if parameter validation failed. Otherwise the
  // return status will be in the sl_status.
  if (psa_status != PSA_SUCCESS) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return sl_status;
}

/***************************************************************************//**
 * Request challenge from SE which can be used to open debug access.
 ******************************************************************************/
sl_status_t sl_se_get_challenge(sl_se_command_context_t *cmd_ctx,
                                sl_se_challenge_t challenge)
{
  sl_status_t sl_status;
  sli_tz_fn_id fn_id = SLI_TZ_SERVICE_SE_MANAGER_GETCHALLENGE_SID;
  psa_invec in_vec[] = {
    { .base = &fn_id, .len = sizeof(sli_tz_fn_id) },
    { cmd_ctx, sizeof(cmd_ctx) },
  };
  psa_outvec out_vec[] = {
    { &sl_status, sizeof(sl_status) },
    { challenge, sizeof(sl_se_challenge_t) },
  };

  psa_status_t psa_status =
    sli_tz_ns_interface_dispatch((sli_tz_veneer_fn)sli_tz_s_interface_dispatch_se_manager,
                                 (uint32_t)in_vec,
                                 IOVEC_LEN(in_vec),
                                 (uint32_t)out_vec,
                                 IOVEC_LEN(out_vec));

  // A PSA error is returned if parameter validation failed. Otherwise the
  // return status will be in the sl_status.
  if (psa_status != PSA_SUCCESS) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return sl_status;
}

/***************************************************************************//**
 * Invalidate current challenge and make a new challenge.
 ******************************************************************************/
sl_status_t sl_se_roll_challenge(sl_se_command_context_t *cmd_ctx)
{
  sl_status_t sl_status;
  sli_tz_fn_id fn_id = SLI_TZ_SERVICE_SE_MANAGER_ROLLCHALLENGE_SID;
  psa_invec in_vec[] = {
    { .base = &fn_id, .len = sizeof(sli_tz_fn_id) },
    { cmd_ctx, sizeof(cmd_ctx) },
  };
  psa_outvec out_vec[] = {
    { &sl_status, sizeof(sl_status) },
  };

  psa_status_t psa_status =
    sli_tz_ns_interface_dispatch((sli_tz_veneer_fn)sli_tz_s_interface_dispatch_se_manager,
                                 (uint32_t)in_vec,
                                 IOVEC_LEN(in_vec),
                                 (uint32_t)out_vec,
                                 IOVEC_LEN(out_vec));

  // A PSA error is returned if parameter validation failed. Otherwise the
  // return status will be in the sl_status.
  if (psa_status != PSA_SUCCESS) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return sl_status;
}

/***************************************************************************//**
 * Unlock debug access using certificate signed with challenge.
 ******************************************************************************/
sl_status_t sl_se_open_debug(sl_se_command_context_t *cmd_ctx,
                             void *cert, uint32_t len,
                             const sl_se_debug_options_t *debug_options)
{
  sl_status_t sl_status;
  sli_tz_fn_id fn_id = SLI_TZ_SERVICE_SE_MANAGER_OPENDEBUG_SID;
  psa_invec in_vec[] = {
    { .base = &fn_id, .len = sizeof(sli_tz_fn_id) },
    { cmd_ctx, sizeof(cmd_ctx) },
    { cert, len },
    { debug_options, sizeof(debug_options) },
  };
  psa_outvec out_vec[] = {
    { &sl_status, sizeof(sl_status) },
  };

  psa_status_t psa_status =
    sli_tz_ns_interface_dispatch((sli_tz_veneer_fn)sli_tz_s_interface_dispatch_se_manager,
                                 (uint32_t)in_vec,
                                 IOVEC_LEN(in_vec),
                                 (uint32_t)out_vec,
                                 IOVEC_LEN(out_vec));

  // A PSA error is returned if parameter validation failed. Otherwise the
  // return status will be in the sl_status.
  if (psa_status != PSA_SUCCESS) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return sl_status;
}

#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)

/***************************************************************************//**
 * Writes data to User Data section in MTP. Write data must be aligned to
 * word size and contain a number of bytes that is divisable by four.
 ******************************************************************************/
sl_status_t sl_se_write_user_data(sl_se_command_context_t *cmd_ctx,
                                  uint32_t offset,
                                  void *data,
                                  uint32_t num_bytes)
{
  sl_status_t sl_status;
  sli_tz_fn_id fn_id = SLI_TZ_SERVICE_SE_MANAGER_WRITEUSERDATA_SID;
  psa_invec in_vec[] = {
    { .base = &fn_id, .len = sizeof(sli_tz_fn_id) },
    { cmd_ctx, sizeof(cmd_ctx) },
    { &offset, sizeof(offset) },
    { data, num_bytes },
  };
  psa_outvec out_vec[] = {
    { &sl_status, sizeof(sl_status) },
  };

  psa_status_t psa_status =
    sli_tz_ns_interface_dispatch((sli_tz_veneer_fn)sli_tz_s_interface_dispatch_se_manager,
                                 (uint32_t)in_vec,
                                 IOVEC_LEN(in_vec),
                                 (uint32_t)out_vec,
                                 IOVEC_LEN(out_vec));

  // A PSA error is returned if parameter validation failed. Otherwise the
  // return status will be in the sl_status.
  if (psa_status != PSA_SUCCESS) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return sl_status;
}

/***************************************************************************//**
 * Erases User Data section in MTP.
 ******************************************************************************/
sl_status_t sl_se_erase_user_data(sl_se_command_context_t *cmd_ctx)
{
  sl_status_t sl_status;
  sli_tz_fn_id fn_id = SLI_TZ_SERVICE_SE_MANAGER_ERASEUSERDATA_SID;
  psa_invec in_vec[] = {
    { .base = &fn_id, .len = sizeof(sli_tz_fn_id) },
    { cmd_ctx, sizeof(cmd_ctx) },
  };
  psa_outvec out_vec[] = {
    { &sl_status, sizeof(sl_status) },
  };

  psa_status_t psa_status =
    sli_tz_ns_interface_dispatch((sli_tz_veneer_fn)sli_tz_s_interface_dispatch_se_manager,
                                 (uint32_t)in_vec,
                                 IOVEC_LEN(in_vec),
                                 (uint32_t)out_vec,
                                 IOVEC_LEN(out_vec));

  // A PSA error is returned if parameter validation failed. Otherwise the
  // return status will be in the sl_status.
  if (psa_status != PSA_SUCCESS) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return sl_status;
}

/***************************************************************************//**
 * Read the EMU->RSTCAUSE after a tamper reset. This function should be called
 * if EMU->RSTCAUSE has been cleared upon boot.
 ******************************************************************************/
sl_status_t sl_se_get_reset_cause(sl_se_command_context_t *cmd_ctx,
                                  uint32_t* reset_cause)
{
  sl_status_t sl_status;
  sli_tz_fn_id fn_id = SLI_TZ_SERVICE_SE_MANAGER_GETRESETCAUSE_SID;
  psa_invec in_vec[] = {
    { .base = &fn_id, .len = sizeof(sli_tz_fn_id) },
    { cmd_ctx, sizeof(cmd_ctx) },
  };
  psa_outvec out_vec[] = {
    { &sl_status, sizeof(sl_status) },
    { reset_cause, sizeof(reset_cause) },
  };

  psa_status_t psa_status =
    sli_tz_ns_interface_dispatch((sli_tz_veneer_fn)sli_tz_s_interface_dispatch_se_manager,
                                 (uint32_t)in_vec,
                                 IOVEC_LEN(in_vec),
                                 (uint32_t)out_vec,
                                 IOVEC_LEN(out_vec));

  // A PSA error is returned if parameter validation failed. Otherwise the
  // return status will be in the sl_status.
  if (psa_status != PSA_SUCCESS) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return sl_status;
}

#endif // _SILICON_LABS_32B_SERIES_2_CONFIG_1

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)

/***************************************************************************//**
 * Temporarily disable tamper configuration using certificate signed with
 * challenge.
 ******************************************************************************/
sl_status_t sl_se_disable_tamper(sl_se_command_context_t *cmd_ctx,
                                 void *cert,
                                 uint32_t len,
                                 sl_se_tamper_signals_t tamper_signals)
{
  sl_status_t sl_status;
  sli_tz_fn_id fn_id = SLI_TZ_SERVICE_SE_MANAGER_DISABLETAMPER_SID;
  psa_invec in_vec[] = {
    { .base = &fn_id, .len = sizeof(sli_tz_fn_id) },
    { cmd_ctx, sizeof(cmd_ctx) },
    { cert, len },
    { &tamper_signals, sizeof(tamper_signals) },
  };
  psa_outvec out_vec[] = {
    { &sl_status, sizeof(sl_status) },
  };

  psa_status_t psa_status =
    sli_tz_ns_interface_dispatch((sli_tz_veneer_fn)sli_tz_s_interface_dispatch_se_manager,
                                 (uint32_t)in_vec,
                                 IOVEC_LEN(in_vec),
                                 (uint32_t)out_vec,
                                 IOVEC_LEN(out_vec));

  // A PSA error is returned if parameter validation failed. Otherwise the
  // return status will be in the sl_status.
  if (psa_status != PSA_SUCCESS) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return sl_status;
}

/***************************************************************************//**
 * Read size of stored certificates in SE.
 ******************************************************************************/
sl_status_t sl_se_read_cert_size(sl_se_command_context_t *cmd_ctx,
                                 sl_se_cert_size_type_t *cert_size)
{
  sl_status_t sl_status;
  sli_tz_fn_id fn_id = SLI_TZ_SERVICE_SE_MANAGER_READCERTSIZE_SID;
  psa_invec in_vec[] = {
    { .base = &fn_id, .len = sizeof(sli_tz_fn_id) },
    { cmd_ctx, sizeof(cmd_ctx) },
  };
  psa_outvec out_vec[] = {
    { &sl_status, sizeof(sl_status) },
    { cert_size, sizeof(cert_size) },
  };

  psa_status_t psa_status =
    sli_tz_ns_interface_dispatch((sli_tz_veneer_fn)sli_tz_s_interface_dispatch_se_manager,
                                 (uint32_t)in_vec,
                                 IOVEC_LEN(in_vec),
                                 (uint32_t)out_vec,
                                 IOVEC_LEN(out_vec));

  // A PSA error is returned if parameter validation failed. Otherwise the
  // return status will be in the sl_status.
  if (psa_status != PSA_SUCCESS) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return sl_status;
}

/***************************************************************************//**
 * Read stored certificates in SE.
 ******************************************************************************/
sl_status_t sl_se_read_cert(sl_se_command_context_t *cmd_ctx,
                            sl_se_cert_type_t cert_type,
                            void *cert,
                            uint32_t num_bytes)
{
  sl_status_t sl_status;
  sli_tz_fn_id fn_id = SLI_TZ_SERVICE_SE_MANAGER_READCERT_SID;
  psa_invec in_vec[] = {
    { .base = &fn_id, .len = sizeof(sli_tz_fn_id) },
    { cmd_ctx, sizeof(cmd_ctx) },
    { &cert_type, sizeof(cert_type) },
  };
  psa_outvec out_vec[] = {
    { &sl_status, sizeof(sl_status) },
    { cert, num_bytes },
  };

  psa_status_t psa_status =
    sli_tz_ns_interface_dispatch((sli_tz_veneer_fn)sli_tz_s_interface_dispatch_se_manager,
                                 (uint32_t)in_vec,
                                 IOVEC_LEN(in_vec),
                                 (uint32_t)out_vec,
                                 IOVEC_LEN(out_vec));

  // A PSA error is returned if parameter validation failed. Otherwise the
  // return status will be in the sl_status.
  if (psa_status != PSA_SUCCESS) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  return sl_status;
}

#endif // _SILICON_LABS_SECURITY_FEATURE_VAULT
#endif // SEMAILBOX_PRESENT
