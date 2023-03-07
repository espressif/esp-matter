/***************************************************************************//**
 * @file
 * @brief Silicon Labs TrustZone secure SE Manager service.
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

#ifndef __SLI_TZ_SERVICE_SE_MANAGER_H__
#define __SLI_TZ_SERVICE_SE_MANAGER_H__

#include "psa/client.h"

#include "em_device.h"

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Service function identifiers

/**
 * \brief Define a progressive numerical value for each SID which can be used
 *        when dispatching the requests to the service
 */
enum {
  SLI_TZ_SERVICE_SE_MANAGER_CHECKSEIMAGE_SID = (0u),
  SLI_TZ_SERVICE_SE_MANAGER_APPLYSEIMAGE_SID,
  SLI_TZ_SERVICE_SE_MANAGER_GETUPGRADESTATUSSEIMAGE_SID,
  SLI_TZ_SERVICE_SE_MANAGER_CHECKHOSTIMAGE_SID,
  SLI_TZ_SERVICE_SE_MANAGER_APPLYHOSTIMAGE_SID,
  SLI_TZ_SERVICE_SE_MANAGER_GETUPGRADESTATUSHOSTIMAGE_SID,
  SLI_TZ_SERVICE_SE_MANAGER_INITOTPKEY_SID,
  SLI_TZ_SERVICE_SE_MANAGER_READPUBKEY_SID,
  SLI_TZ_SERVICE_SE_MANAGER_INITOTP_SID,
  SLI_TZ_SERVICE_SE_MANAGER_READOTP_SID,
  SLI_TZ_SERVICE_SE_MANAGER_GETSEVERSION_SID,
  SLI_TZ_SERVICE_SE_MANAGER_GETDEBUGLOCKSTATUS_SID,
  SLI_TZ_SERVICE_SE_MANAGER_APPLYDEBUGLOCK_SID,

#if defined(CRYPTOACC_PRESENT)
  SLI_TZ_SERVICE_SE_MANAGER_READEXECUTEDCOMMAND_SID,
  SLI_TZ_SERVICE_SE_MANAGER_ACKCOMMAND_SID,
#endif // CRYPTOACC_PRESENT

#if defined(SEMAILBOX_PRESENT)
  SLI_TZ_SERVICE_SE_MANAGER_GETSTATUS_SID,
  SLI_TZ_SERVICE_SE_MANAGER_GETSERIALNUMBER_SID,
  SLI_TZ_SERVICE_SE_MANAGER_GETOTPVERSION_SID,
  SLI_TZ_SERVICE_SE_MANAGER_ENABLESECUREDEBUG_SID,
  SLI_TZ_SERVICE_SE_MANAGER_DISABLESECUREDEBUG_SID,
  SLI_TZ_SERVICE_SE_MANAGER_SETDEBUGOPTIONS_SID,
  SLI_TZ_SERVICE_SE_MANAGER_ERASEDEVICE_SID,
  SLI_TZ_SERVICE_SE_MANAGER_DISABLEDEVICEERASE_SID,
  SLI_TZ_SERVICE_SE_MANAGER_GETCHALLENGE_SID,
  SLI_TZ_SERVICE_SE_MANAGER_ROLLCHALLENGE_SID,
  SLI_TZ_SERVICE_SE_MANAGER_OPENDEBUG_SID,
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
  SLI_TZ_SERVICE_SE_MANAGER_WRITEUSERDATA_SID,
  SLI_TZ_SERVICE_SE_MANAGER_ERASEUSERDATA_SID,
  SLI_TZ_SERVICE_SE_MANAGER_GETRESETCAUSE_SID,
#endif // _SILICON_LABS_32B_SERIES_2_CONFIG_1
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  SLI_TZ_SERVICE_SE_MANAGER_DISABLETAMPER_SID,
  SLI_TZ_SERVICE_SE_MANAGER_READCERTSIZE_SID,
  SLI_TZ_SERVICE_SE_MANAGER_READCERT_SID,
#endif // _SILICON_LABS_SECURITY_FEATURE_VAULT
#endif // SEMAILBOX_PRESENT

  SLI_TZ_SERVICE_SE_MANAGER_MAX_SID,
};

//------------------------------------------------------------------------------
// Function declarations

psa_status_t sli_tz_se_check_se_image(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t sli_tz_se_apply_se_image(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t sli_tz_se_get_upgrade_status_se_image(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t sli_tz_se_check_host_image(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t sli_tz_se_apply_host_image(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t sli_tz_se_get_upgrade_status_host_image(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t sli_tz_se_init_otp_key(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t sli_tz_se_read_pubkey(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t sli_tz_se_init_otp(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t sli_tz_se_read_otp(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t sli_tz_se_get_se_version(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t sli_tz_se_get_debug_lock_status(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t sli_tz_se_apply_debug_lock(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);

// VSE only.
#if defined(CRYPTOACC_PRESENT)
psa_status_t sli_tz_se_read_executed_command(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t sli_tz_se_ack_command(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
#endif // CRYPTOACC_PRESENT

// HSE only.
#if defined(SEMAILBOX_PRESENT)
psa_status_t sli_tz_se_get_status(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t sli_tz_se_get_serialnumber(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t sli_tz_se_get_otp_version(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t sli_tz_se_enable_secure_debug(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t sli_tz_se_disable_secure_debug(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t sli_tz_se_set_debug_options(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t sli_tz_se_erase_device(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t sli_tz_se_disable_device_erase(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t sli_tz_se_get_challenge(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t sli_tz_se_roll_challenge(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t sli_tz_se_open_debug(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
psa_status_t sli_tz_se_write_user_data(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t sli_tz_se_erase_user_data(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t sli_tz_se_get_reset_cause(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
#endif // _SILICON_LABS_32B_SERIES_2_CONFIG_1
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
psa_status_t sli_tz_se_disable_tamper(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t sli_tz_se_read_cert_size(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
psa_status_t sli_tz_se_read_cert(psa_invec *in_vec, size_t in_len, psa_outvec *out_vec, size_t out_len);
#endif // _SILICON_LABS_SECURITY_FEATURE_VAULT
#endif // SEMAILBOX_PRESENT

#ifdef __cplusplus
}
#endif

#endif // __SLI_TZ_SERVICE_SE_MANAGER_H__
