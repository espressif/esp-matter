/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Device Query Service component.
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

/**
 * @defgroup device-query-service Device Query Service
 * @ingroup component
 * @brief API and Callbacks for the Device Query Service Component
 *
 * This component queries new devices for their endpoints and clusters
 * to record information in the Device Database component.
 */

/**
 * @addtogroup device-query-service
 * @{
 */

/**
 * @name API
 * @{
 */

/** @brief Enable / Disable the device query service
 *
 * @param enable enable/disbale value Ver.:Always
 *
 */
void emberAfPluginDeviceQueryServiceEnableDisable(bool enable);

/** @brief Device query service get state
 *
 * @return enable state
 */
bool emberAfPluginDeviceQueryServiceGetEnabledState(void);

/** @brief get EUI64 of current discovery target
 *
 * @param returnEui64 Ver.:Always
 *
 */
void emberAfPluginDeviceQueryServiceGetCurrentDiscoveryTargetEui64(EmberEUI64 returnEui64);

/** @} */ // end of name API
/** @} */ // end of device-query-service
