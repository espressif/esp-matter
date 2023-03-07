/***************************************************************************/ /**
 * @file
 * @brief Implementation for the Basic Server Cluster plugin.
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

#ifndef SILABS_ZIGBEE_BASIC_H
#define SILABS_ZIGBEE_BASIC_H

/**
 * @defgroup basic Basic Server
 * @ingroup component cluster
 * @brief API and Callbacks for the Basic Cluster Server Component
 *
 * Silicon Labs implementation of the Basic server cluster.
 * This component implements the optional ResetToFactoryDefaults
 * command, which allows any remote device in the network to reset
 * the local device. When the application receives this command,
 * the component resets attributes managed by the framework to their
 * default values and then calls a callback so that the application can
 * perform any other necessary reset-related operations.
 * The command is disruptive and not required, so Silicon Labs recommends
 * disabling this component for most production applications.
 *
 */

/**
 * @addtogroup basic
 * @{
 */

/**
 * @name Callbacks
 * @{
 */

/**
 * @defgroup basic_cb Basic
 * @ingroup af_callback
 * @brief Callbacks for Basic Component
 *
 */

/**
 * @addtogroup basic_cb
 * @{
 */
/** @brief Reset to factory defaults.
 *
 * This function is called by the Basic server plugin when a request to reset
 * to factory defaults is received. The plugin resets attributes managed by
 * the framework to their default values. The application should perform any
 * other necessary reset-related operations in this callback, including
 * resetting any externally-stored attributes.
 *
 * @param endpoint   Ver.: always
 */
void emberAfPluginBasicResetToFactoryDefaultsCallback(uint8_t endpoint);
/** @} */ // end of basic_cb
/** @} */ // end of callbacks
/** @} */ // end of group basic

#endif // SILABS_ZIGBEE_BASIC_H
