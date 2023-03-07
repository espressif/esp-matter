/***************************************************************************//**
 * @file: multirail-demo.h
 * @brief Prototypes of routines to utilise a second instance of RAIL.
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

#ifndef MULTIRAIL_DEMO_H
#define MULTIRAIL_DEMO_H

#include "rail.h"
#include "rail_types.h"

/**
 * @defgroup multirail-demo  Multi Rail Demo
 * @ingroup component
 * @brief API and Callbacks for the Multi Rail Demo Component
 *
 * This component provides the sample code for using dual RAIL instances. It includes
 * a simple CLI to demonstrate the functionality.
 *
 */

/**
 * @addtogroup multirail-demo
 * @{
 */

/**
 * @name API
 * @{
 */

/*
 * Note that these functions are provided for your convenience.
 * You can override any of them by using your own implementation.
 */

/** @brief Initialize a new RAIL handle.
 *
 * @param railCfg RAIL init options (NULL to use the current Zigbee config)
 * @param txPowerConfig RAIL power options (NULL to use defaults)
 * @param paAutoMode Whether an automatic PA mode is enabled
 * @param defaultTxPower Default TX power in deci-dBm
 * @param txFifo Pointer to a TX FIFO buffer (NULL to use the default)
 * @param txFifoSize Size of the TX buffer
 * @param panId PAN ID (use 0xFFFF if not needed)
 * @param ieeeAddr Long IEEE address (use NULL if not needed)
 *
 * @return New RAIL handle, NULL if not successfully initialized.
 */
RAIL_Handle_t emberAfPluginMultirailDemoInit(RAIL_Config_t *railCfg,
                                             RAIL_TxPowerConfig_t *txPowerConfig,
                                             bool paAutoMode,
                                             RAIL_TxPower_t defaultTxPower,
                                             uint8_t *txFifo,
                                             uint16_t txFifoSize,
                                             uint16_t panId,
                                             const uint8_t *ieeeAddr);

/** @brief Return the second RAIL handle.
 *
 * @return The second RAIL handle. Null if not initialized.
 */
RAIL_Handle_t emberAfPluginMultirailDemoGetHandle(void);

/** @brief Send a packet using the second RAIL instance.
 *
 * @param buff Pointer to the data (make sue it is properly formatted)
 * @param size Size of the data
 * @param channel Channel (11-26, or 0 to use the current Zigbee channel)
 * @param scheduledTxConfig (optional)
 * @param schedulerInfo (optional) Scheduler configuration (NULL to use defaults)
 *
 * @return RAIL status.
 */
RAIL_Status_t emberAfPluginMultirailDemoSend(const uint8_t *buff,
                                             uint32_t size,
                                             uint8_t channel,
                                             RAIL_ScheduleTxConfig_t *scheduledTxConfig,
                                             RAIL_SchedulerInfo_t *schedulerInfo);

/** @} */ // end of name API

/**
 * @name Callbacks
 * @{
 */

/**
 * @defgroup multirail_demo_cb Multirail Demo
 * @ingroup af_callback
 * @brief Callbacks for Multirail Demo Component
 *
 */

/**
 * @addtogroup multirail_demo_cb
 * @{
 */

/** @brief A callback called whenever a secondary instance RAIL event occurs.
 *
 * @param[in] handle A handle for a RAIL instance.
 * @param[in] events A bit mask of RAIL events (full list in rail_types.h)
 */
void emberAfPluginMultirailDemoRailEventCallback(RAIL_Handle_t handle,
                                                 RAIL_Events_t events);

/** @} */ // end of multirail_demo_cb
/** @} */ // end of name Callbacks
/** @} */ // end of multirail-demo

#endif // MULTIRAIL_DEMO_H
