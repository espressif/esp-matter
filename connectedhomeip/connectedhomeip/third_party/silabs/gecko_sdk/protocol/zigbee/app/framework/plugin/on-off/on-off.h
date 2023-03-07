/***************************************************************************//**
 * @file
 * @brief Routines for the On-Off plugin, which implements the On-Off server
 *        cluster.
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

#ifndef SILABS_ZIGBEE_ON_OFF_H
#define SILABS_ZIGBEE_ON_OFF_H

/**
 * @defgroup on-off On / Off Server
 * @ingroup component cluster
 * @brief API and Callbacks for the On/Off Cluster Server Component
 *
 * Silicon Labs implementation of the On/Off server cluster.
 * If the ZLL On/Off server cluster component is also enabled, this component
 * uses it for handling ZLL enhancements. Otherwise, all ZLL extensions
 * are disabled. This component requires extending to integrate the software
 * into the hardware that the on/off signals will control.
 *
 */

/**
 * @addtogroup on-off
 * @{
 */

/**
 * @name Callbacks
 * @{
 */

/**
 * @defgroup on_off_cb On Off
 * @ingroup af_callback
 * @brief Callbacks for On Off Component
 *
 */

/**
 * @addtogroup on_off_cb
 * @{
 */

/** @brief On/off cluster server post init.
 *
 * Following the resolution of the on/off state at startup for this endpoint, perform any
 * additional initialization needed, e.g., synchronize hardware state.
 *
 * @param endpoint Endpoint that is being initialized  Ver.: always
 */
void emberAfPluginOnOffClusterServerPostInitCallback(uint8_t endpoint);

/** @} */ // end of on_off_cb
/** @} */ // end of name Callbacks
/** @} */ // end of on-off

#endif // SILABS_ZIGBEE_ON_OFF_H
