/***************************************************************************//**
 * @file
 * @brief Definitions for the ZLL Commissioning Common plugin.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "zll-commissioning.h"

/** @brief Initial Security State
 *
 * This function is called by the ZLL Commissioning Common plugin to determine the
 * initial security state to be used by the device. The application must
 * populate the ::EmberZllInitialSecurityState structure with a configuration
 * appropriate for the network being formed, joined, or started. Once the
 * device forms, joins, or starts a network, the same security configuration
 * will remain in place until the device leaves the network.
 *
 * @param securityState The security configuration to be populated by the
 * application and ultimately set in the stack. Ver.: always
 */
WEAK(void emberAfPluginZllCommissioningCommonInitialSecurityStateCallback(EmberZllInitialSecurityState *securityState))
{
}

/** @brief Touch Link Complete
 *
 * This function is called by the ZLL Commissioning Common plugin when touch linking
 * completes.
 *
 * @param networkInfo The ZigBee and ZLL-specific information about the network
 * and target. Ver.: always
 * @param deviceInformationRecordCount The number of sub-device information
 * records for the target. Ver.: always
 * @param deviceInformationRecordList The list of sub-device information
 * records for the target. Ver.: always
 */
WEAK(void emberAfPluginZllCommissioningCommonTouchLinkCompleteCallback(const EmberZllNetwork *networkInfo,
                                                                       uint8_t deviceInformationRecordCount,
                                                                       const EmberZllDeviceInfoRecord *deviceInformationRecordList))
{
}

/** @brief Reset To Factory New
 *
 * This function is called by the ZLL Commissioning Common plugin when a request to
 * reset to factory new is received. The plugin will leave the network, reset
 * attributes managed by the framework to their default values, and clear the
 * group and scene tables. The application should perform any other necessary
 * reset-related operations in this callback, including resetting any
 * externally-stored attributes.
 *
 */
WEAK(void emberAfPluginZllCommissioningCommonResetToFactoryNewCallback(void))
{
}
