/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "af.h"

/** @brief Secure Ezsp Init
 *
 * This function is called when the Secure EZSP initializes.
 *
 * @param ncpSecurityKeyStatus Status of the NCP key: SET or UNSET Ver.: always
 * @param hostSecurityKeyStatus Status of the Host key: SET or UNSET Ver.: always
 * @param ncpSecurityType Type of NCP key: Temporary or Permanent Ver.: always
 */
WEAK(void emberSecureEzspInitCallback(EzspStatus ncpSecurityKeyStatus,
                                      EzspStatus hostSecurityKeyStatus,
                                      SecureEzspSecurityType ncpSecurityType))
{
}
