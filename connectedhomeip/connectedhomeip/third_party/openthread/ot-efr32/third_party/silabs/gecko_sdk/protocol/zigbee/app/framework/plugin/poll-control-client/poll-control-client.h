/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Poll Control Client plugin.
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
 * @defgroup poll-control-client Poll Control Client
 * @ingroup component cluster
 * @brief API and Callbacks for the Poll Control Cluster Client Component
 *
 * Silicon Labs implementation of the Poll Control client cluster. The component
 * responds in kind to check-in commands from the paired Poll Control server.
 *
 */

/**
 * @addtogroup poll-control-client
 * @{
 */

/** @} */ // end of poll-control-client

// Set fast polling mode
void emAfSetFastPollingMode(bool mode);

// Set fast polling timeout
void emAfSetFastPollingTimeout(uint16_t timeout);

// Set response mode
void emAfSetResponseMode(bool mode);

// Print mode and timeout
void emAfPollControlClientPrint(void);
