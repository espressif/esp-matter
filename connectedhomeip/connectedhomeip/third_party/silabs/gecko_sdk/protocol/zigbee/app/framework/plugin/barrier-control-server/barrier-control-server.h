/***************************************************************************/ /**
 * @file
 * @brief APIs for the Barrier Control Server plugin.
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
 * @defgroup barrier-control-server Barrier Control Server
 * @ingroup component cluster
 * @brief API and Callbacks for the Barrier Control Cluster Server Component
 *
 * Silicon Labs implementation of the Barrier Control server cluster.
 *
 */

/**
 * @addtogroup barrier-control-server
 * @{
 */

// No public API or Callbacks to document currently

/** @} */ // end of barrier-control-server

// There are helper getter/setting APIs that are shared between the core
// implementation and the CLI code. They are private to the plugin.

// This will always either return the current BarrierPosition attribute value
// or assert.
uint8_t emAfPluginBarrierControlServerGetBarrierPosition(uint8_t endpoint);

// This will always either set the current BarrierPosition attribute value or
// assert.
void emAfPluginBarrierControlServerSetBarrierPosition(uint8_t endpoint,
                                                      uint8_t barrierPosition);

// This will either return whether or not the PartialBarrier bit is set in the
// Capabilities attribute value, or it will assert.
bool emAfPluginBarrierControlServerIsPartialBarrierSupported(uint8_t endpoint);

// This will increment the OpenEvents, CloseEvents, CommandOpenEvents, and
// CommandCloseEvents attribute values depending on which combination of the
// open and command arguments are passed, or assert.
void emAfPluginBarrierControlServerIncrementEvents(uint8_t endpoint,
                                                   bool open,
                                                   bool command);

// This will read the SafetyStatus attribute and return the value, or assert.
uint16_t emAfPluginBarrierControlServerGetSafetyStatus(uint8_t endpoint);

// We use a minimum delay so that our barrier changes position in a realistic
// amount of time.
#define MIN_POSITION_CHANGE_DELAY_MS 30
